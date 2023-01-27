/* udpreceive.c
	
	This S-function receives one or more values from a remote address via UDP protocol.
	Both Windows and Linux RTAI environments are supported
	
	NOTE: to compile this function inside Windows you must UNCOMMENT the statement <#define WINDOWS_TARGET> few lines below
	
	Author: Roberto Mati (mati@dsea.unipi.it)
	
	October 2006,
	version 1.0

*/


#define S_FUNCTION_NAME  udpreceive
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

#define NUM_PARAMS          4

#define LOCAL_IP_INDEX		0
#define LOCAL_PORT_INDEX	1
#define SAMPLE_TIME_INDEX	2
#define OUTPUT_SIZE_INDEX	3

#define LOCAL_IP_PARAM		ssGetSFcnParam(S,LOCAL_IP_INDEX)
#define LOCAL_PORT_PARAM	ssGetSFcnParam(S,LOCAL_PORT_INDEX)
#define SAMPLE_TIME_PARAM	ssGetSFcnParam(S,SAMPLE_TIME_INDEX)
#define OUTPUT_SIZE_PARAM	ssGetSFcnParam(S,OUTPUT_SIZE_INDEX)

#define LOCAL_PORT			(short)mxGetScalar(LOCAL_PORT_PARAM)

#define SAMPLE_TIME			(real_T)mxGetScalar(SAMPLE_TIME_PARAM)
#define OUTPUT_SIZE			(int_T)mxGetScalar(OUTPUT_SIZE_PARAM)

/*****************/
/*USER'S DEFINES */
/*****************/

#define NUM_OUTPUT_PORTS		2

//output IDs
#define OUTPUT_SIGNAL           0
#define SYNCHRO                 1

//flag dimension size
#define FLAG_SIZE               1

//signal dimension size

// set a 100Kb receive buffer
#define BUFFER_LENGTH	102400	

/**********************/
/*WORK VECTOR DEFINES */
/**********************/

//--index for manage IWork
#define SOCK_ID                 0


//--INDEX debug data collector

#define RX_PACK_ID		0
#define	RX_INTERVAL		1
#define	RX_DATA			2
#define PCK_FLUSH		3

#ifdef MATLAB_MEX_FILE
#define MDL_CHECK_PARAMETERS

static void mdlCheckParameters(SimStruct *S)
{
	mxArray *mxPort			= LOCAL_PORT_PARAM;
	mxArray *mxSample		= SAMPLE_TIME_PARAM;
	mxArray *mxOutputSize	= OUTPUT_SIZE_PARAM;
	real_T	*pr;

	if (!mxIsChar(LOCAL_IP_PARAM)) 
    {
		ssSetErrorStatus(S, "Local IP must be a string (dotted quad notation)");
        return;
	}		

	pr = mxGetPr(mxPort);
	
	if  (!mxIsDouble(mxPort) || (mxGetNumberOfElements(mxPort) != 1) || (pr[0] != (int_T)(pr[0])) || (pr[0] < 0))
	{	
		ssSetErrorStatus (S, "Local PORT must be an unsigned int");
	    return;
	}

	pr = mxGetPr(mxSample);
	if (!mxIsDouble(mxSample) || (mxGetNumberOfElements(mxSample) != 1) || ((pr[0] < 0) && (pr[0] != -1)))
	{
		ssSetErrorStatus(S,"Sample time must be a positive double precision number");
        return;
	}

	pr = mxGetPr(mxOutputSize);
	
	if  (!mxIsDouble(mxOutputSize) || (mxGetNumberOfElements(mxOutputSize) != 1) || (pr[0] != (int_T)(pr[0])) || (pr[0] < 0))
	{	
		ssSetErrorStatus (S, "Output size must be an unsigned int");
	    return;
	}



} // end of mdlCheckParameters
#endif


static void mdlInitializeSizes(SimStruct *S)
{
	#ifdef DEBUG_MODE
	printf("[From udpreceive] Starting mdlInizializeSizes\n");
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
	
	if (!ssSetNumInputPorts(S,0))
		return;
	
	    
	if (!ssSetNumOutputPorts(S, NUM_OUTPUT_PORTS)) 
		return;

    ssSetOutputPortWidth(S, OUTPUT_SIGNAL, OUTPUT_SIZE);	// signal output
    ssSetOutputPortWidth(S, SYNCHRO, FLAG_SIZE);			// synchro flag
	
    ssSetNumSampleTimes(S, 1);

	ssSetNumRWork(S, 0);	// common work vectors

	#ifdef WINDOWS_TARGET	// Windows work vectors

    ssSetNumIWork(S, 1); 
    ssSetNumPWork(S, 0); 

	#else					// RTAI work vectors

    ssSetNumIWork(S, 3);
    ssSetNumPWork(S, 2);	// pointers for bufferR and bufferO
	
	#endif					// work vectors
	
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);

	#ifdef DEBUG_MODE
	printf("[From udpreceive] Ending mdlInizializeSizes\n");
	#endif

} /* end mdlInitializeSizes */

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_OUTPUT_PORT_DIMENSION_INFO
void mdlSetOutputPortDimensionInfo(SimStruct *S, int_T port, const DimsInfo_T *dimsInfo)
{
    if(!ssSetOutputPortDimensionInfo(S, port, dimsInfo)) return;
}
#endif

