"use strict";
document.addEventListener("DOMContentLoaded", mInit);
var oWatch={	//Object holding data for exchanging
	'Title':'Android Webview App 191113'
	,nPeriod:500		//Refresh period in ms
	,sFileName:'data.txt'	//Filename on the server
	,prot:{}		//protocol description from android
	,oVarDesc:{}	//{varname: _VarDescObj }Containing variable names
	,oVarData:{}	//{varname: _VarDataObj	} holding volatile data
	,wx:{}			//{window[id]:_WXObj	}Object of control settings wx.[id].[param]=
}

var _VarDescObj={	//Information about the dataobject protocol
	//held by oWatch.prot[VarName]
	VarName	:'FW varname'	//this object name
	,Alias	:'Desc name'
	,Factor	:1000	//raw 2 unit Conversion factor
	,Offset	:0		//subtracted before conversion
	,Unit	:'u'	//Name of the units mV,mA % etc
	,oData	:_VarDataObj	//access to the data
}
var _VarDataObj={
 
	//	held by oWatch.data[idWX]
	VarName	:'FW varname'	//this object name
	,nIdx: 0		//Index in Android protocol
	,Id: -1			//Valid range [64-128]Id  for cSerial protocol (reserved)
	,Size: 2
	,Vector:[1,2,3]		//Raw data
	,Poke:false			//Flag for sending data to device
	,Peek:true			//Flag for reading data from device
}
var _WXObj={			//Widget descriptor
	//	held by oWatch.wx[idWX]
	id		:' ID'
	,varname:'VarName'			//Identifier for the protocol object
	,Index	: 0				//Index for scalar in data vector
	,Range	: [0,100]		//Visible range
}

