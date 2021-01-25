//file: publishvars.h
//gdoc:  url
//same as mdataexch.c in LM firmware
#ifndef __GLOBALDATA__    // only once guard
#define __GLOBALDATA__
#ifdef __cplusplus    //important- Tell the compiler that we have c code
extern "C" {
#endif
extern int nTimerInMs[3];     //GLOBAL VARIABLES for the Protocol
extern int nTestVar[];
extern int Art_signal[120];
extern int Gain[2];
extern int nDbgLvl;

/*        GLOBAL METHODS        */
void mGenerateSignal();
void mWaitCycleStart(void);

typedef union {			//Flags for the operation modes
int     all_flags[1];      				/* Allows us to refer to the flags 'en masse' */
struct
{
 uint32_t
   ENPLW					: 1,							/*Bit 0: Enable piecewise linear function */
   ENCHRGVCCS		: 1,     					/*Bit 1: Enable CHRGVCCS, Enable stimulation */
   ENSWITCHES		: 1,							/* Enable MUX Switches */
   HVPS_ON 				: 1,							/*3 Enable HVPS charging VPP*/
   DEBUGGING			: 1,							/*4: Debug									 */
   SINEGENERATOR	: 1,							/*5:	Output a sine, set to null by  STIMENABLE											*/
   spare6				: 1,							/* Enable PLS CTRL on channel 2 */
   spare7				: 1,							/*  */
   spare8 		: 1,							/* Increase or decrease aIAmp depending ALSO on RMSnew-RMSold */
   spare9		: 1,							/* Blanking enable (first 20 samples) */
   SR_FILTER			: 1,							/*Bit 10: Enable Slewrate filtering rather than IIR filter of RMS */
   ONECHSUM			: 1,							/* Two in channels, one out channel */
   SAVECOUNT			: 1,							/* ??? */
   RESETCOUNT		: 1,							/* ???*/
   spare17				: 1,							/* Unused */
   spare16				: 1,							/* Unused */
   spare15				: 1,							/* Unused */
   spare14				: 1,							/* Unused */
   spare13				: 1,							/* Unused */
   spare12				: 1,							/* Unused */
   spare11				: 1,							/*Bit 20: Unused */
   spare10				: 1,							/* Unused */
   SAVESETTINGS	: 1,							/* B22		Save current settings in FLASH */
   THREEBATTERIES	: 1,							/* Bit 23 Unused */
   BATTLOW				: 1,							/*Bit 24: Low battery level*/
   BATTFULL			: 1,							/*Bit 25: Battery is full */
   INVALIDSETUP	: 1,							/*Bit 26: Settings are invalid when read from FLASH */
   SHUTDOWN			: 1,							/*Bit 27: Shutting down the device								 	*/
   STIMENABLE		: 1,							/* Enable stimulation output or PAUSE      	*/
   KEY_DOWN			: 1,							/*Bit 29: Key is down */
   KEY_SHORTPRESS	: 1,						/*Bit 30: Short press on pushbutton 								*/
   KEY_LONGPRESS: 1;								/* Long press on pushbutton 								*/
 } bits;
} ModeType;
extern ModeType nMode;




#ifdef __cplusplus // important- Tell the compiler that we have c has ended
}
#endif

//End of guard
#endif   //__GLOBALDATA__
