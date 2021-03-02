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
#include "getWiFiCreds.h"
#include <HTTPClient.h>
#include <Arduino.h>
#include <stdint.h>           //Define standard types uint32_t etc
#include <stdbool.h>				//Boolan types rt210107
/*  TEsting forward backward declarations       */
int testA(int i){
  return 0;//  return testB(i);
}
void testB(){
  int i;
    i=i+testA(i);
}
void mTesting1(){
  //Write the actual IP to the server which can be fetched by
  //Todo make this relative to the directory
  String URL = "http://thorsen.it/public/lm2020/draft_webapp/getsetip.php";
  DEBUG(1,"\n------------------ SAVING IP ON SERVER------------------");
  DEBUG(1,"\nCalling  %s\n",URL.c_str() );
  WiFi.begin("LABIONWIFI", "L@bion2015"); //Connect to WiFi
  HTTPClient http;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
 }
  String MyIpIs=IpAddress2String(getIP());
  DEBUG(1,"WiFi.localIP() %s\n" ,MyIpIs.c_str() );
  bool http_begin = http.begin(URL);
  String message_name = "message_sent";
  String message_value =  MyIpIs;
  String payload_request = message_name + "=" + message_value;  //Combine the name and value
   Serial.printf("Sending %s\n",payload_request.c_str() );
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.sendRequest("POST", payload_request);
  String payload_response = http.getString();
  Serial.printf("Received %s\n",payload_response.c_str() );
}
void mTesting2(){
  nTestVar[1]=nTestVar[0];
  nTestVar[0]= millis();  //mS
  nTestVar[2]=nTestVar[0]-nTestVar[1];
}
#define ARRAYLENGTH(x)     (sizeof(x)/sizeof(x[0]))


void mGenerateSignal(){
   static int phaseshift=1;   //Example of static variable. is only initialized once
  phaseshift=phaseshift+1;;
  nTestVar[4]=phaseshift;
  DEBUG(5,"phaseshift  %i\r\n", phaseshift);
  int len=sizeof(Art_signal)/sizeof(Art_signal[0]);
  for (int i=0;i<len ;i++){    //try until timeout
    float arg=((float)i+phaseshift)/5;
    Art_signal[i]=(int)(sin(arg)*(float)Gain[0]);
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
  //  sleep(1);
	}
	nTimerInMs[1]=nTimerInMs[0];							//Start of cycle time nTimerInMs[1] is the current time in mS
}
