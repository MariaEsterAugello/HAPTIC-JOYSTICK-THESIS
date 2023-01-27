/* udpsend.c
	
	This S-function sends one or more input values to a remote address via UDP protocol.
	Both Windows and Linux RTAI environments are supported
	
	NOTE: to compile this function inside Windows you must UNCOMMENT the statement <#define WINDOWS_TARGET> few lines below
	
	Author: Roberto Mati (mati@dsea.unipi.it)
	
	October 2006,
	version 1.0

*/


#define S_FUNCTION_NAME  udpsend
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#define WINDOWS_TARGET		// define to generate code for Windows environment

//#define DEBUG_MODE

#ifdef WINDOWS_TARGET	// Windows header files

#include <stdio.h>
#include <winsock.h>
#include <errno.h>
#include <string.h>

#else					// RTAI header files

#ifndef MATLAB_MEX_FILE

#ifndef KEEP_STATIC_INLINE
#define KEEP_STATIC_INLINE
#endif

#include <unistd.h>	
#include <stdio.h>
#include <errno.h>	
#include <string.h>	
#include <sys/socket.h>	
#include <resolv.h>	
#include <arpa/inet.h>
#include <fcntl.h>
#include <rtai_user.h>

#include <rtai_lxrt_user.h>
#include <rtnet_lxrt.h>

#endif			// MATLAB_MEX_FILES
#endif			// header files

/*****************************/
/*SETUP THE INPUT PARAMETERS */
/*****************************/

#define NUM_PARAMS			4

#define LOCAL_IP_INDEX		0
#define REMOTE_IP_INDEX		1
#define REMOTE_PORT_INDEX	2
#define SAMPLE_TIME_INDEX	3

#define REMOTE_IP_PARAM		ssGetSFcnParam(S,REMOTE_IP_INDEX)
#define REMOTE_PORT_PARAM   ssGetSFcnParam(S,REMOTE_PORT_INDEX)
#define SAMPLE_TIME_PARAM   ssGetSFcnParam(S,SAMPLE_TIME_INDEX)
#define LOCAL_IP_PARAM		ssGetSFcnParam(S,LOCAL_IP_INDEX)		// to be used only in RTAI

#define REMOTE_PORT			(short) mxGetScalar(REMOTE_PORT_PARAM)
#define SAMPLE_TIME			(double)mxGetScalar(SAMPLE_TIME_PARAM)

/*****************/
/*USER'S DEFINES */
/*****************/

//input index
#define INPUT_SIGNAL		0

//signal dimension size
#define SIGNAL_SIZE			ssGetInputPortWidth(S, INPUT_SIGNAL)

#define	BUFFER_LENGTH		102400	// max size for send buffer

/**********************/
/*WORK VECTOR DEFINES */
/**********************/

//--index for manage IWork
#define SOCK_ID                 0
#define PACKET_ID               1

#ifdef MATLAB_MEX_FILE
#define MDL_CHECK_PARAMETERS
static void mdlCheckParameters(SimStruct *S) 
{
	mxArray *mxPort		= REMOTE_PORT_PARAM;
	mxArray *mxSample	= SAMPLE_TIME_PARAM;
	real_T	*pr;

	if (!mxIsChar(REMOTE_IP_PARAM)) 
    {
		ssSetErrorStatus(S, "Remote IP must be a string (dotted quad notation)");
        return;
	}		

	pr = mxGetPr(mxPort);
	if  (!mxIsDouble(mxPort) || (mxGetNumberOfElements(mxPort) != 1) || (pr[0] != (int_T)(pr[0])) || (pr[0] < 0))
	{	
		ssSetErrorStatus (S, "Remote PORT must be an unsigned int");
	    return;
	}

	pr = mxGetPr(mxSample);
	if (!mxIsDouble(mxSample) || (mxGetNumberOfElements(mxSample) != 1) || ((pr[0] < 0) && (pr[0] != -1)))
	{
		ssSetErrorStatus(S,"Sample time must be a positive double precision number");
        return;
	}

} // end of mdlCheckParameters
#endif


static void mdlInitializeSizes(SimStruct *S)
{
	#ifdef DEBUG_MODE
	printf("[From udpsend] Starting mdlInizializeSizes\n");
	#endif
	
	ssSetNumSFcnParams(S, NUM_PARAMS);      

	#if defined(MATLAB_MEX_FILE)

    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S))
		return;
	
	mdlCheckParameters(S);
	
    if (ssGetErrorStatus(S) != NULL) return;
	#endif
		
	ssSetNumContStates(S, 0);
 	ssSetNumDiscStates(S, 0);
	
	if (!ssSetNumInputPorts(S, 1))
		return;
	ssSetInputPortWidth(S, INPUT_SIGNAL, DYNAMICALLY_SIZED);

    
	if (!ssSetNumOutputPorts(S, 0)) 
		return;
	
    ssSetInputPortDirectFeedThrough(S, 0, 1); 		
    
    ssSetNumSampleTimes(S, 1);

    ssSetNumRWork(S, 0);
    ssSetNumPWork(S, 1);    // a pointer for buffer

	#ifdef WINDOWS_TARGET	// Windows work vectors
	
    ssSetNumIWork(S, 1); 
	
	#else					// RTAI work vectors
		
    ssSetNumIWork(S, 3);
    ssSetNumDWork(S, 0);
   
	#endif					// work vectors

    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs( S, 0);
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);


	#ifdef DEBUG_MODE
	printf("[From udpsend] Ending mdlInizializeSizes\n");
	#endif

} /* end mdlInitializeSizes */


