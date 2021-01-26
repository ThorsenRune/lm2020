//file inoProtocol.c  rev rt210107
/* Serial communication protocol
 Descrizione: vedi Documentation of Communication protocol LM
 gdoc:https://docs.google.com/document/d/1Y0uAmDxrib83ERQXSZ3OCNFLCmVO6x9wy5fkIZfH31E/edit
*/

#include "inoProtocol.h"
#include "publishvars.h"

uint8_t aTX[kTXBufferSize];		// Transmit array
uint8_t aRX[kTXBufferSize];		//Receive array
tFIFO oTX;             //Transmit buffer
tFIFO oRX;             //
tTXProt oTXProt;							//global object of tranmission protocol

typedef struct sFIFO1{
volatile    int         size ;   /* maximum number of elements           */
volatile    int         start;  /* index of oldest element              */
volatile    int         end;    /* index at which to write new element  */
volatile    char         full;   // Flag for buffer beeing full
volatile    char         empty;  // Flag for buffer beeing empty
volatile    uint8_t*			 elems;  /* vector of elements                   */
} *tFIFO1;

void mCommInitialize(void){
  if (nDbgLvl&(2<<6))  mDebugMsg("mCommInitialize");
 //Initialize the communication protocol allocating memory for buffers

 oTX=mFIFO_new(aTX,sizeof(aTX)); // get a new object
 oRX=mFIFO_new(aRX,sizeof(aRX)); // get a new object
 if (nDbgLvl&(2<<6))  mDebugInt("oRX Free",mFIFO_Free(oRX));
}

void mTX_PushTxt(const char* PushTxt) {
 //Send a string on the serial channel

   uint8_t zlen,j;
   for (zlen=0;((PushTxt[zlen]>=32)&(zlen<50));zlen++){}; 	// Find length of string
   mFIFO_push(oTX,zlen);
   for (  j=0;j<zlen;j++){
     mFIFO_push(oTX,PushTxt[j]);   // Push on TX buffer
   }
}
void ExposeVars2Host(const char* VarName,uint8_t VarId,uint8_t ArrLen,uint8_t VarType)    // Send the k
{
//  dbprint(VarName,VarId,ArrLen,VarType);
   mFIFO_push(oTX,kCommInit);				// Header
   mTX_PushTxt((char*) VarName);							//Send symbolic name
   mFIFO_push(oTX,VarId);							// Send identifier
   mFIFO_push(oTX,ArrLen);							//Send ArrLen
   mFIFO_push(oTX,VarType);   //Send VarType
}

void mTXExposeArray(tTXProt*  obj,const char* name,int Arr[],uint8_t  ArrLength, tUartCmd  ArrType)
// Register a variable and expose it to host
//  Called by using the macro EXPOSEARRAY
//  transferred from LM firmware by RT201218
{
   if (obj->VarCount<kMaxVarCount){	//Only if not full
         obj->VarId[obj->VarCount]=obj->VarCount+kIDOffset;
         obj->VarPtr[obj->VarCount]= Arr;
         obj->VarLen[obj->VarCount]=ArrLength;
         obj->VarType[obj->VarCount]=ArrType;
/*todo uncomment          ExposeVars2Host(name,obj->VarId[obj->VarCount],
                         ArrLength,
                         ArrType);
*/
       ExposeVars2Host(name,obj->VarId[obj->VarCount],
                        ArrLength,
                        kSend32Bit);
         obj->VarCount++;                     //Increment number of registered elements
         };
}

//--------------    HELPER FUNCTIONS (Could be encapsulated as private)


