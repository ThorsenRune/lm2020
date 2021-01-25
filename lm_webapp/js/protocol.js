// Communicatin protocol The protocol as the data packages which can be exchanged with the device
 "use strict"; //declare vars
//------------ TX RX to the LM device

var konst={}
/*********** COMMUNICATION PREAMBLE CONSTANTS    ********************/
konst.kError	=13 ;	 	//FSM constant - signalling error
konst.kReady	=100;	 	//FSM constant - signalling ready
konst.kHandshake=104;      	// Waiting for the protocol to settle get FW version info in return
konst.kCommInit=101			// Request to initialize Protocol, send request to target (R170314)
konst.kSetReq = 102;     	//Client write to device memory
konst.kGetReq = 111;  		//Request to read variable from server, this will increase a pointer in the target causing a send of the variable
konst.k32Bit=232			// Receving data from device
///////////////////////////////////////////////////////////////
const prot={		//Encapsulating communication protocol
	kCommInit 		: konst.kCommInit		//Communication constants
	,kGetReq 		: konst.kGetReq
	,kSetReq 		: konst.kSetReq
	,sFirmwareInfo	:''						//Firmware information returned by the kHandshake
  ,oData:            //New JSON ready object for the protocol definition (ex. oWatch)
  {	//Object holding data for exchanging
  	'Title':'Application name and version'	//Default string overwritten by JSON data in setup file
  	,nPeriod:2000		//Refresh period in ms
  	,sFileName:'data.txt'	//Filename on the server
  	,oVarDesc:{}	//{varname: _VarDescObj }Containing variable names
  	,oProtElemVar :{}	//{varname: _VarDataObj	} holding volatile data
  	,wx:{}			//{window[id]:_WXObj	}Object of control settings wx.[id].[param]=
  }
	,get oProtElemVar (){return prot.oData.oProtElemVar;} //Protocol element (alias: oProtElemVar =prot.oData.oProtElemVar;  //Todo untangle
	,get oVarDesc(){return prot.oData.oVarDesc;} //Description
	,get oWidgets(){return prot.oData.wx;} 		 //Widgets
	,SetElement(el){	//Copy or create FW specific protocol element
		var varname=el.VarName
    if (!this.oProtElemVar[varname]) {//Element is Undefined
      this.oProtElemVar[varname]={};  //Creaate it
    }
    var dst=this.oProtElemVar[varname]
		this.oProtElemVar[varname]=Object.assign(dst,el)  //Overwrite or create
    return this.oProtElemVar[varname];  //Return the element
	}
	,ElementByName(VarName){
		return prot.oProtElemVar[VarName]
	}
	,ElementByVarId(varid){		//Return the element by its variable id
		var aNames=Object.keys(prot.oProtElemVar)		for (var i=0;i<aNames.length;i++){
			var el=prot.oProtElemVar[aNames[i]];
			if (el.nVarId==varid) return el;
		}
	}
	,ListVarNames(){
		var els=this.oProtElemVar;
		var lst=Object.keys(els)
		return lst;
	}
  ,setVector(el,vector){    //Sets the dataarray of the element
    el.pokedata=true;       //Sets the flag requesting to set data on device
    el.Vector=vector;
  }
  ,getVector(el){    //Sets the dataarray of the element
    oData.peekdata=true;	//Request a readback from device
    return vector
  }
  ,get refreshRate(){return this.oData.nPeriod;}
  ,set refreshRate(newvalue){ this.oData.nPeriod=newvalue;}
}
/* pattern example
prot.oProtElemVar.__proto__.setVector=function() {
	alert ('Implement setVector');
}
*/
prot.state=function(setState){ //Returns the state of the protocol
  if(typeof(this._state)==='undefined') this._state=konst.kCommInit;
  if(typeof(setState)!=='undefined')
  {
    if (setState==konst.kReady) mMessage("Changed protocol state to:READY")
    else if (setState==konst.kCommInit) mMessage("Changed protocol state to:kCommInit")
    else mMessage("Changed protocol state to: "+setState)
    this._state=setState;
  }
  if (this._state==konst.kReady) return this._state
  if (validate()) this._state=konst.kReady
  return this._state
  function validate(){	//Check if protocol is received from device
  //						all elements have nVarId valid number in range 64--100
  //rev201125			todo:move into object
  	var aNames=Object.keys(prot.oProtElemVar)
    if (!aNames.length) return false;
  	for (var i=0;i<aNames.length;i++){
  		var el=prot.oProtElemVar[aNames[i]];
  		if (!el.nVarId) return false; 		//Undefined id
  		if (el.nVarId<64) return false;			//Unset element
  		if (el.nVarId>100) return false;		//Some error
  	}
    debugger
  	return true;
  }
}
prot.CleanUp=function(){  //Remove unregistered variables from protocol
//						all elements have nVarId valid number in range 64--100
  var aNames=Object.keys(prot.oProtElemVar)
  for (var i=0;i<aNames.length;i++){
    var el=prot.oProtElemVar[aNames[i]];
    if (!el.nVarId)delete(prot.oProtElemVar[aNames[i]]); 		//Undefined id remove it
  }
}

