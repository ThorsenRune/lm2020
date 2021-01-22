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
  mDebugMsg("mTesting2");
  mPushRX2FIFO(kGetReq);		//Simulate kCommInit reception
  mPushRX2FIFO(64 );		//first variable
}
#define ARRAYLENGTH(x)     (sizeof(x)/sizeof(x[0]))
void mGenerateSignal(){
  int len=sizeof(Art_signal)/sizeof(Art_signal[0]);
  int amplitude=1234;
  for (int i=0;i<len ;i++){    //try until timeout
    //Art_signal[i]=amplitude*sin(i/30);
    Art_signal[i]=i;
  }

}

void mWaitCycleStart(void){						 	// Wait using the system clock
  /* To implement
	int i;
	nTimerInMs[2]=60-(nTimerInMs[0]-nTimerInMs[1]);	//Time since epoch start = the spare CPU time
	i=SysTimer();
	while ((LastSysTick-kMainLoopIntervalInSystemTicks)<i) {
		if (LastSysTick<i){
			LastSysTick=LastSysTick+0x00FFFFFF;					// i roll over, add 24 bit to LastSysTick
		}
		i=SysTimer();
	}
	LastSysTick=i;
	nTimerInMs[1]=nTimerInMs[0];							//Start of cycle time nTimerInMs[1] is the current time in mS
  */
}
