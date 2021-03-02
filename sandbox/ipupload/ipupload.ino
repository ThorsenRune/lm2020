#include <WiFi.h>
#include <HTTPClient.h>
#include "debug.h"   //Enables debugging with   DUMP(someValue);  TRACE();
#include "ArduinoTrace.h"  //	DUMP(nDbgLvl);TRACE();

//From https://stackoverflow.com/questions/58844448/send-data-from-esp32-to-a-server-via-wifi
/*
  Basic startingpoint for sending the IP address of the ESP to a server where it can be stored in a file
  and then retrieved for the browser client to connect via websocket to ESP

*/

String host = "http://thorsen.it/public/lm2020/sandbox/ipupload/";
String file_to_access = "test_post.php";
String URL = host + file_to_access;
String IpAddress2String(const IPAddress& MyStaticIP)
{
  return String(MyStaticIP[0]) + String(".") +\
  String(MyStaticIP[1]) + String(".") +\
  String(MyStaticIP[2]) + String(".") +\
  String(MyStaticIP[3])  ;
}

void setup(){
    Serial.begin(115200);
  Serial.printf("Starting %s\n",URL.c_str() );
  delay(1000);
   WiFi.begin("LABIONWIFI", "L@bion2015"); //Connect to WiFi
   HTTPClient http;
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }


   bool http_begin = http.begin(URL);
   String message_name = "message_sent";

   delay(1000);
   String MyIpIs=IpAddress2String(WiFi.localIP()).c_str();
   Serial.printf("WiFi.localIP() %s\n" ,MyIpIs.c_str() );

   String message_value = "Sending IP:"+MyIpIs;
   String payload_request = message_name + "=" + message_value;  //Combine the name and value
    Serial.printf("Sending %s\n",payload_request.c_str() );
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");
   int httpResponseCode = http.sendRequest("POST", payload_request);
   String payload_response = http.getString();
   Serial.printf("Received %s\n",payload_response.c_str() );
}
void loop()
{
 delay(5000);
  }
