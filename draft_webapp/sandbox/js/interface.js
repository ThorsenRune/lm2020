"use strict";
var oVal={};			//Object of values
var bRelay= (typeof Android =='undefined') ;//Android is an object injected from the  Webview when using the slider.
var bAndroid=!bRelay;		//We are in app
var dataurl='data.php'	;
//var dataurl='data.php'	;

// PROTOCOL
/*

*/

//----------------------   SERVER
var oRetData={};	//Create a permanent copy

//	****************************	SERVER INTERFACE	***************
var mPHPCall = function(url,cmd,data,callback) {
	//cmd ={save,load,swap}
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