static void mdlInitializeSampleTimes(SimStruct *S)
{
	#ifdef DEBUG_MODE
	printf("[From udpreceive] Starting mdlInizializeSampleTimes\n");
	#endif

	ssSetSampleTime(S, 0, SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0.0);

	#ifdef DEBUG_MODE
	printf("[From udpreceive] Ending mdlInizializeSampleTimes\n");
	#endif
	
} /* end mdlInitializeSampleTimes */


#define MDL_START
#if defined(MDL_START)
static void mdlStart(SimStruct *S) 
{
    
	#define MAX_DIM_BUFFER 17

    char	local_ip[MAX_DIM_BUFFER];
    int     i = 0;
    int     sockfd = 0;

    
#ifdef WINDOWS_TARGET		// variables inside Windows
	
	struct sockaddr_in 		input_sock;
	struct hostent  		*host;
	unsigned long			hostaddr;

	WORD wVersionRequested = MAKEWORD(1, 1); 
	WSADATA wsaData; 
	int err = WSAStartup(wVersionRequested, &wsaData); 

#else						// variables inside RTAI

    int     ret = 0;
    void	**PWork = ssGetPWork(S);
    real_T	*bufferR;
    real_T	*bufferO;
    
	#ifndef MATLAB_MEX_FILE
    struct sockaddr_in local;
	#endif

#endif						// variables	    

    
	#ifdef DEBUG_MODE
	printf("[From udpreceive] Starting mdlStart\n");
	#endif     
    
#ifdef WINDOWS_TARGET		// mdlStart Windows code

	if (err != 0) return; 
	ssSetIWorkValue(S, 0, 0);
	
	//gethostname((char FAR *)local_ip, MAX_DIM_BUFFER);
	mxGetString(LOCAL_IP_PARAM, local_ip, MAX_DIM_BUFFER);

	if ((host = gethostbyname(local_ip)) == 0)
	{
		//gethostbyname has failed, try by addr
		if((hostaddr = inet_addr(local_ip)) == INADDR_NONE) 
		{
			//inet_addr has failed as well
			printf("udpreceive - Unable to resolve local name. Error :%d\n",WSAGetLastError());

			ssSetErrorStatus(S,"udpreceive - Unable to resolve local name");		
			ssSetStopRequested(S, 1);	// stop simulation
			return;
		}
		if ((host = gethostbyaddr((const char *)hostaddr, sizeof(hostaddr), AF_INET)) == 0)
		{
			//all tries have failed
			printf("udpreceive - Unable to resolve local name. Error :%d\n",WSAGetLastError());
		
			ssSetErrorStatus(S,"udpreceive - Unable to resolve local name");		
			ssSetStopRequested(S, 1);	// stop simulation
			return;		
		}
	}

	// prepare socket	
	memcpy((char *)&input_sock.sin_addr, host->h_addr, host->h_length);
	input_sock.sin_family = AF_INET;
	for (i = 0; i < 8; i++) input_sock.sin_zero[i] = 0;
	input_sock.sin_port = htons(LOCAL_PORT);
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("udpreceive - Socket creation error :%d\n",WSAGetLastError());

		ssSetErrorStatus(S,"udpreceive - Socket creation error");		
		ssSetStopRequested(S, 1);	// stop simulation
		return;
	}
	
	// non blocking mode
	i = 1;
	ioctlsocket(sockfd, FIONBIO, &i);

	i = OUTPUT_SIZE;
	setsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (const char *)&i, sizeof(real_T));
	
	i = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char FAR*)&i, sizeof(real_T));
	
	if (bind(sockfd, (struct sockaddr FAR *)&input_sock, sizeof(struct sockaddr)) < 0)
	{
		printf("udpreceive - Socket receive error :%d\n",WSAGetLastError());

		ssSetErrorStatus(S,"udpreceive - Socket receive error");		
		ssSetStopRequested(S, 1);	// stop simulation
		return;
	}

	ssSetIWorkValue(S, SOCK_ID, sockfd);

#else			// mdlStart RTAI code

    mxGetString(LOCAL_IP_PARAM, local_ip, MAX_DIM_BUFFER);
    
    //DEBUG INFOs......
    printf ("\nLocal IP    : %s\n ",local_ip);
    printf ("\nLocal Port  : %d\n ",LOCAL_PORT);
    //.......END DEBUG


