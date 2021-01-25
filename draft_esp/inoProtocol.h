//file: "inoProtocol.h"
/*doc: https://docs.google.com/document/d/1HoKNnbNBFE3pRI-E8wktXJT77zEehv3WXVZ2eMM2qAY/edit
*/

#ifndef ___cProtocol    // only once guard
#define ___cProtocol

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>				//Boolan types rt210107
#include <stdint.h>           //Define standard types uint32_t etc
//  GLOBALS					**************************************************** 21017
void mTesting();						//Declare your functions here for ino to recognize it
void mTesting2();						//test functions
//Interface methods
int mPopTXFIFO();		//Send to client
void mPushRX2FIFO(int var);   //Receive from client

void mProtocolProcess(void);
// A MACRO THAT DEFINE EXPOSE TO PROTOCOL for vectors
#define EXPOSEARRAY(obj,arg1) mTXExposeArray(obj,#arg1,arg1,sizeof(arg1)/sizeof(arg1[0]),kSend32Bit);
/* Expands to something like
	mTXExposeArray(&oTXProt,"xData",		   data name
		xData,														//Var pointer
		sizeof(xData)/sizeof(xData[0]),		//length kADCBuffsize
		kSend32Bit);
*/

#include "system.h"
extern EX_FLAGS bErrFlags;  /* Allocation for the Flags */

enum{			//Workaround for declaring a constant
	 kTXBufferSize=600 			//Size of UART sending buffer at 115kbaud, 690 bytes/cycle can be sent
};
/* Circular buffer object */
typedef struct sFIFO{
volatile    int         size ;   /* maximum number of elements           */
volatile    int         start;  /* index of oldest element              */
volatile    int         end;    /* index at which to write new element  */
volatile    char         full;   // Flag for buffer beeing full
volatile    char         empty;  // Flag for buffer beeing empty
volatile    uint8_t*			 elems;  /* vector of elements                   */
} *tFIFO;

extern tFIFO  oTX;			//The objects holding tramsmission data (rt210107)
extern tFIFO  oRX;
//#define EXPOSESCALAR(arg1) ExposeScalar(#arg1,arg1,1,sizeof(arg1));
//***** IMPORTING (Requiring the following methods to be defined)
void Expose2Protocol(void);					//+ A method that initializes the protocol by sending all variable names to host
void mSendVersionInfo(void);
// Communication constants
typedef enum commHeaders {
	kReady=0,
	kUartErrMsg=13,
  kCommInit = 101 ,//      '"Initialize Protocol request to DSP and preample for returned data [SymbolicVarName, VarId,ArraySize and Type]
  kHandshake = 104,//       '"Handshake"
  kSendByte=208,
   kSetReq = 102, //   		'Host write to uP memory
   kGetReq  = 111 ,//    	'Request to read uP memory
//   kDSP2HostData = 201//    'Response to DSP2HostCmd with data from DSP
	kSend24Bit=224,						//'Wordlength to send
	kSend32Bit=232
} tUartCmd;


/* -----------------------   PROTOCOL DEFINITIONS  ------------*/
#define kMaxVarCount 20         //Possible number of variables in the protocol
#define kIDOffset 64						// First id number (could also be an array of possible ids)
typedef struct {
    char VarCount;    // Number of variables registered by mTXExposeArray
    // The variables name is not preserved it is sent by Expose method and serves no more
    char VarId[kMaxVarCount];          // Unique identifier for host
    char VarLen[kMaxVarCount];         // vectorlength of data
    int* VarPtr[kMaxVarCount];          //Ptr to  data array
    tUartCmd VarType[kMaxVarCount];        // bytelength of data
		char TXCount[kMaxVarCount];					//Downcounter for transmissions
																				// when it reaches zero no more transmissions will be performed
}   tTXProt ;			//protocol type for RX/TX (before it was CLSProtPack)
extern tTXProt oTXProt;			//global object of tranmission protocol

/* END-------------------   PROTOCOL DEFINITIONS  ------------*/
// Packs
//kSend24Bit,VarId,Count,24bit data[count]


/********************* METHODS *************************/
	void mDispatchRX( );   	//Receive UART data and process it
	void mDispatchTX(tTXProt*  obj);
	//Run though object and send data witch have TXCount>0
	void mTXExposeArray(tTXProt*  obj,const char* name,int Arr[],uint8_t  ArrLength, tUartCmd  ArrType);
	// Register a variable and expose it to host
	void mProtocol_Init(tTXProt*  obj);			//Initializing the protocol




/****************PRIVATE******************************/
//extern void UART_TX_Trigger (void );			//Sends a datum if TX FIFO buffer is not empty and no data are being transmitted
//void Ucom_Send24bit(void* oTX,int VarId, int *  x24Bit2Send,int Count);
void Ucom_Send32bit(tFIFO oTX,int VarId, int *  Data2Send,int Count);
void mCommInitialize(void);	//	Initialize the communication buffer


// Type of elements to buffer
typedef struct { uint8_t value; } tBuffElem;
//161028




/*------------- INTERFACE -----------------------*/
tFIFO mFIFO_new( void* array,int size1);        //Returns an object of FIFO data
void mFIFO_del(tFIFO  cb);          // Destructor
// METHODS
int mFIFO_push(tFIFO  cb, uint8_t elem );//use mPushRX2FIFO and mPushTX2FIFO
int mFIFO_peek(tFIFO cb,   int offset );
int mFIFO_pop(tFIFO cb );
int mFIFO_size( tFIFO cb);					// Return size of buffer
bool mFIFO_isEmpty( tFIFO cb);				// Returns 1 if empty
int mFIFO_isFull(tFIFO  cb);				//FIFO is full
int mFIFO_Free(tFIFO   cb);                  // Number of free elements in buffer







#ifdef __cplusplus
}
#endif




#endif  //End of guard