//Higher level functions


prot.mTX_ProtReset=function(){		//Resettig the communicato protocol with kCommInit
	if (debug) mMessage('Resetting protocol',true);
  serial.send(konst.kHandshake);  //Send the reset request to device (There is some bug so resend the request)
	serial.send(konst.kCommInit);  //Send the reset request to device (There is some bug so resend the request)
  prot.state('initializing');  //Set protocol as invalid until validated
  //Will be altered in mRX_ProtInit
}


prot.TXDispatch=function(){	//mTXDispnDataLengthatch - the response will be mRXDispatch
	//Using the statpointer prot.State
	//The reinitialization is in case of device beenig reset
	//Chaining the events according to the prot.State FSM pointer
	if (!serial.isReady()) return	//Return serial channel is not open
	if (konst.kCommInit==prot.state()){	//Request handshake and protocol if not waiting
		if (mWatchDog.waiting) return prot.state()
		//Else request handshake and protocol
		prot.mTX_ProtReset();
		//if (prot.ListVarNames().length==received elements
		display.doRedraw=true;
	} else if (prot.state()==konst.kReady){
		var aNames=Object.keys(prot.oProtElemVar)
		for (var i=0;i<aNames.length;i++){
			var VarData=prot.oProtElemVar[aNames[i]];
      if (!VarData.nVarId){//          mMessage("Unregistered variable "+aNames[i]);
//          debugger
      }else if (VarData.pokedata){
				mTX_SetReq(VarData);
				VarData.pokedata=false;	//Clear write request flag
			} else if (VarData.peekdata)
				mTX_GetReq(VarData);
				VarData.peekdata=false;//Clear read request flag
		}
	} else if (prot.state()==prot.kError){
		debugger //todo some action on protocol error
	}
  function mTX_SetReq(oProtElem){      /* Change data in device memory          Write data to device memory*/
  //Sending: Cmd,Varid,addressoffset,4databytes
      var offs;
      if (!oProtElem.nVarId){
          mDebugMsg("mTX_SetReq Unregistered variable: "+oProtElem.VarName);
          debugger
          return;
      }
      if (!oProtElem.Vector) return false;
  	//Loop through object data elements
      for(offs=0; offs<oProtElem.nDataLength; offs++)
  	{
  		serial.send(konst.kSetReq);                      //command
  		serial.send(oProtElem.nVarId);             //array identifier
  		serial.send(offs);                         //offset in array
  		var b = utils.mInt2ByteArray(oProtElem.Vector[offs]);
  		serial.send(b[0]);        //value of the array[offset]
  		serial.send(b[1]);        //value of the array[offset]
  		serial.send(b[2]);        //value of the array[offset]
  		serial.send(b[3]);        //value of the array[offset]
  	}
  }
  function mTX_GetReq(oProtElem){        //TX Get request - use mRXGetReq to receive the response
      if (!oProtElem.nVarId){
          mDebugMsg("mTX_GetReq:Unregistered variable: "+oProtElem.VarName);        debugger
          return;
      }
      serial.send(konst.kGetReq);
      serial.send(oProtElem.nVarId);
  }


}
prot.mRXDispatch=function(RXFiFo){//201112   from java mRXDispatch
	if (!serial.isReady()) return	//Return serial channel is not open
	if (serial.RXFiFo.length<3) return false;//No package is shorter than 3 bytes
	if (konst.kHandshake==RXFiFo[0]){
		return mRX_Handshake(RXFiFo);
	} else if (konst.kCommInit==RXFiFo[0]){
    var ret=mRX_ProtInit();
		return ret
	} else if (konst.k32Bit==RXFiFo[0]){
		var ret= mRXGetReq(RXFiFo);
    if (debug) mShowDropDownValue();
    return ret
	} else {	//This should not happen but if data gets scrambled it does
		mDebugMsg('mRXDispatch:Error in protocol, flushing protocol');
    serial.RXFiFo=[]
		debugger
	}
	function mRX_Handshake(){	//Return the handshake
		if (serial.RXFiFo[1]>serial.RXFiFo.length-2) return false; //Pack not ready yet
		var nBytes=serial.RXFiFo[1]
		prot.sFirmwareInfo=''
		for (var i=0;i<nBytes;i++) {
			var chr=			String.fromCharCode(serial.RXFiFo[i+2])
            prot.sFirmwareInfo = prot.sFirmwareInfo+ chr
        }
		serial.RXFiFo=serial.RXFiFo.slice(nBytes+2);	//Remove cmd,length,payload
		mMessage(prot.sFirmwareInfo,true)
		return true
	}

	function mRX_ProtInit(){	//Register element in protocol
	//Rev 2020
	// Todo: determine when the protocol has been completed
	//cmd,bStringLength,sElementName,bVarId,bDataLength,bVarType
	//Requires stringlength+4 bytes
		if (serial.RXFiFo[1]+5>serial.RXFiFo.length) return false; //Pack not ready yet
		var el={}     //Create an empty element
		var idx=1;
		var nBytes=serial.RXFiFo[idx]; //Length of element name
		el.VarName=''
		for ( idx=2;idx<nBytes+2;idx++) {
			el.VarName=el.VarName+ String.fromCharCode(serial.RXFiFo[idx])
        }
		el.nVarId=serial.RXFiFo[idx++]  //Todo:9 refactor all to be zVarId like in FW code
		el.nDataLength=serial.RXFiFo[idx++];//Length of data array
		el.nVarType=serial.RXFiFo[idx++];//Length of data array
		el=prot.SetElement(el);	//Register the element in the protocol and get a refernce to the protocol element
    //Bugfix: rt210125
    if (!el.Vector) el.Vector=Array(el.nDataLength).fill(0);      //Create vector it not preinitialized in setup sFileName
    //Otherwise use default values
    else if (el.Vector.length!=el.nDataLength) el.Vector=Array(el.nDataLength).fill(0); //Be sure the length is correct
    if (typeof el.pokedata=='undefined') el.pokedata=false;   //Initialize pokedata flag
    if (typeof el.peekdata=='undefined') el.peekdata=false;   //Initialize peekdata flag
    if (prot.ListVarNames().indexOf(el.VarName)>(prot.ListVarNames().length-2)) {  //Assume the protocol is ready when last var has been received
      prot.state(konst.kReady); //Assume that prot is ready
    }
		serial.RXFiFo=serial.RXFiFo.slice(idx);	//Remove cmd & payload
		if (debug) mMessage(JSON.stringify(el));
		display.doRedraw=true;	//request to redraw the display
		return true
	}
	function mRXGetReq(){  //The response to a mTX_GetReq
	//cmd,id,bDataLength,bDataLength*data
	//Requires stringlength+4 bytes
		var idx=1;
		if (6>serial.RXFiFo.length) return false; //Pack not ready yet, minimum pack is cmd,id,data = 6bytes
		var nId=serial.RXFiFo[idx++];		//Identfier
		var nBytes=serial.RXFiFo[idx++]; //Length of data array
		if (3+(4*nBytes)>serial.RXFiFo.length) return false; //Pack not ready yet, minimum pack is cmd,id,data = 6bytes
		var data=[];
		for (var i = 0; i< nBytes; i++) {   //Perform bitwise or with the data shifted
			var b=[]
			b.push(serial.RXFiFo[idx++])
			b.push(serial.RXFiFo[idx++])
			b.push(serial.RXFiFo[idx++])
			b.push(serial.RXFiFo[idx++])
			data.push(utils.ByteArray2Long(b))
		}
		serial.RXFiFo=serial.RXFiFo.slice(idx);	//Remove cmd & payload
		var el=prot.ElementByVarId(nId)
    if (!el) return;
		if (el.pokedata) {
      debug //Don't read if you are writing
    }else{
      el.Vector=data
    }
		if (debug) debug.ReceivedElement=el
		return true
	}

}



