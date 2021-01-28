// File: system.c
//	gdoc:https://docs.google.com/document/d/1LTyk1aeT9Fkd8Zu6w4zBtRTay2VnPOga3YDqHwz6GeA/edit
//rev: rt210108
/*
	Description:HAL (Hardware Abstraction Layer)	LM hardware system specific low level methods
  Watchdog: for powering down when no communication has been done for a while
  System setup
*/

#include "system.h"
#include "publishvars.h"

/*				SYSTEM CONTROL 				*/
int nShutDownTimer=0;				//Timer for shutting down
const int kPowerWatchDogTimeOut=100000;//todo9:define value
const kCommunicationTimeOut=100000;		//todo9:define value
int nPowerWatchDog=2000;
EX_FLAGS bErrFlags;									// Allocation for the Flags

void mPowerWatchDogReset(void){			//Reset the power off (idle) watchdog
//  todo9: this resets a timeout when communication is active
/*    When the nPowerWatchDog we can put RF communication
			and maybe other circuits   to sleep*/
	nPowerWatchDog=kPowerWatchDogTimeOut+kCommunicationTimeOut;
}

void MainSetup(){
  nMode.all_flags[0]= 0U;          // Clear all mode flags
  bErrFlags.all_flags[0] = 0U;			// Clear all errpr flags
  nMode.bits.DEBUGGING=1;
  if (nMode.bits.DEBUGGING) mDebugMsg("MainSetup1");
  mCommInitialize();//Initialize the protocol communication with HOST
  if (nMode.bits.DEBUGGING) mDebugMsg("Done mCommInitialize");
//+todo8	mStartTimer0(t0count);														// Start timer0
  mPowerWatchDogReset();  //Reset the watchdog
}
