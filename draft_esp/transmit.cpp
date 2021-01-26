/*
		Handling wireless transmissions
			websocket
			todo: bluetooth

*/
#include "transmit.h"


bool mStartWebSocket2(){
  IPAddress staticIP=getIP();
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(staticIP,staticIP,subnet);  // if using static IP, enter parameters at the top
  Serial.println(("Starting   WiFi|"+getAP_SSID() +"| , |"+getAP_PASS()+"|"+IpAddress2String(getIP())+"|").c_str());
  WiFi.begin(getAP_SSID().c_str(),getAP_PASS().c_str());
  for (int i=0;i<TimeoutWifi;i++){ //Loop until timeout
    if (WiFi.status() == WL_CONNECTED) return true;   //Happily connected to wifi
    delay(1000); //Sleep to let connect
    Serial.print("."); //Make some waiting dots
  }
  mDebugMsgcpp("WiFi is not available, check credentials");
  WiFi.disconnect();
  return false;   //WiFi not available maybe creaden
}