/*  (De)Codign of commuication packages
*    //-----------------------------------mTX_Dispatch--------------------------------------------------------------------
*   R170316   revision to use new FSM constants for the protocolstate
*   R170314   some bugfixes, separating protocol states in requesto and acknowledge receipt
*   rev:161203/RT                    refactored to take the state as by reference
*   +170601  returns true if data are sent and some response is expected
*	201111 refact java -> javascript
*/

/*					Prot specific helper functions				*/
prot.lib={}

/**********************************/
//*******************  HELPERS

prot.info=function(){
		if (!prot.oData.status) prot.oData.status={ProtState:0}
		return prot.oData.status;
	}

prot.aVarNames=function(){	//Get list of exposed variables
		return Object.keys(prot.oData.prot);
	}


prot.oVarProt=function(sVarName){//Return _VarDescObj etc
		//access to _VarDescObj and  _VarDataObj
		var oDesc=_VarDescObj;	//Initialize
		//Set extending functions
 		function myUnit(sNew){//191107
			if(sNew){	prot.oVarDesc[sVarName].Unit=sNew;		}
			return prot.oVarDesc[sVarName].Unit;
		}
		function myAlias(sNew){//191107
			if(sNew){	prot.oVarDesc[sVarName].Alias=sNew;		}
			return prot.oVarDesc[sVarName].Alias;
		}
		function myFactor(sNew){//191107
			if(sNew){	prot.oVarDesc[sVarName].Factor=sNew;		}
			return prot.oVarDesc[sVarName].Factor;
		}
		function myOffset(sNew){//191107
			if(sNew){	prot.oVarDesc[sVarName].Offset=Number(sNew);		}
			return Number(prot.oVarDesc[sVarName].Offset);
		}

		var oRet={
			get varname()	{return sVarName}
			,Alias	:myAlias	//Use function
			,Value	:myValue //Use function
			,Unit	:myUnit
			,Factor	:myFactor
			,Offset	:myOffset
		}
		return oRet;
		function myValue(idx,val){
			return prot.mVarValue(sVarName,idx,val)
		}
	}