/* For future use */
void Ucom_Send24bit(void * oTX,int VarId, int *  x24Bit2Send,int Count){
 int i;
       mFIFO_push(oTX,kSend24Bit);	// xmit header
       UART_TX_Trigger();			// Start transmission
       mFIFO_push(oTX,VarId);			// xmit VarId
       mFIFO_push(oTX,Count);			// xmit Count
   for (i=0; i< Count; i++){		//!!!implement overflow check
     while (mFIFO_Free(oTX)<3)
       {UART_TX_Trigger();
         bErrFlags.errbits.bTXOverflow =1;
       }
         mFIFO_push(oTX, x24Bit2Send[i]>>16);			//RT:Send ADC0 data (bit 16:24)
         mFIFO_push(oTX, x24Bit2Send[i]>>8);			//RT:Send ADC0 data (bit 16:24)
         mFIFO_push(oTX, x24Bit2Send[i]>>0);			//RT:Send ADC0 data (bit 16:24)
       }
}


// In host its received by ShowSignals
void Ucom_Send32bit(tFIFO oTX,int VarId, int *  Data2Send, int Count){
/*	@oTX: Pointer to the FIFO register
   @VarId:	Identifier of the variable
   @Count:	Number of elements in the array (1 is scalar)
   @Data2Send:	The array of data to send as 32 bit data (4 bytes)
*/
 int i;

  if  (mFIFO_Free(oTX)<3) mDebugHalt("oTX fifo overflow");
       mFIFO_push(oTX,kSend32Bit);	// xmit header
       mFIFO_push(oTX,VarId);			// xmit VarId
       mFIFO_push(oTX,Count);			// xmit Count
       for (i=0; i< Count; i++){		//!!!implement overflow check
   while (mFIFO_Free(oTX)<4)
         {//UART_TX_Trigger();
           bErrFlags.errbits.bTX32Overflow =1;
         }
         mFIFO_push(oTX, Data2Send[i]>>24);			//RT:Send ADC0 data (bit 16:24)
         mFIFO_push(oTX, Data2Send[i]>>16);			//RT:Send ADC0 data (bit 16:24)
         mFIFO_push(oTX, Data2Send[i]>>8);			//RT:Send ADC0 data (bit 16:24)
         mFIFO_push(oTX, Data2Send[i]>>0);			//RT:Send ADC0 data (bit 16:24)
       }
}


/*
   Protocol manipulations using tTXComm types


*/
int8_t mTXVarId_Find(tTXProt* obj, char zVarId)   //return index of varid
/* 	@obj: 		the protocol object
   @zVarId:	the ID of the variable to find
   @return:	index of the sought variable
*/
{
   char i;
   for (i=0; i<obj->VarCount ; i++ )
     {  if (zVarId==obj->VarId[i])return i;
     }
     return -1;
};

//Handshake
void mSendVersionInfo(){//Send information about the firmware
   mFIFO_push(oTX,kHandshake);				    // Header
   mTX_PushTxt((char*) FWversion);				//Send Firmware information
}



////////////////////////////	RX TX Dispatcher	////////////////////////////

//todo7: refactor mDispatchTX /mDispatchRX to mEncodeRFTX /mDecodeRFRX
void mDispatchTX(tTXProt*  obj){   //Run though object and send data witch have TXCount>0
/* 	@obj: 		the protocol object
using:void Ucom_Send32bit(tFIFO oTX,int VarId, int *  Data2Send, int Count)
*/
   char i;
   for (i=0; i<obj->VarCount ; i++ )
     {  if (0< obj->TXCount[i])
       {
         //send the data
         if (obj->VarType[i]==kSend32Bit){
            Ucom_Send32bit(oTX,obj->VarId[i], obj->VarPtr[i],obj->VarLen[i]);
          }
/* todo9: implement later
         else if (obj->VarType[i]==kSend24Bit)
         {Ucom_Send24bit(oTX,obj->VarId[i], obj->VarPtr[i],obj->VarLen[i]);}
         */
         obj->TXCount[i]=obj->TXCount[i]-1;			//Decrease counter
       }
     }
 /*		if (bErrFlags.all_flags[0]) {
       Ucom_Send32bit(oTX,kUartErrMsg,bErrFlags.all_flags ,1);
     }	*/
}

