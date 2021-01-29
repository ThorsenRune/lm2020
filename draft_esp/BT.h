//file: BT.h
/*
		Handling wireless transmissions
			 bluetooth
			 issue: FC_RT210128
			 Libraries should be included by default in Arduino !????
*/

#ifndef ____BT____    // only once guard to avoid recursive inclusion
#define ____BT____
#include <Arduino.h>
//#define DEBUG_ON 1
#include <BLEDevice.h>

#include "BLEUtils.h"
#include "BLEServer.h" //Library to use BLE as server
#include "BLE2902.h"

void onBTConnect(BLEServer* pServer);
void onBTDisconnect(BLEServer* pServer);
void onBTWrite(BLECharacteristic *pCharacteristic);
void InitBLE();

 

#endif  //____BT____