prot.oWX=function(idWX){		//contains Index, min,max wrapper
		return this.mWX(idWX);
	}
prot.mWX=function(idWX){		//contains Index, min,max
	//access to _WXObj ->_VarDescObj -> _VarDataObj
		if(!idWX)  idWX=mCurrentCtrlId();	//Identifier of current control
		if(!idWX) return null;				//No active widget
		//Ensure the widget is defined in settings
		if(!prot.oWidgets[idWX])prot.oWidgets[idWX]={}
		var oWX=prot.oWidgets[idWX] 	//object in control settings
		//Get the reference
 		oWX.id=idWX					//Align Identifier
		var sVarName=oWX.varname;
		var oVarDesc=		prot.oVarDesc[sVarName];
		var myVar={
			 VarName		:_VarName
			,    Index		:_Index
			, 	 Value 		:nValue
			, VectorUnits	:_VectorUnits
 			,get	 Alias()	{
				if(oVarDesc)				return oVarDesc.Alias
			}
			,set	 Alias(s)	{oVarDesc.Alias =s}
			,Unit:function(s)	{if (s) oVarDesc.Unit =s;return oVarDesc.Unit }
			,Factor:function(s)	{if (s) oVarDesc.Factor =s;return oVarDesc.Factor }
			,Offset:function(s)	{if (s) oVarDesc.Offset =s;return oVarDesc.Offset }
			,	Range		:mRange		//function
			,	id			:idWX
			,oVarDesc:oVarDesc
		}
		if (prot.oProtElemVar[sVarName]) {
			myVar.oProtElemVar=		prot.oProtElemVar[sVarName];
		}
		function _VectorUnits(vector){
			return prot.mVectorUnits(sVarName,vector)
		}
		function nValue(val){//191107
			var oData=prot.oVarProt(oWX.varname)
			//if (val!==undefined)				oData.Value(oWX.Index,val)
			//Using prot.mVarValue(sVarName,idx,val)
			return oData.Value(oWX.Index,val)  //Sets val if given
		}

		function _VarName(sName){
			if (sName !==undefined) oWX.varname=sName;
			if (!oWX.varname) oWX.varname=undefined;
			return oWX.varname;
		}
		function _Index(idx){
			if (idx !==undefined) oWX.Index=idx;
			return oWX.Index
		}
		function mRange(aNew){
			if (aNew!==undefined)oWX.Range=aNew;
			return oWX.Range;
		}
		return myVar;
	}
