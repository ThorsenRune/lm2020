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

EX_FLAGS bErrFlags;									// Allocation for the Flags

void mPowerWatchDogReset(void){			//Reset the power off (idle) watchdog
  mDebugHalt("Implement watchdog");
//  todo3: enable following line
//	nPowerWatchDog=kPowerWatchDogTimeOut+kCommunicationTimeOut;
}

void MainSetup(){
  nMode.all_flags[0]= 0U;          // Clear all mode flags
  bErrFlags.all_flags[0] = 0U;			// Clear all errpr flags
  nMode.bits.DEBUGGING=1;
  if (nMode.bits.DEBUGGING) mDebugMsg("MainSetup1");
  mCommInitialize();//Initialize the protocol communication with HOST
  if (nMode.bits.DEBUGGING) mDebugMsg("Done mCommInitialize");
//+todo8	mStartTimer0(t0count);														// Start timer0
//+todo8: mPowerWatchDogReset();  //Reset the watchdog
}
