// A MACRO THAT DEFINE EXPOSE TO PROTOCOL for vectors    //ID1215A
//#define EXPOSEARRAY(obj,arg1) mTXExposeArray(obj,#arg1,arg1,sizeof(arg1)/sizeof(arg1[0]),kSend32Bit);
/* Expands to something like
  mTXExposeArray(&oTXProt,"xData",       data name
    xData,                            //Var pointer
    sizeof(xData)/sizeof(xData[0]),   //length kADCBuffsize
    kSend32Bit);
*/
#include "inoProtocol.h"
#include "publishvars.h"
/*  TEsting forward backward declarations       */
int testA(int i){
    return testB(i);
}
void testB(){
  int i;
    i=i+testA(i);
}
void mTesting(){
  mDebugMsg("mTesting");
//  mPushRX2FIFO(kHandshake);		//Simulate handshake reception
//  mPushRX2FIFO(kCommInit);		//Simulate kCommInit reception
  //Todo. why do you need to double send?



}
void mTesting2(){
   mGenerateSignal();
}
#define ARRAYLENGTH(x)     (sizeof(x)/sizeof(x[0]))

void mGenerateSignal(){
  int len=sizeof(Art_signal)/sizeof(Art_signal[0]);
  for (int i=0;i<len ;i++){    //try until timeout
    Art_signal[i]=(int)(sin(i/30)*(float)Gain[0]);
  }
}

void mWaitCycleStart(void){						 	// Wait until 60ms has passed since last call
  int kCycleTime=60000;    //Cycle time in us
  nTimerInMs[0]=micros();  //   microseconds
  nTimerInMs[2]=kCycleTime-(nTimerInMs[0]-nTimerInMs[1]);	//the spare CPU time, time spend in loop waiting
	for (int i=0;i<(100*kCycleTime);i++){
    nTimerInMs[0]=micros();  //   microseconds
    if ((kCycleTime<(nTimerInMs[0]-nTimerInMs[1])) ) break; //kCycleTime has elapsed
    if ((nTimerInMs[0]-nTimerInMs[1])<0) break;   //Brutal overflow check
    sleep(1);
	}
	nTimerInMs[1]=nTimerInMs[0];							//Start of cycle time nTimerInMs[1] is the current time in mS
}
