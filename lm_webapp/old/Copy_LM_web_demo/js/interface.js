"use strict";
var oVal={};			//Object of values
var bRelay= (typeof Android =='undefined') ;//Android is an object injected from the  Webview when using the slider.
var bAndroid=!bRelay;		//We are in app
//var dataurl='https://thorsen.it/test/webview/data.php'	;
var dataurl='data.php'	;
//var dataurl='data.php'	;

// PROTOCOL
/*
	if not connected then set:
	mCommunicate(true);
    cProgram3.oaProtocols[0].mStateSet(cKonst.eProtState.kDoReconnect)
	
	Android.mJS2AndCmd('BTPair')		to pair a BT
	Android.mJS2AndCmd('Status')		to get protocol
*/

//----------------------   SERVER   
var oRetData={};	//Create a permanent copy 
var mAndExchange=function(){	//Write old data and get new from server
	if (bAndroid) {
		for (var sName in prot.oVarData){
			var obj=prot.oVarData[sName];
			if (obj){
			if (obj.pokedata){
				mPokeData(obj)
				obj.pokedata=false;
			} else if (obj.Peek){
				mPeekData(obj)
				obj.Peek=false;
			}}
		}
	}	
	function mPeekData(oVarData)	{	//Read from device or server
		if (!oVarData) return;  //The protocol hasnt been initialized
		if (bRelay) return;	//Not in app
		var varidx=oVarData.nIdx;		//Index of the variable for oaProtElem[varidx].mDataRead(nOffs);
		var ArrLen=oVarData.Size;		//Length of the data array
		var aVals=oVarData.Vector;			//Clear previous data
		for (var i=0;i<ArrLen;i++){
			aVals[i]=Android.mJS2And1(prot.kGetReq,parseInt(varidx),i,0);
		}
	}
	function mPokeData(oVarData)	{//Send Data to android or server
		if (!oVarData) return;  //The protocol hasnt been initialized
		if (bRelay) return;	//Not in app
		var varidx=oVarData.nIdx;		//Index of the variable for oaProtElem[varidx].mDataRead(nOffs);
		var aVals=oVarData.Vector;				//Data vector
		for (var nOffs=0;nOffs<aVals.length;nOffs++){
			if (aVals[nOffs]!==undefined)
			Android.mJS2And1(prot.kSetReq,		//Send to device
				parseInt(varidx)
				,nOffs
				,parseInt(aVals[nOffs])
			);	//Send the value ->And->BT
		}
	}
	
}
//Android interface
And.mReset=function(SerialDeviceName) {			//Contact device and reset protocol
	Android.mJS2AndCmd('Reconnect',  SerialDeviceName)
	Android.mJS2And1(prot.kCommInit,0,0,0)	
	
}
And.mStatus=function(){
	var s=Android.mJSGetStatus();
	s=s.replaceAll("'",'"');			//Correct singlequotes to double for JSON.parsing
	var state=JSON.parse(s);			//Now get the keys by Object.keys(And.prot)
	return state
}
And.FetchProtocol=function(){	//Get the protocol from device
	var s=Android.mJSGetProtocol();
	s=s.replaceAll("'",'"');			//Correct singlequotes to double for JSON.parsing
	var prot=JSON.parse(s);			//Now get the keys by Object.keys(And.prot)
	return prot
}
And.showAndroidToast= function(toast) {//Call a method showToast() in Apps java  
	Android.mAndReload();
	Android.showToast(toast);
}
And.mAnd2JS=function(cmd){		//JS Request data from Android to 
	//In JAVA:Use myWebView.evaluateJavascript("And.mAnd2JS;",null);//Send data to javascript
	popup.mMessage(cmd);
	prot.mGetStatus();
}
And.showAndroidToast= function(toast) {//Call a method showToast() in Apps java  
	Android.showToast(toast);
} 
//		SERVER INTERFACE	
var mPHPCall = function(url,cmd,data,callback) {
	return   new Promise((resolve, reject) => {
		var xhr = new XMLHttpRequest();			//Stuff using the str
		var sending={}
		if (prot.sFileName) data.sFileName=prot.sFileName;	//Override default filename
		sending.cmd=cmd;
		sending.data=data;
		var sending=JSON.stringify(sending);
	//	s=encodeURIComponent(s);
		xhr.open('POST', url, true);
		xhr.timeout = 4000;			//See https://stackoverflow.com/questions/23725085/failed-to-load-resource-neterr-network-io-suspended
		xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded; charset=ISO-8859-1');	
		xhr.onreadystatechange = function(){
			if(this.readyState==4)
			if(this.status == 200 ){
				try{
					var s= this.responseText;
					try{
						oRetData=JSON.parse(s);		//All good
					}
					catch(er){	//Cant parse the params;
						oRetData={};
						oRetData.errors=s;
					}
				}catch(e){
					oRetData={};
					oRetData.errors=e;
					reject(e);			//What do we do with reject?
				}
				if(callback) callback(oRetData);		//Report the result
			} else {
				oRetData={};
				oRetData.errors="No connection"+this.statusText+this.responseText;
				if(callback) callback(oRetData);
			}
		}
		xhr.send("data=" + sending);
		})
}