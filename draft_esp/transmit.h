//file: transmit.h
/*
		Handling wireless transmissions
			websocket
			todo: bluetooth

*/

#ifndef ____TRANSMIT____    // only once guard to avoid recursive inclusion
#define ____TRANSMIT____
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "getWiFiCreds.h"
bool mStartWebSocket2();
bool mWaitForWSClient2(int TimeOutClient);
void mTransmit();
void mReceive(uint8_t *data, size_t len);




#endif  //____TRANSMIT____