void mDispatchRX( ){   //Receiving data and dispatch commands
/* 	@obj: 		the protocol object
  //Processing the serial receive data and
   // puts them in the protocol buffer
Revisions:
*/
  static uint8_t rcv1,idx;			//Data received
  static tUartCmd rxCmd=kReady;     // Current cmd state
  static uint8_t zState;				// State machine indicator
  static int *  DestData;
  while (!mFIFO_isEmpty(oRX)) {     // Char in buffer, go read and process it
    if (nDbgLvl>3) mDebugInt("Available",mFIFO_available(oRX));
    rcv1=mFIFO_pop(oRX);       //Get new data
    if (nDbgLvl&(2<<6)) mDebugInt("popped",rcv1);
    if (0==rxCmd) 			//Set command as receive data
      {
        rxCmd=(tUartCmd) rcv1;
        zState=0;			//Restart statemachine
        // Process single command procedures
         if  (kCommInit ==rxCmd)				// Reset the protocol by sending exposed variables
         {
           if (nDbgLvl&(2<<6))  mDebugMsg("dbExpose2Protocol");
           Expose2Protocol();
           rxCmd=kReady;											//Command is processed
         } else if (kHandshake==rxCmd)
         {
           if (nDbgLvl&(2<<6))  mDebugMsg("dbmSendVersionInfo");
           mSendVersionInfo();
           bErrFlags.all_flags[0] = 0U	;		//Reset error flags
            rxCmd=kReady;											//Command is processed  RT210121  (Forgot this)
         }
      }
     //Longer messages will be processed here
    else if (kSetReq ==rxCmd)					//Set values of variable.Client writes to variable
     {
       if (0==zState){				// Get variable
         idx=mTXVarId_Find(&oTXProt,rcv1);
         DestData=oTXProt.VarPtr[idx];	//Make a pointer to the address
         zState=1;
       }
       else if (1==zState)		//Index of array to write to
         {
         idx=rcv1;
         zState++;
         }
       else if (2==zState)
       {
         DestData[idx]=rcv1<<24;
         zState++;
       }
       else if (3==zState)
       {
         DestData[idx]+=rcv1<<16;
         zState++;
       }
       else if (4==zState)
       {
         DestData[idx]+=rcv1<<8;
         zState++;
       }
       else if (5==zState){
         DestData[idx]+=rcv1;
         rxCmd=kReady;											//Command is processed
       }
     }
    else if (kGetReq ==rxCmd)					//Request data command. Client want to read variable
    {		//Expect ID and increase its send counter
       idx=mTXVarId_Find(&oTXProt,rcv1);
       oTXProt.TXCount[idx]++; //Make it send
       zState=0;				//End  statemachine
       rxCmd=kReady;
        if (nDbgLvl&(2<<6))  mDebugInt("kGetReq for ",idx);
       //todo: enable mPowerWatchDogReset();		//R180920 Reset the watchdog for power timeout
     }
     else {
       //This would be a reveive error because rxCmd was not recognized
       mDebugMsg("bReceiveError in mDispatchRX");
       bErrFlags.errbits.bReceiveError =1;
       zState=0;				//End  statemachine
       rxCmd=kReady;
     }
   }
}


void mProtocol_Init(tTXProt*  obj)						//Initialize protocol object
{
 obj->VarCount=0;
}



/****************MAIN ENTRY***********************/

void mPushRX2FIFO(int var){   //Push data from client to the FIFO
   mFIFO_push(oRX,var);
     if (mFIFO_isFull(oRX)) {
       bErrFlags.errbits.bRXOverflow =1;	//Receive overflow
     }
}
int mPopTXFIFO(){
  return mFIFO_pop(oTX);
}
void mProtocolProcess(void){
   mDispatchTX(&oTXProt);
   mDispatchRX( );
}




/********************       FIFO BUFFERS ***********************/

