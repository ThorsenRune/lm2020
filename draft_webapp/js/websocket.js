 // This is a module of prototype.html used for websocket.ino to interface with LM from browser (client)
 // todo: rename globally to webserial.js
var oWS={			//The websocket interface class
	EditIP:function(){	//Let user insert IP manually
			var wsip=mGetIpFromLocationbar();
			wsip=prompt("Websocket IP:",wsip);
			if (wsip) location.search='ws='+wsip;
			Main_Init();
	}
}

var bWiFi_BT_State=true;	//todo:refactor name temporary
var bUseBluetooth=function(newstate){
	/*
		args: true=>use BT, false=>use WiFI, null=> return current state
		returns true if BT is used flase if WiFi
		@author:RT210128 toggle bluetoot/WiFi
	*/
	if (typeof newstate==='undefined') return bWiFi_BT_State;
	bWiFi_BT_State=newstate;
	if (bWiFi_BT_State){		//Use bluetooth
		connectviaBT()	//Start a connection todo: refactor to InitiazeBT
		//TODO:startBTnotifications should be in a callback of connectviaBT
		setTimeout(function(){
			mMessage('Waiting for pairing');
			startBTnotifications();//Start the communication
			mMessage('BT transmission started');
		},15000);
	} else {
		mWebSocket_InitAsync();			//Setup the websocket
	}
	//Todo business logic to enable disable wifi/bluetooth

};
var ws=null;	//The websocket - a serial RX/TX channel to LM

var mGetIpFromLocationbar=function(){		//This will get a WS ip from local storage or from search query
	if (location.search)
	if (location.search.split('=')[0] == "?ws"){		//A websocket IP was supplied
		localStorage.WS_IP=location.search.split('=')[1];	//Set the new IP
	}
	if (localStorage.WS_IP) return localStorage.WS_IP;
	else return "192.168.1.8";//Some default value OBS: no spaces & use .
}

var mWebSocket_InitAsync=function(callbackonconnect){		//Async
/*		the staticIP has to match the address set in websocket.ino   */
	var  staticIP=mGetIpFromLocationbar();
	var gateway="ws://"+staticIP+"/ws";
	mMessage("Connecting to:"+gateway);
	ws = new WebSocket(gateway);
	document.getElementById( "idStatus").innerHTML="Calling device";
	aRXData=[]
	ws.binaryType="arraybuffer"
	mMessage('Connecting to:'+staticIP);
	ws.onopen = function() {
		document.getElementById( "idStatus").innerHTML="CONNECTED";
		if (callbackonconnect)callbackonconnect();
	 };
	  ws.onmessage = function(evt) { //Receiving data from serial
		var data=new Uint8Array(evt.data)   //Todo: make a loop for all data
		serial.onReceive(data)
		//ws.send('hello, server');
	};
	ws.onerror = function (error) {
		document.getElementById( "idStatus").innerHTML="Failed to connect";
		mMessage('Cant connect to '+staticIP);
	};
}


//	BT_InitBluetoot - setup addEventListener BTReceiveEvent
//	*--->	BTReceiveEvent (data on BT) ->read data -> serial.onReceive(data)


function connectviaBT() {
	// initialize bluetooth and  setup an event listener
	//todo: refactor name mWebSocket_InitAsync
	document.getElementById( "idStatus").innerHTML="Connecting via BT";
	//returns data from BT as Uint8Array [1..20]
	//Todo: write what this does in a comment is this the Ternary Operator? (variable = (condition) ? expressionTrue : expressionFalse)
	return (bluetoothDeviceDetected ? Promise.resolve() : getDeviceInfo() && isWebBluetoothEnabled())
	.then(connectGATT)  //todo:@FC please explain what is happening here
	.then(_ => {
		console.log('Evaluating signal of interest...')
		return gattCharacteristic.readValue()	//receiving data from BT - Uint8Array [1..20]
	})
	.catch(error => {
		console.log('Waiting to start reading: ' + error)
	})
}

function connectGATT() {  // works like ws.onmessage
	 //When the user has paired the bluetooth this will set the isBTConnected flag
	 // CONNECT TO A GENERIC ATTRIBUTE
	 if (bluetoothDeviceDetected.gatt.connected && gattCharacteristic) {
		 return Promise.resolve()
	 }
	 return bluetoothDeviceDetected.gatt.connect()
	 .then(server => {
		 console.log('Getting GATT Service...')
		 return server.getPrimaryService(bleService)
	 })
	 .then(service => {
		 console.log('Getting GATT Characteristic...')
		 return service.getCharacteristic(bleCharacteristic)
	 })
	 .then(characteristic => {			//Similar to ws.onmessage
		 gattCharacteristic = characteristic
		 gattCharacteristic.addEventListener('characteristicvaluechanged',
				 handleChangedValue) //Like serial.onReceive
		 document.getElementById( "idStatus").innerHTML="CONNECTED";
		 alert('Bluetooth connected');
		 isBTConnected = true;
	 })

 }

  //HANDLE RECEIVED MESSAGES (STORE THEM AND CONVERT THEM)
 //Read data from BT
  function handleChangedValue(event) {
 	 let decoder = new TextDecoder('utf-8');
 	 let value = event.target.value
 	 var now = new Date()
 	 console.log('> ' + now.getHours() + ':' + now.getMinutes() + ':' + now.getSeconds() + ' Received message is: ' + decoder.decode(value) );
	  mDebugMsg1(1,'> ' + now.getHours() + ':' + now.getMinutes() + ':' + now.getSeconds() + ' Received message is: ' + decoder.decode(value) );
 	 receivedValue=value;
	 serial.onReceive(receivedValue);
 //	 MessageReceived = receivedValue;
   mDebugMsg1(1,"CONNECTED, Acquiring data");
 	 isBTConnected = true;
  }

