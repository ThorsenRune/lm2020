//file: BT.h
/*
		Handling wireless transmissions
			 bluetooth

*/

#ifndef ____BT____    // only once guard to avoid recursive inclusion
#define ____BT____
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h> //Library to use BLE as server
#include <BLE2902.h> 

void onBTConnect(BLEServer* pServer);
void onBTDisconnect(BLEServer* pServer);
void onBTWrite(BLECharacteristic *pCharacteristic);
void InitBLE();

#endif  //____BT____