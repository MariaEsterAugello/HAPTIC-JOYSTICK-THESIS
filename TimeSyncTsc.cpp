#include <windows.h>

#define S_FUNCTION_NAME  TimeSyncTsc
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#define SAMPLE_TIME_IDX 0
#define SAMPLE_TIME_PARAM(S) ssGetSFcnParam(S,SAMPLE_TIME_IDX)

#define ACCELERATOR_IDX  1
#define ACCELERATOR_PARAM(S) ssGetSFcnParam(S,ACCELERATOR_IDX)

#define NPARAMS   2

//define DELTAOUTPUT to get idle time in output
#define DELTAOUTPUT

LARGE_INTEGER base_time;
double frequency;
//FIX to compile on x64 architecture (no naked declaration and no asm keyword)
//    https://docs.microsoft.com/en-us/cpp/cpp/naked-cpp?view=msvc-170
//       and
//    https://docs.microsoft.com/en-us/cpp/intrinsics/rdtsc?view=msvc-170
/*__declspec(naked) __int64 getTimeStamp(void) {
  __asm	{
    rdtsc       
    ret
  } 
} */

#pragma intrinsic(__rdtsc)
//use of function:
// unsigned __int64 i;
// i = __rdtsc();
__int64 getTimeStamp(void) {
return __rdtsc();
}





#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
static void mdlCheckParameters(SimStruct *S)
{
	if (!mxIsDouble(SAMPLE_TIME_PARAM(S)) || mxGetNumberOfElements(SAMPLE_TIME_PARAM(S)) != 1) {
		ssSetErrorStatus(S,"TIMESINC - Sample Time must be a double precision number");		return;
	}
	if (mxGetScalar(SAMPLE_TIME_PARAM(S))<0) {
		ssSetErrorStatus(S,"TIMESINC - Sample Time must be > 0");
		return;
	}
	if (!mxIsDouble(ACCELERATOR_PARAM(S))|| mxGetNumberOfElements(SAMPLE_TIME_PARAM(S)) != 1) {
		ssSetErrorStatus(S, "TIMESINC - Multiply Factor must be a double precision number");
		return;
	}
	if (mxGetScalar(ACCELERATOR_PARAM(S))<0) {
		ssSetErrorStatus(S, "TIMESINC - Multiply Factor must be > 0");
		return;
	}
}
#endif /* MDL_CHECK_PARAMETERS */


static void mdlInitializeSizes(SimStruct *S)
{
	ssSetNumSFcnParams(S, NPARAMS);  /* Number of expected parameters */
#if defined(MATLAB_MEX_FILE)
	if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
		mdlCheckParameters(S);
		if (ssGetErrorStatus(S) != NULL) {
			return;
		}
	} else {
		return; 
	}
#endif
	ssSetNumContStates(S, 0);
	ssSetNumDiscStates(S, 0);
	if (!ssSetNumInputPorts(S, 0)) return;   
#ifdef DELTAOUTPUT
	if (!ssSetNumOutputPorts(S, 1)) return;  
	ssSetOutputPortWidth(S, 0, 2);
#else
	if (!ssSetNumInputPorts(S, 0)) return;   
#endif
	ssSetNumSampleTimes(S, 1);
	ssSetNumRWork(S, 0);
	ssSetNumIWork(S, 1);
	ssSetNumPWork(S, 0); 
	ssSetNumModes(S, 0); 
	ssSetNumNonsampledZCs(S, 0);
	ssSetOptions(S, 0);
}
	
static void mdlInitializeSampleTimes(SimStruct *S)
{   
	ssSetSampleTime(S, 0, mxGetScalar(SAMPLE_TIME_PARAM(S)));
	ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_START 
#if defined(MDL_START) 
static void mdlStart(SimStruct *S)
{   
    LARGE_INTEGER frequencyLI;


    ssSetIWorkValue(S, 0, 1);
    QueryPerformanceFrequency(&frequencyLI);
    frequency = (double)frequencyLI.QuadPart;
    
    printf("\nReal-Time Simulation Started....\n");
    printf("High Performance Counter Resolution: %f usec\n",1000000.0/frequency);
    
    SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
    //SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}     
#endif

static void mdlOutputs(SimStruct *S, int_T tid)
{   
#ifdef DELTAOUTPUT
	real_T  *y = ssGetOutputPortRealSignal(S,0); // the pointers vector and use
#endif
	double 	sim_timer;
	double  real_timer;
	double 	delta = 0.0;
	double  mult;
	int     phase;
	LARGE_INTEGER   now;
    __int64         now64;
    

	now64 = getTimeStamp();
    mult = mxGetScalar(ACCELERATOR_PARAM(S));
	phase = ssGetIWorkValue(S, 0);
    
    if (phase==1) {
		//firts time called
		ssSetIWorkValue(S, 0, 2);
	} else {
		if (phase==2) {
			//second time called
            QueryPerformanceCounter(&base_time);
			ssSetIWorkValue(S, 0, 3);
		} else {
			//phase==3 : RT sync
			sim_timer=ssGetT(S);
			QueryPerformanceCounter(&now);
			real_timer=( ((double)(now.QuadPart - base_time.QuadPart)) / frequency ) * mult;
			delta=sim_timer-real_timer;
			while(real_timer<sim_timer) {
				//Sleep((int)((sim_timer-real_timer)*1000.0));
                QueryPerformanceCounter(&now);
				real_timer=( ((double)(now.QuadPart - base_time.QuadPart)) / frequency ) * mult;
			}
		}
	}
	//set output;
#ifdef DELTAOUTPUT
	y[0]=delta;
	y[1]=(double)now64;
#endif	       
}                                                

static void mdlTerminate(SimStruct *S)
{
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    printf("\nReal-Time Simulation Terminated!\n");
}                                                                // function

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