prot.mVarValue=function(sVarName,idx,val){		// Rev 191107 todo: replace by prot.getVector
		//Get/Set value with digital conversion
		//in: name of the element, idx= the index in the data array , val-if given will set the new value
		if (!idx==undefined) debugger;
		var oData=prot.oProtElemVar[sVarName];
		if(!oData) return undefined;
		var oDesc=prot.oVarDesc[sVarName]
		if (val!=undefined){
			var raw=(val/oDesc.Factor)+Number(oDesc.Offset);
		    if (oData.Vector)			oData.Vector[idx]=raw;    //RT210121  guard void
			oData.pokedata=true;	//Request a write to device
			oData.peekdata=true;	//Request a readback from device
		} else {
      if (undefined ===oData.Vector) return undefined
			if (undefined ===oData.Vector[idx]) return undefined
			var val=oData.Vector[idx]
			if (typeof val==='undefined') debugger;	//Error you probably missed the index
      if (typeof oDesc.Offset==='undefined') debugger;//todo fix potential errors
			var u=(val-oDesc.Offset)*oDesc.Factor;
			oData.peekdata=true;
			return u;
		}
	}
prot.mVectorUnits=function(sVarName,vector){		//Rev 191108
		//Get/Set vector in data units
		var oProtElemVar =prot.oProtElemVar[sVarName];
		if(!oProtElemVar ) return undefined;
		var oDesc=prot.oVarDesc[sVarName]
		if (vector!=undefined){
			for (var idx=0;idx<vector.length;idx++){//Scale units 2 raw data
				oProtElemVar .Vector[idx]=(vector[idx]/oDesc.Factor)+Number(oDesc.Offset);
			}
			prot.pokedata(oProtElemVar );
		} else {
//			if (undefined ===oProtElemVar .Vector) return undefined
			var vector=oProtElemVar.Vector       //The data array
      if (vector){
  			var vY=Array(vector.length);
  			for (var idx=0;idx<vector.length;idx++){//Scale raw data to units
  				 vY[idx]=(vector[idx]-oDesc.Offset)*oDesc.Factor;
  			}
      }
			oProtElemVar.peekdata=true;           //Request a new read from LM device
			return vY;
		}
	}






/******************DOCUMENTATION***************************



Protocol Serial Swimlane diagram

+──────+             +────────+
|Client|             |  |LM|  |
++─────+             +───+────+
 |   kHandshake          |
 |──────────────────────>|
 |  Firmware Version     |
 |<──────────────────────+
 :                       :
 |   kCommInit           |
 +───────────────────────>
 |  FW Variable format   |
 |<───────────────────── +
 |  FW Variable format   |
 |<──────────────────────+
 |      1..n (Note201113)|
 |                       |
 :                       :
 +  kGetReq = 111        +
 |──────────────────────>|
 |  k32Bit=232+data		 |
 |<──────────────────────+

 :                       :
 |  kSetReq = 102 + data |
 |──────────────────────>|
 |  none				 |
 |<──────────────────────+



http://localhost/public/asciiflow/


Note201113: The mRX_ProtInit will repeat for each element in the exposed protocol








*/