static void mdlInitializeSampleTimes(SimStruct *S)
{
	#ifdef DEBUG_MODE
	printf("[From udpsend] Starting mdlInizializeSampleTimes\n");
	#endif

    ssSetSampleTime(S, 0, SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);

	#ifdef DEBUG_MODE
	printf("[From udpsend] Ending mdlInizializeSampleTimes\n");
	#endif
	    
} /* end mdlInitializeSampleTimes */


#define MDL_START
#if defined(MDL_START)
static void mdlStart(SimStruct *S) 
{

	#define MAX_DIM_BUFFER 17

	char 				remote_ip[MAX_DIM_BUFFER];
	int 				i;
	int 				sockfd;
    real_T 				*buffer;


#ifdef WINDOWS_TARGET		// variables inside Windows

	struct sockaddr_in	remote;
	struct hostent  	*host;
	unsigned long		hostaddr;

	WORD	wVersionRequested = MAKEWORD(1, 1); 
	WSADATA wsaData; 
	int		err = WSAStartup(wVersionRequested, &wsaData); 


#else				// variables inside RTAI
        
	char	local_ip[MAX_DIM_BUFFER];
    int     ret = 0;

    void 	**PWork = ssGetPWork(S);

    #ifndef MATLAB_MEX_FILE
    struct	sockaddr_in remote;
	struct	sockaddr_in local;
    #endif
    
#endif				// variables	    
    
	#ifdef DEBUG_MODE
	printf("[From udpsend] Starting mdlStart\n");
	#endif     
    
#ifdef WINDOWS_TARGET	// mdlStart Windows code

	#ifdef MATLAB_MEX_FILE
	
	
	#ifdef WINDOWS_TARGET		// execute only inside Windows environment 	
	
	if (err != 0) return; 
	ssSetIWorkValue(S, 0, 0);
	mxGetString(REMOTE_IP_PARAM,remote_ip,MAX_DIM_BUFFER);

	if ((host = gethostbyname(remote_ip)) == 0)
	{
		// gethostbyname has failed, try by addr
		if ((hostaddr = inet_addr(remote_ip)) == INADDR_NONE) 
		{
			// inet_addr has failed as well
			printf("udpsend - Unable to resolve host name. Error :%d\n", WSAGetLastError());

			ssSetErrorStatus(S,"udpsend - Unable to resolve host name");
			ssSetStopRequested(S, 1);	// stop simulation
			return;		

		}
		if ((host = gethostbyaddr((const char *)hostaddr, sizeof(hostaddr), AF_INET)) == 0)
		{
			// all tries have failed
			printf("udpsend - Unable to resolve host name. Error :%d\n",WSAGetLastError());

			ssSetErrorStatus(S,"udpsend - Unable to resolve host name");
			ssSetStopRequested(S, 1);	// stop simulation
			return;		
		}
	}
	// prepare socket	
	memcpy((char *)&remote.sin_addr, host->h_addr, host->h_length);
	remote.sin_family = AF_INET;
	for (i = 0; i < 8; i++) remote.sin_zero[i] = 0;
	remote.sin_port = htons(REMOTE_PORT);
	if ((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("udpsend - Socket creation error :%d\n",WSAGetLastError());
		
		ssSetErrorStatus(S,"udpsend - Socket creation error");
		ssSetStopRequested(S, 1);	// stop simulation
		return;		
	}

		
	buffer = (real_T*) calloc(SIGNAL_SIZE, sizeof(real_T));

	setsockopt (sockfd, SOL_SOCKET, SO_SNDBUF , (const char *)&buffer, sizeof(real_T));

	if (connect(sockfd, (const struct sockaddr *)&remote, sizeof(struct sockaddr)) != 0) 
	{
		printf("udpsend - Socket send error :%d\n", WSAGetLastError());
			
		ssSetErrorStatus(S,"udpsend - Socket send error");
		ssSetStopRequested(S, 1);	// stop simulation
		return;		
	}

	ssSetIWorkValue(S, 0, sockfd);
	
	#endif				// WINDOWS_TARGET
	
	#endif				// MATLAB_MEX_FILE    
    
#else			// mdlStart RTAI code
    
    mxGetString(REMOTE_IP_PARAM, remote_ip, MAX_DIM_BUFFER);
    mxGetString(LOCAL_IP_PARAM, local_ip, MAX_DIM_BUFFER);   
        
    //DEBUG INFOs......
    printf ("Remote IP   : %s\n ",remote_ip);
    printf ("Remote Port : %d\n ",REMOTE_PORT);
    //.......END DEBUG


	#ifndef MATLAB_MEX_FILE
	
    // allocation of buffer
    buffer = (real_T*) calloc(SIGNAL_SIZE, sizeof(real_T));
    
    // for local socket
    
    memset (&local, 0, sizeof(struct sockaddr_in) );
    
    // setup SOCKADDR STRUCT parameters
    local.sin_family = AF_INET;
    local.sin_port = htons(25000);
	inet_aton(local_ip, &local.sin_addr);

    //For remote socket:=================
    memset (&remote, 0, sizeof(struct sockaddr_in) );
    
    //  setup SOCKADDR STRUCT parameters
    remote.sin_family = AF_INET;
    remote.sin_port = htons(REMOTE_PORT);
    inet_aton(remote_ip, &remote.sin_addr);

    // create the UDP socket
    sockfd = rt_socket(AF_INET,SOCK_DGRAM, 0);
    
    // set as non-blocking socket
    fcntl(sockfd, F_SETFL,O_NONBLOCK);


    // bind the socket
	if ( rt_socket_bind(sockfd, (struct sockaddr*) &local, sizeof(local)) != 0 ) 
	{
		printf ("Error during socket binding: return value %d\n", ret);
		ssSetStopRequested(S, 1);	// stop simulation
		return;
	}

    //connect the socket
    if ( (ret = rt_socket_connect(sockfd, (struct sockaddr*)&remote, sizeof(struct sockaddr_in))) < 0)
	{
		printf("Error during socket connecting: return value %d\n", ret);
		ssSetStopRequested(S, 1);	// stop simulation
		return;
	}

	// store socket descriptor into the WorkVector
        ssSetIWorkValue(S, SOCK_ID, sockfd );              

	#endif	
        
    //Initial conditions
    ssSetIWorkValue (S,PACKET_ID, 0);
    
#endif			// mdlStart code
    
    ssSetPWorkValue (S, 0, buffer);
    
	#ifdef DEBUG_MODE
	printf("[From udpsend] Ending mdlStart\n");
	#endif
    
}
#endif


static void mdlOutputs(SimStruct *S, int_T tid) 
{
    InputRealPtrsType	input	= ssGetInputPortRealSignalPtrs(S,INPUT_SIGNAL);
    int_T 				i		= 0;
    int_T 				ret 	= 0; 
    int_T 				sockfd  = ssGetIWorkValue(S, SOCK_ID);
    void				**PWork = ssGetPWork(S);
    real_T				*buffer = (real_T*) PWork[0];

#ifdef WINDOWS_TARGET		// variables inside Windows

	fd_set set;

#else						// variables inside RTAI
    	
    
    #ifndef MATLAB_MEX_FILE
        
    // packet used during communication
    struct PACKET_STR pack_tx,pack_rx;
    
    #endif
    
#endif						// variables    
    
#ifdef WINDOWS_TARGET		// mdlOutputs Windows code    
    
	FD_ZERO(&set);
	FD_SET(sockfd, &set);
	ret = select(NULL, NULL, &set, NULL, NULL);

	for (i = 0; i < SIGNAL_SIZE; i++)
		buffer[i] = *input[i];

	ret = send(sockfd, (const char FAR *)buffer, sizeof(real_T) * SIGNAL_SIZE, 0);

#else				// mdlOutputs RTAI code
	
    #ifndef MATLAB_MEX_FILE
    
	memset (&pack_tx, 0, sizeof(struct PACKET_STR) );
		
	pack_tx.packet_id = ssGetIWorkValue(S, PACKET_ID);
		
	ssSetIWorkValue(S, PACKET_ID, (pack_tx.packet_id + 1) );
	    
    //get number of input signals
    for (i = 0; i < SIGNAL_SIZE; i++) 
    {
        buffer[i] = *(input[i]);
    }

    //send whole input vector
    ret = rt_socket_send (sockfd, buffer, sizeof(real_T) * SIGNAL_SIZE, 0); 

    if (ret <= 0) 
	{ 
    	//clear data
    	memset (&pack_tx, 0, sizeof(struct PACKET_STR) );	    
    }
	
   	#endif	// MATLAB_MEX_FILE
    	
#endif			// mdlOutputs code
} /* end mdlOutputs */


static void mdlTerminate(SimStruct *S)
{
	
	int sockfd = ssGetIWorkValue(S, SOCK_ID);	
	
#ifdef WINDOWS_TARGET		// variables inside Windows


#else				// variables inside RTAI	
	
    void **PWork = ssGetPWork(S);

#endif				// variables

	#ifdef DEBUG_MODE
	printf("[From udpsend] Starting mdlTerminate\n");
	#endif

#ifdef WINDOWS_TARGET		// mdlTerminate Windows code

	shutdown(sockfd, 0x01);
	closesocket(sockfd);
	WSACleanup();

#else
	
	#ifndef MATLAB_MEX_FILE
	
	if ( rt_socket_close(sockfd) < 0 )
	    printf ("Error closing the socket!\n");
	#endif

    free(PWork[0]);
    ssSetPWorkValue(S, 0, NULL);
#endif				// mdlTerminate code

	#ifdef DEBUG_MODE
	printf("[From udpsend] Ending mdlTerminate\n");
	#endif


} /* end mdlTerminate */



#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