#ifndef MATLAB_MEX_FILE

    //  Allocation of bufferR and bufferO
	bufferR = (real_T*) calloc(OUTPUT_SIZE, sizeof(real_T));
	bufferO = (real_T*) calloc(OUTPUT_SIZE, sizeof(real_T));
    
    memset (&local, 0, sizeof(struct sockaddr_in) );

    //  setup SOCKADDR STRUCT parameters
	local.sin_family = AF_INET;
	local.sin_port = htons(LOCAL_PORT);
	inet_aton(local_ip, &local.sin_addr);

    // create the UDP socket
    sockfd = rt_socket(AF_INET,SOCK_DGRAM, 0);
    
    // set as non-blocking socket
    fcntl(sockfd, F_SETFL,O_NONBLOCK);

    // bind the socket
	if ( rt_socket_bind(sockfd, (struct sockaddr*) &local, sizeof(local)) != 0 )
	{
		printf ("Error during socket binding: return value %d\n", ret);
		return;
	}
	
    // set initial conditions: reset the buffer...

    for (i = 0; i < OUTPUT_SIZE; i++)
	{
        bufferR[i] = 0;
        bufferO[i] = 0;
    }
  
	#endif

    // store socket descriptor 
    ssSetIWorkValue(S, SOCK_ID, sockfd );              
     
    ssSetPWorkValue(S, 0, bufferR);
    ssSetPWorkValue(S, 1, bufferO);

#endif			// mdlStart code
    
    
	#ifdef DEBUG_MODE
	printf("[From udpreceive] Ending mdlStart\n");
	#endif

}
#endif


static void mdlOutputs(SimStruct *S, int_T tid)
{
    real_T  *y			= ssGetOutputPortRealSignal(S, OUTPUT_SIGNAL); 
    real_T	*flag_sync	= ssGetOutputPortRealSignal(S, SYNCHRO);
    int		sockfd		= ssGetIWorkValue(S, SOCK_ID);
	int		ret			= 0;
	int		i;

#ifdef WINDOWS_TARGET		// variables inside Windows

	fd_set 	set;
    
#else						// variables inside RTAI
	
	// various declaration
    int		flush = 0;
         
    void	**PWork 	= ssGetPWork(S);
    real_T 	*bufferR 	= (real_T*) PWork[0];
    real_T 	*bufferO 	= (real_T*) PWork[1];
    real_T 	*temp; 

	#ifndef MATLAB_MEX_FILE
        
    // packet used during communication
    struct PACKET_STR pack_rx;

	#endif        
    

#endif						// variables    

#ifdef WINDOWS_TARGET		// mdlOutputs Windows code    

	
	*flag_sync = 0;			// synchro flag at 0

	FD_ZERO(&set);
	FD_SET(sockfd, &set);

	ret = recv(sockfd, (const char FAR *)y, sizeof(real_T) * OUTPUT_SIZE, 0);
	
	if (ret != SOCKET_ERROR) 	
		*flag_sync = 1;

#else				// mdlOutputs RTAI code

    #ifndef MATLAB_MEX_FILE	
 
    *flag_sync = 0;
    memset(&pack_rx, 0, sizeof(struct PACKET_STR));
    
    ret = rt_socket_recv(sockfd, bufferR, sizeof(real_T)*OUTPUT_SIZE, 0 );
        
    if (ret > 0) 
    {
    
        while (ret > 0) 
        {
        	// flushing  socket's buffer
    	    ret = rt_socket_recv(sockfd, bufferR, sizeof(real_T) * OUTPUT_SIZE, 0 ); 	   	   
    	    flush = flush + 1; 	    
		} 
	
	    #ifdef _PRINT_F_
	        printf("P.Flushed:  %d  Ret:  %d\n",(flush),ret);
	    #endif
              
        // store the rcv data in the buffer...
        temp = bufferR;
        bufferR = bufferO;
        bufferO = temp;
        ssSetPWorkValue(S, 0, bufferR);
        ssSetPWorkValue(S, 1, bufferO);
        
        //..update the flag...
        *flag_sync = 1;
          
    }
    else 
    {
    	// clear data
    	memset (&pack_rx, 0, sizeof(struct PACKET_STR) );        
    }
   

	// output the signal getting data from the buffer
    for (i = 0; i < OUTPUT_SIZE; i++)	
		y[i] = (bufferO[i]);

#endif

#endif			// mdlOutputs code

} /* end mdlOutputs */


static void mdlTerminate(SimStruct *S)
{

#ifdef WINDOWS_TARGET		// variables inside Windows

	int sockfd = ssGetIWorkValue(S, 0);

#else						// variables inside RTAI	

    // load the socket descriptor
	int sockfd		= ssGetIWorkValue(S, 0);
	void **PWork	= ssGetPWork(S);

#endif						// variables

	#ifdef DEBUG_MODE
	printf("[From udpreceive] Starting mdlTerminate\n");
	#endif

#ifdef WINDOWS_TARGET		// mdlTerminate Windows code

	//Close Socket	
	shutdown(sockfd, 0x01);
	closesocket(sockfd);
	WSACleanup();

#else

#ifndef MATLAB_MEX_FILE
	if ( rt_socket_close(sockfd) < 0 )
	    printf ("Error closing the socket!\n");
#endif
        
        free(PWork[0]);
        free(PWork[1]);

        ssSetPWorkValue(S, 0, NULL);
        ssSetPWorkValue(S, 1, NULL);

#endif				// mdlTerminate code

	#ifdef DEBUG_MODE
	printf("[From udpreceive] Ending mdlTerminate\n");
	#endif


} /* end mdlTerminate */


#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif