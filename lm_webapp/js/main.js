//main.js
'use strict';	//Require variable declarations
document.addEventListener("DOMContentLoaded", Main_Init);
/*		STATEMACHINE FOR ASYNC CALLS in the Mainloop



	note: it has been chosen not to use the async/promise facilities in javascript
			to keep coding style with c


	States of protocol	'prot.State'
	Then 	only if serial.isReady()
		|	prot.kCommInit	:	prot.State=prot.kCommInit	, do handshake and initialize protocol
		|	prot.kReady		:	prot.State= prot.kReady running

*/

/***********************	INITIALIZATION		***********************************/
function  Main_Init(){
	//Change the default filename for dataexchange
	prot.sFileName=mDataFile();		//Get the settings file e.g. data.txt
	display.init()
	display.idVarName1.onchange=mDDSetVarName;
	mWebSocket_InitAsync();			//Setup the websocket
	init_SetupFromServer(dataurl);	//Get protocol and widget setup from a file
	initAndSetupTheSliders();
	signal.init();
	display.redraw();					//Redraw the display
	prot.state(prot.kCommInit);		//Start the statemachine for initializing communciation
	Main_Loop();					//Goto the main requesting data from device and polling answers periodically
}

/***********************	MAIN PROCESSING		***********************************/
function mDataFile(newfile){
	var datafile=location.hash.replace('#','').replaceAll('.txt','');
	if (newfile){
		datafile=newfile.replaceAll('.txt','');;
	}else{
		if (datafile.length<3) datafile='data';
	}
	datafile=datafile.replaceAll('.txt','');
	location.hash=datafile
	return datafile+'.txt';
}


function Main_Loop(){		//This is the refresh loop of the program
	//Called peridoically
	if (Main_Loop.running) return;
	Main_Loop.running=true
	if (display.doRedraw) display.redraw();
	display.refresh()		;//Update screen widgets and get userinput
	 //Commuication requesting data from device (FSM)
	prot.TXDispatch();	//Exchange RX/TX of data from the protocol
	while (prot.mRXDispatch(serial.RXFiFo));	//Empty the queue from device
	if (oWatch.nPeriod>60)
		setTimeout(function(){ 
			Main_Loop()
		},oWatch.nPeriod); 	//Loop peridoically
	//Though setTimeout is not precise it allows easily to change the period
	Main_Loop.running=false
}
Main_Loop.running=false;







var init_SetupFromServer=function(dataurl){ //
		//Start by loading the protocol from server
	mPHPCall(dataurl,'load',oWatch,function(){
		if (oRetData.data){
			oWatch=oRetData.data;
		} else {
			alert ("File not found" );//oRetData.errors
			return;
		}
		oWatch.cmd=prot.kCommInit;
	});
}


function mWatchDog(settimeout){		//Getter/setter for a watchdog.
	if (settimeout) {
		mWatchDog.timeout=false
		mWatchDog.waiting=true
		setTimeout(function(){	//Wait until the protocol has setteled
			mWatchDog.timeout=true
			mWatchDog.waiting=false
		},settimeout)
	}
}
mWatchDog.waiting=false;		//Initial state of the wathcdog
