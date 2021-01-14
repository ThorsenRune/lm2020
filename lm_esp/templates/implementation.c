//file: filename.c
//gdoc: url


/*
	Descrizione: vedi Documentation of Communication protocol LM
*/

#include "filename.h"


/*			PATTERNS			*/

/*	wait loop pattern
//Wait here until user has submitted the response in startapp (startAPP==true)
bool mWaitUntilTrueOrTimeout(bool &bFlag){
  for (int i=0;i<100;i++){    //try until timeout
    if (bFlag) return true;
    delay(1000);
  }
  mDebugMsg("Timeout mWaitUntilTrueOrTimeout");
  return false;
}
		*/
return mWaitUntilTrueOrTimeout(startAPP);
