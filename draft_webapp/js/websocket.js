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
		alert('Cant connect to '+staticIP);
	};
}


//	BT_InitBluetoot - setup addEventListener BTReceiveEvent
//	*--->	BTReceiveEvent (data on BT) ->read data -> serial.onReceive(data)


// initialize bluetooth and  setup an event listener
//todo: refactor name mWebSocket_InitAsync
function readBT() {
	document.getElementById( "idStatus").innerHTML="Connecting via BT";
	//returns data from BT as Uint8Array [1..20]
	//Todo: write what this does in a comment is this the Ternary Operator? (variable = (condition) ? expressionTrue : expressionFalse)
	return (bluetoothDeviceDetected ? Promise.resolve() : getDeviceInfo() && isWebBluetoothEnabled())
	.then(connectGATT)
	.then(_ => {
		console.log('Evaluating signal of interest...')
		return gattCharacteristic.readValue()	//receiving data from BT - Uint8Array [1..20]
	})
	.catch(error => {
		console.log('Waiting to start reading: ' + error)
	})
}
// CONNECT TO A GENERIC ATTRIBUTE
 function connectGATT() {  // ws.onmessage
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
		 isConnected = true;
	 })

 }

  //HANDLE RECEIVED MESSAGES (STORE THEM AND CONVERT THEM)
 //Read data from BT
  function handleChangedValue(event) {
 	 let decoder = new TextDecoder('utf-8');
 	 let value = event.target.value
 	 var now = new Date()
 	 console.log('> ' + now.getHours() + ':' + now.getMinutes() + ':' + now.getSeconds() + ' Received message is: ' + decoder.decode(value) )
 	 receivedValue=decoder.decode(value);
 //	 MessageReceived = receivedValue;
   document.getElementById( "idStatus").innerHTML="CONNECTED, Acquiring data";
 	 isConnected = true;
  }

/////////////////// WEB BT

var deviceName = 'MeCFES'  //BT name
var bleService = '783b26f8-740d-4187-9603-82281d6d7e4f' 			//UUID for the Service - generated for the LM project
var bleCharacteristic = '1bfd9f18-ae1f-4bba-9fe9-0df611340195' //UUID for the received characteristic - generated for the LM project - Receiving data
var bleCharacteristicWrite = 'bb99a060-6fa8-4bba-9ef0-731634e96e88' //UUID for the characteristic to send - generated for the LM project - Used for writing
var bluetoothDeviceDetected //Characteristics of the connected BT device
var gattCharacteristic //Generic ATTribute (GATT) characteristic
let receivedValue = "";
let isConnected = false;
var MessageReceived = "";

//VERIFY WEB BT COMPATIBILITY FOR THE BROWSER
//Todo: call somewhere when selecting bluetooth if (isWebBluetoothEnabled() {...}
 function isWebBluetoothEnabled() {
	 if (!navigator.bluetooth) {
		 console.log('Web Bluetooth API is not available in this browser!')
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
	 console.log('Requesting any Bluetooth Device...')
	 return navigator.bluetooth.requestDevice(options).then(device => {
		 bluetoothDeviceDetected = device
	 }).catch(error => {
		 //Todo:
		 mAlert('Cannot connect to bluetooth'+ error.message);
		 //Todo : add type of error. handleChangedValue
	 })
 }



 //SEND A MESSAGE TO THE ESP32
 //Todo: pass data as parameter writeBT(data)
  function writeBT() {
  const name = input.value();
  let encoder = new TextEncoder('utf-8');
  log('Setting Characteristic User Description...');
  gattCharacteristic.writeValue(encoder.encode(name))
  .then(_ => {
 	 log('> Characteristic User Description changed ttodo1o: ' + name);
  })
  .catch(error => {
		//todo9: userfriendly message
 	 log('Argh! ' + error);
  });


 }




//??NOW TO UPDATE DATA??//
//START NOTIFICATIONS
 function startBT() {
	 gattCharacteristic.startNotifications()
	 .then(_ => {
		 console.log('Start reading...')
	 })
	 .catch(error => {
		 console.log('[ERROR] Start: ' + error)
	 })
 }
//STOP NOTIFICATIONS
 function stopBT() {
	 gattCharacteristic.stopNotifications()
	 .then(_ => {
		 console.log('Stop reading...')
	 })
	 .catch(error => {
		 console.log('[ERROR] Stop: ' + error)
	 })
 }


//////////////////	SERIAL COMMUNIATONS
var serial={}		//Lowlevel serial communication
serial.isReady=function (){
		return (ws.readyState==ws.OPEN);
	}
serial.RXFiFo=[];	//Consume data by data= serial.RXFiFo.shift
serial.send=function(nByte){		//Calling the websocket
	var buffer=new ArrayBuffer(1)
	var view=new Int8Array(buffer)
	view[0]=nByte
	ws.send(buffer)
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
