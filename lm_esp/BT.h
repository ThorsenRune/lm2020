//file: BT.h
/*
		Handling wireless transmissions
			 bluetooth
			 issue: FC_RT210128
			 Libraries should be included by default in Arduino !????
*/
/*
#ifndef ____BT____    // only once guard to avoid recursive inclusion
#define ____BT____
#include <Arduino.h>
#include "debug.h"   //Enables debugging with   DUMP(someValue);  TRACE();

#include <BLEDevice.h>
#include "BLEUtils.h"
#include "BLEServer.h" //Library to use BLE as server
#include "BLE2902.h"

void mTransmit();
void mReceive2(uint8_t *data, size_t len);
void onConnect(BLEServer* pServer);
void onDisconnect(BLEServer* pServer);
void onWrite(BLECharacteristic *pCharacteristic);
bool InitBLE();



#endif  //____BT____
*/
