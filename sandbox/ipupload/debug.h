
#ifndef __DEBUG__    // only once guard
#define __DEBUG__
/*
		Two macros useful for debugging
		DEBUG(bit,"pringf string");			Will echo to the serial viewport if nDbgLvl has the bit set
		BRK(bit,"pringf string");			Will stop and wait for user to press return on the serial monitor
*/
bool mChangeDebugLevel();			//Change the debugging level by insering a number on the terminal send
extern int nDbgLvl;
//Macro to print if debuglevel has the given bit set
#define DEBUG(bit,...) {if (nDbgLvl&(1<<(bit))) { Serial.printf(__VA_ARGS__); } }
#define BRK(bit,...) {if (nDbgLvl&(1<<(bit))) { Serial.printf(__VA_ARGS__);Serial.println(" ---- ");BREAK(); } }
//End of guard
#endif   //__DEBUG__
