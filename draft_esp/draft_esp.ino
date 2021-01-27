/*  Unit testing for lm_esp
    This is our playground where we test new functionality

*/

//#include lm_esp.ino
/*
    COMMUNICATION
      mReceive: receive data from client and send to internal protocol or passes to subsystem (LM)
      mTransmit: inverse of mReceive, sends to client
      use : publishvars.c to expose variables to the client
*/
//    SETTINGS
int TimeoutWifi   =20;  //Seconds of timeout to get wifi connection
int TimeOutClient =120;//Seconds of timeout mWaitForWSClient, time for user to connect
bool bRelayLM2018 = false;    //Apply protocol to arduino FW or relay to LM_FW
#define RXD2 16       //Define the pins for  U2UXD
#define TXD2 17


//Libraries
#include <stdint.h>           //Define standard types uint32_t etc
#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "debug.h"   //Enables debugging with   DUMP(someValue);  TRACE();
#include "ArduinoTrace.h"  //	DUMP(nDbgLvl);TRACE();
//    LM setup
#include "getWiFiCreds.h" //establish connection to the  wifi accesspoint (WAP or internet WiFi router)
extern "C" {  //Note- neccessary to implement C files
  #include "system.h"
  #include "inoProtocol.h"      //Including h file allows you to access the functions
  #include "publishvars.h"
}
#include "transmit.h";          //Where websocket and Bluetooth calls reside
void(* ReBoot)(void) = 0;   //Reboot the chip using ReBoot();

/*******   The two STANDARD ARDUINO functions (setup and loop)   **********/
/*  ----------   ARDUINO ENTRY POINT  -------------mProtocolProcess--------*/

void setup(){
  mESPSetup();
  DEBUG(1,"-------------running Android setup ------------");
  MainSetup();		//Setup the system, protocol & pointers.. (rt210107)
  int ret=mWifiSetupMain();
  if (!ret) {DEBUG(1,"REBOOT");delay(2000);ReBoot();}
  //Websocket running okmProtocolProcess
  mDebugMsg("calling main loop in LM_ESP.ino");
  mTesting1();
}// This returns to an intrinsic call to loop()

/*******************ARDUINO MAIN WHILE LOOP *******************************/
//      AUTOMATICALLTY CALLED BT ARDUINO IDE
void loop(){		//The main loop of the
    mWaitCycleStart();							// 1.  Wait for a block start using system clocks
   if (nDbgLvl>0) mGenerateSignal();
/*************      BUSINESS LOGIC FOR THE SIGNAL PROCESSING AND COMMUNICATION  *****/
//  mSignalProcessing();					//3. Signal Processing
//  mSystemActions();						//4.System management
    mTransmit();                //send data to client. Corresponding  mReceive is interrupt handled
    mTesting2();
    mChangeDebugLevel();
}
/******************************* END MAIN WHILE(1) *******************************/
void serialEvent2() {  //automatic event from LM serial connection
  //Figure another way to relay to client
/*  while (Serial2.available()&&(SendDataBuf<MAXL)) {
    int sendbyte=Serial2.read();
    mSendData[SendDataBuf]=sendbyte;    //Get a byte from serial port
    SendDataBuf++;
  }
  */
}

void serialEvent(){
  mChangeDebugLevel();
}
String printBinary(int inByte)     //Convert byte to bitstring
{
  String s="B";
  for (int b = 15; b >= 0; b--)
  {
    s=s+bitRead(inByte, b);
  }
  return s;
}
bool mChangeDebugLevel(){ //Takes a number from the arduino console and set the nDbgLvl accordingly
  if (Serial.available()>0){
    int debuglevel = Serial.parseInt();
    if (debuglevel>0){
      Serial.printf("\nChanged debugging bitpattern to : %s\n",printBinary(debuglevel).c_str());
    //  printBinary(debuglevel);
      nDbgLvl=debuglevel;
      return true;
    }
  }
  return false;
}


void mESPSetup(){
  Serial.begin(115200);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);  //Connection to LM subsystem
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     mDebugHalt("Fatal problem in spiffs");
  }
}
//  Debugging  message and continue
void mDebugMsg(char msg[]){
      Serial.print("Debugger says: \t ");
      Serial.printf("%s\n",msg );

  }
void mDebugInt(char msg[],int data){
    Serial.printf("%s  \t:\t %i\t %c\n",msg,data,(char)data);
  }
//  Stop and do endless loop
void mDebugHalt(char msg[]){
  while (1){
    Serial.print("\nHALT Command (stopped with endless loop) ");
    Serial.printf("\n%s",msg );
    delay(300000);
  }
}
void  mDebugMsgcpp(char msg[]){
      Serial.print("Debugger says: \t ");
      Serial.printf("%s\n",msg );
    //  delay(100);
  }
void mDebugHaltcpp(char msg[]){
      Serial.print("\n------------HALT  ------\n");
      Serial.println(msg );
    while (1){
      delay(300000);
    }
  }
void mPrint(String msg){
      //delay(100);
      Serial.printf("%s\n",msg.c_str() );
      //delay(100);
  }