const prot={
	kCommInit : 101,		//Communication constants
	kGetReq : 111,  
	kSetReq : 102
	,get oVarData(){return oWatch.oVarData;} //Data
	,get oVarDesc(){return oWatch.oVarDesc;} //Description
	,get oWidgets(){return oWatch.wx;} 		 //Widgets 
	
	,mGetStatus(){
		if (bAndroid){
			oWatch.status=And.mStatus();		//Get the status
		}
	}
	,mDataExchange(){
		if (bAndroid){
			if (oRetData)
			if (oRetData.data)
			if (oRetData.data.Poke){
				oWatch=oRetData.data;
				prot.lastState='redraw'
				oWatch.Poke=false;
			}
			//mAndExchange();	//Data exhange to Android App
			mPHPCall(dataurl,'swap',oWatch);	//Get file and write new oWatch
		} else  {
			if (oWatch.Poke){ 			//mPHPCall(dataurl,'swap',oWatch,mCb);
				prot.lastState='poke'
				mPHPCall(dataurl,'save',oWatch);		
			} else{
				mPHPCall(dataurl,'load',oWatch,mCb);
			}
			function mCb(oRetData){		//when returning from PHP PROCESS SAVED oWatch
				if (oRetData.data)			oWatch=oRetData.data;
				if (oWatch){
					if (oWatch.Poke) {
						prot.lastState='redraw'
						oWatch.Poke=false;
					}
				}
			}
		}		
		oWatch.Poke=false;	//Done poking in the exchange process
	}
	,mInitialize(callback){ 
		//Start by loading the protocol from server
		mPHPCall(dataurl,'load',oWatch,function(){
			if (oRetData.data){
				oWatch=oRetData.data;
			} else {
				alert (oRetData.errors)
				return;
			}
			if (bAndroid){
				And.mReset( oWatch.status.SerialDeviceName)
			}
			oWatch.cmd=prot.kCommInit;
			if (bAndroid) oWatch.prot=And.FetchProtocol()		//Get the protocol from android
			prot.mGetStatus();
			callback()
		});
	}
	,mSetup(){	//Prepare the protocol
		//Add variables to protocol
		var ptr=oWatch;
		if (!ptr.oVarDesc){              // ||!ptr.oVarDesc.size()){
			ptr.oVarDesc={};
		}
		if(!ptr.oVarData)ptr.oVarData={}
		var aNames=Object.keys(ptr.prot);
		for (var i=0;i<aNames.length;i++){ 
			var sName=aNames[i];
			var obj=ptr.prot[sName];			 
			if(Array.isArray(obj)){
				//Override with DSP settings
				ptr.oVarData[sName]={}//Contain what comes from uP protocol
				if (!ptr.oVarDesc[sName])ptr.oVarDesc[sName]={}  //Initialize settings if not there
				var oVarData=ptr.oVarData[sName]
				//_VarDataObj type
				oVarData.VarName=sName;
				oVarData.Id=obj[0];		//Identifier of datavector (internal to And app)
				oVarData.nIdx=i;		//Variable Index in Android protocol
				oVarData.Size=obj[1];	//Length of datavector
				if (oVarData.Vector===undefined)
					oVarData.Vector=Array(oVarData.Size).fill(0)//Initialize the dataarray
					//todo: delete next statement as obsolete
					if (ptr[sName]&&ptr[sName].Vector)	oVarData.Vector=ptr[sName].Vector
				oVarData.Peek=false;
				oVarData.Poke=false;
				var oVarDesc=ptr.oVarDesc[sName];	//Preserve what is there in descriptor
				//_VarDescObj type	
				oVarDesc.VarName=sName;
				if (obj[2]!==undefined) oVarDesc.Unit=obj[2]
				if (obj[3]!==undefined) oVarDesc.Offset=Number(obj[3])	 //Raw data offset (unit value =zero)
				if (obj[4]!==undefined) oVarDesc.Factor=1/obj[4]	//unit value =factor*(raw-offset)
				if (!oVarDesc.Factor) oVarDesc.Factor=1;//Avoid zero division
				if (!oVarDesc.Alias)  oVarDesc.Alias=sName;
				delete(ptr.prot[sName]);//Removing used protocol entries
				prot.Poke();
			}
		}
	}
	,info(){
		if (!oWatch.status) oWatch.status={ProtState:0}
		return oWatch.status;
	}
	
	,Poke(oVarData){		//Set a flag to poke to device
		oWatch.Poke=true;
		if (oVarData)
			oVarData.Poke=true;
	}
	,aVarNames(){	//Get list of exposed variables
		return Object.keys(oWatch.prot);
	}
 
	,nIdx(sVarName){
		var i =prot.oVarData.nIdx[sVarName].nIdx
		return i;
	}
 
	,oVarProt(sVarName){//Return _VarDescObj etc
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

	,oWX(idWX){		//contains Index, min,max wrapper
		return this.mWX(idWX);
	} 
	, mWX(idWX){		//contains Index, min,max
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
 			,get	 Alias()	{return oVarDesc.Alias }
			,set	 Alias(s)	{oVarDesc.Alias =s}
			,Unit:function(s)	{if (s) oVarDesc.Unit =s;return oVarDesc.Unit } 
			,Factor:function(s)	{if (s) oVarDesc.Factor =s;return oVarDesc.Factor } 
			,Offset:function(s)	{if (s) oVarDesc.Offset =s;return oVarDesc.Offset } 
			,	Range		:mRange		//function
			,	id			:idWX
			,oVarDesc:oVarDesc
		}
		if (prot.oVarData[sVarName]) myVar.oVarData=		prot.oVarData[sVarName];
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
    ,mVarValue(sVarName,idx,val){		//Rev 191107
		//Get/Set value with digital conversion
		if (!idx==undefined) debugger;
		var oData=prot.oVarData[sVarName];
		if(!oData) return undefined;
		var oDesc=prot.oVarDesc[sVarName]
		if (val!=undefined){
			var raw=(val/oDesc.Factor)+Number(oDesc.Offset);
			oData.Vector[idx]=raw;
			prot.Poke(oData);
		} else {
			if (undefined ===oData.Vector[idx]) return undefined
			var val=oData.Vector[idx]
			if (val===undefined) debugger;	//Error you probably missed the index
			var u=(val-oDesc.Offset)*oDesc.Factor;
			oData.Peek=true;
			return u;
		}
	}
	,mVectorUnits(sVarName,vector){		//Rev 191108
		//Get/Set vector in data units 
		var oVarData=prot.oVarData[sVarName];
		if(!oVarData) return undefined;
		var oDesc=prot.oVarDesc[sVarName]
		if (vector!=undefined){
			for (var idx=0;idx<vector.length;idx++){//Scale units 2 raw data
				oVarData.Vector[idx]=(vector[idx]/oDesc.Factor)+Number(oDesc.Offset);
			}
			prot.Poke(oVarData);
		} else {
//			if (undefined ===oVarData.Vector) return undefined
			var vector=oVarData.Vector
			var vY=Array(vector.length);
			for (var idx=0;idx<vector.length;idx++){//Scale raw data to units
				 vY[idx]=(vector[idx]-oDesc.Offset)*oDesc.Factor;
			}
			oVarData.Peek=true;
			return vY;
		}
	}
}
 