/*------------------------- IMPLEMENTATION --------------------*/
// Initialize the buffer
tFIFO mFIFO_new( void* array,int size1) {
   tFIFO cb=malloc(sizeof(struct sFIFO));  //Mandatory
   cb->size  = size1;
   cb->start = 0;
   cb->end   = 0;
   cb->full = 0;
   cb->empty = 1;  // Initially the buffer is empty
   cb->elems = array;
   return cb;
}

// Return full flag
int mFIFO_isFull(tFIFO cb) {
   return cb->full; }


// Return empty flag
bool mFIFO_isEmpty(tFIFO  cb) {
  if (cb->empty==1)
    return true;
  else
    return false;
   //return cb->empty;
 }
int mFIFO_available(tFIFO  cb){
  return mFIFO_size(cb)-mFIFO_Free(cb);
}



// Size of buffer
int mFIFO_size(tFIFO  cb) {
   return cb->size; }
// Size of buffer


int _mFIFO_FreeSub(tFIFO  cb) {
   // Its empty so return the buffersize
   if (cb->empty)
       return cb->size;
   // Start is behind end so end can rollover (+size)
   if (cb->end>cb->start)
       return cb->size-(cb->end-cb->start);
 // End is before start:start-end is free
   if (cb->end<cb->start)
       return cb->start-cb->end;
   if (cb->full)
       return 0;
 return 0; // (cb->full)
}
int mFIFO_Free(tFIFO  cb) {
 int i;
     //INTDIS;
     i=_mFIFO_FreeSub( cb);
     //INTENA;
     return  i;
}


// Push an element on the buffer, returns 1 when full else 0
int mFIFO_push1(tFIFO cb, uint8_t val ){
   cb->empty=0;                //Not empty anymore
   if (cb->full) {				//Buffer is full
       if (cb->full){		//Overflow condition (wait and hope)
//							INTENA;			//Enable and disable interrupt
//							mWaitMs(1);
             bErrFlags.errbits.bFifoOverflow=1;
//							INTDIS;
         }							//If full wait until empty
   }
   cb->elems[cb->end]  =  val;
   cb->end = (cb->end + 1);
   if (cb->end >= cb->size){   //Rollover
       cb->end=0;
   }
   if (cb->end == cb->start){   //Buffer full
       cb->full=1;
   }
   return cb->full;
}
int mFIFO_push(tFIFO cb, uint8_t val ){     //Incoming data from serial stream
 int i;
      //INTDIS;   //Disable interrupts
      //todo: if no send buffer and cb=oTX then
  if (mFIFO_isFull(cb)) 					{  //todo8: remove redundance of  bRXOverflow
    bErrFlags.errbits.bRXOverflow =1;	//Receive overflow
    if (cb==oRX) mDebugMsg("oRX buffer problem");
    else  mDebugMsg("oTX buffer problem");
    mDebugHalt("FIFO Overflow in mFIFO_push");
  } else {
    i=mFIFO_push1(cb, val );
    return i;
  }
}


// Check an element in the buffer, returns  o1 when full else 0
int mFIFO_peek(tFIFO cb, int offset ){
 int count=0,pos=0;
   count=(cb->end-cb->start);          // Number of elements in buffer
   if (count<0) count=count+cb->size;      //Rollover
   pos = (cb->start + offset);
   if (pos >= cb->size) {   //Rollover
       pos=pos-cb->size;
   }
   return cb->elems[pos];
}




// Get oldest element from the buffer.
int mFIFO_pop1(tFIFO cb ){
   int val=0;
   cb->full=0;                //Not full anymore
   val = cb->elems[cb->start];
   cb->start = (cb->start + 1);
   if (cb->start >= cb->size){   //Rollover
       cb->start=0;
   }
   if (cb->end == cb->start){   //Buffer full
       cb->empty=1;
   }
   return val;
}
int mFIFO_pop(tFIFO cb ){
 int i;
      //INTDIS;
     i= mFIFO_pop1(cb );
     //INTENA;
   return i;
}
