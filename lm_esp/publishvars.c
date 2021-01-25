//file: publishvars.c
//gdoc: https://docs.google.com/document/d/1oPU1YRp7GJ42Dva90mEjCJe7_z8a_jbUWS-Unu9kEh8/edit
//rev: rt210108   -   testing and documentation


/*
	Descrizione: Publish variables to the protocol
  make sure the variable is global i.e. declare it "extern int nTestVar[3];" in a *.h file. for example in publishvars.h
  then put it in the Expose2Protocol as "EXPOSEARRAY(&oTXProt,nTestVar);"
*/

#include "inoProtocol.h"
#include "publishvars.h"


/*********GLOBAL VARIABLES**************/
ModeType nMode;
int nTimerInMs[3]={};							// Milliseconds 1= T0 start of cycle 2=actual ms, 3How many clockcycles are available as resource
int nTestVar[3]={901,902,903};							// a test variable
int Art_signal[120]={3};                      //Artificial signal
int Gain[2]={100,10};								// Stimulation level gain
int nDbgLvl=1;   //Verbosity level for debuggin messages

/***********    INSERT THOSE YOU WANT TO PUBLISH BELOW *************/
void Expose2Protocol(){  //todo5:refactor name to 'PublishProtocol'
  // Define and transmit vars to HOST
  //mDebugMsg("Called Expose2Protocol");    //
  mProtocol_Init (&oTXProt);			       //Initialize protocol object
/*============================	INSERT VARIABLES THAT YOU WANT TO EXPOSE BELOW ====================	*/
  EXPOSEARRAY(&oTXProt,nTestVar);
  EXPOSEARRAY(&oTXProt,nTimerInMs);
  EXPOSEARRAY(&oTXProt,nMode.all_flags);
  EXPOSEARRAY(&oTXProt,bErrFlags.all_flags);
  EXPOSEARRAY(&oTXProt,Art_signal);
  EXPOSEARRAY(&oTXProt,Gain);
}