/////////////////// WEB BT

var deviceName = 'MeCFES'  //BT name
var bleService = '783b26f8-740d-4187-9603-82281d6d7e4f' 			//UUID for the Service - generated for the LM project
var bleCharacteristic = '1bfd9f18-ae1f-4bba-9fe9-0df611340195' //UUID for the received characteristic - generated for the LM project - Receiving data
var bleCharacteristicWrite = 'bb99a060-6fa8-4bba-9ef0-731634e96e88' //UUID for the characteristic to send - generated for the LM project - Used for writing
var bluetoothDeviceDetected //Characteristics of the connected BT device
var gattCharacteristic //Generic ATTribute (GATT) characteristic
let receivedValue = "";
let isBTConnected = false;
var MessageReceived = "";

//VERIFY WEB BT COMPATIBILITY FOR THE BROWSER
//Todo: call somewhere when selecting bluetooth if (isWebBluetoothEnabled() {...}
 function isWebBluetoothEnabled() {
	 if (!navigator.bluetooth) {
		 mDebugMsg1(1,'Web Bluetooth API is not available in this browser!')
		 //
		 return false
	 }

	 return true
 }

 //ESTABLISH BT CONNECTION
 function getDeviceInfo() {
	 //Filter devices by Service UUID and Name
	 let options = {
		 optionalServices: [bleService],
		 filters: [ { "name": deviceName } ]
	 }
	 mMessage('Requesting any Bluetooth Device...')
	 return navigator.bluetooth.requestDevice(options).then(device => {
		 bluetoothDeviceDetected = device
	 }).catch(error => {
		 //Todo:
		 mMessage('Cannot connect to bluetooth'+ error.message);
		 //Todo : add type of error. handleChangedValue
	 })
 }



 //SEND A MESSAGE TO THE ESP32
 //Todo: pass data as parameter send2LMviaBT(data)
  function send2LMviaBT(value) {   //value is an arry of bytes
  //let encoder = new TextEncoder('utf-8');
  mDebugMsg1(1,'Setting Characteristic User Description...');
  gattCharacteristic.writeValue(value)
  .then(_ => {
 	 mDebugMsg1(1,'> Characteristic User Description changed to: ' + value);
  })
  .catch(error => {
		//todo9: userfriendly message
 	 mDebugMsg1(1,'Argh! ' + error);
  });


 }




//??NOW TO UPDATE DATA??//
//START NOTIFICATIONS
 function startBTnotifications() {
	 gattCharacteristic.startNotifications()
	 .then(_ => {
		 console.log('Start reading...');
		 mDebugMsg1(1,'Start reading...');
	 })
	 .catch(error => {
		 console.log('[ERROR] Start: ' + error);
		  mDebugMsg1(1,'[ERROR] Start: ' + error);

	 })
 }
//STOP NOTIFICATIONS
 function stopBTnotifications() {
	 gattCharacteristic.stopNotifications()
	 .then(_ => {
		 console.log('Stop reading...');
		  mDebugMsg1(1,'Stop reading...');
	 })
	 .catch(error => {
		 console.log('[ERROR] Stop: ' + error);
		mDebugMsg1(1,'[ERROR] Start: ' + error);
	 })
 }


//////////////////	SERIAL COMMUNIATONS
var serial={}		//Lowlevel serial communication
serial.isReady=function (){
		//Todo: return true on BT open
		return (ws.readyState==ws.OPEN);
	}
serial.RXFiFo=[];	//Consume data by data= serial.RXFiFo.shift
serial.send=function(nByte){		//Calling the websocket nByte =[1,2,3,4,] di dati
	var buffer=new ArrayBuffer(1)
	var view=new Int8Array(buffer)
	view[0]=nByte
	if(isBTConnected){
		send2LMviaBT(buffer)			//todo : if buffer.length > 20 loop split
	} else {
		ws.send(buffer)

	}
}
serial.onReceive=function(data){		//Called from the websockt
	for (var i=0;i<data.length;i++){
		serial.RXFiFo.push(data[i]);
	}
//-	if (serial.RXFiFo.length>0) mFromDevice(serial.RXFiFo)		//Testing
	//obsolete here while (prot.mRXDispatch(serial.RXFiFo));		//Distributing the received data as long as data are presen
	//return aRXData.shift(0)
}

/*
	notes:
	include in html with: <script src="websocket.js"></script>

Handles serial communication between lm and client (browser)


*/
