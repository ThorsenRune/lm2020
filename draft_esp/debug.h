
#ifndef __DEBUG__    // only once guard
#define __DEBUG__


bool mChangeDebugLevel();			//Change the debugging level by insering a number on the terminal send
extern int nDbgLvl;
//Macro to print if debuglevel has the given bit set
#define DEBUG(bit,...) {if (nDbgLvl&(1<<(bit))) { Serial.printf(__VA_ARGS__); } }




//End of guard
#endif   //__DEBUG__
