//file: transmit.h
/*
		Handling wireless transmissions
			websocket
			todo: bluetooth

*/

#ifndef ____TRANSMIT____    // only once guard to avoid recursive inclusion
#define ____TRANSMIT____
#include "ArduinoTrace.h"  //	DUMP(nDbgLvl);TRACE();
#include "debug.h"				//	DEBUG(1,"something %i",nDbgLvl);
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "getWiFiCreds.h"
#include "publishvars.h"
bool mStartWebSocket2();
bool mWaitForWSClient(int TimeOutClient);
void mTransmit();
void mReceive2(uint8_t *data, size_t len);
bool mWifiSetupMain();
void _WsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
bool mStartWebSocket3();

#endif  //____TRANSMIT____
