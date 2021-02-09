/* Development of collaborative platform
Template Created by: Kenrick Beckett   Name: Chat Engine
*/
var idSendText;
var port=8090;

var phpdata={};		//Data to share over the websocket
phpdata.ShareData=1000;

//sudo su; kill -9 $(sudo lsof -t -i:5006)
function mInit(){
		idSendText=sendie;
		name=window.localStorage.getItem('chatusername');
    	if (!name || name === ' ')     //	   name = "Guest";	        // default name is 'Guest'
        // ask user for name with popup prompt
        var name = prompt("Enter your chat name:", name);
    	// strip tags
    	var kName = name.replace(/(<([^>]+)>)/ig,"");
        window.localStorage.setItem('chatusername',name);//Save the name
    	// display name on pageidSendText
			idName.innerHTML="You are: <span id='chatuser'>" + name + "</span>";
      idSendText.onkeydown=function(event) {
        var keyCode1 = event.which;
				if (keyCode1 == 13) {
					mWSSend(idSendText.value);					//Send
					event.preventDefault();  		          //Stop key
				}
			};
		idSendText.value="Some message";
		idPort.innerText=port;
		WSInit();
		}

var WSInit=function (){ //Initialize the websocket
	var websocket = new WebSocket("ws://localhost:"+port);
	websocket.onopen = function(event) {
		showMessage("Connection is established!");
	}
	websocket.onmessage = function(event) {
		var Data = JSON.parse(event.data);
		mWSReceive(Data);
	};
	websocket.onerror = function(event){
		showMessage(" Problem due to some Error ");
	};
	websocket.onclose = function(event){
		showMessage(" Connection Closed ");
	};
	window.websocket=websocket; //Export it
}
function mWSSend(msg){  //Send to WS
  idSendText.value=msg;   //Set the send text
  mDataRcvd('> ' + msg);  //Echo locally
	var messageJSON = {
		msgSender: sChatUser()
		,message: msg
		,ShareData:phpdata.ShareData+1
	};
	websocket.send(JSON.stringify(messageJSON));
}
function mWSReceive(Data){			//Read from WS
	if (Data.ShareData) phpdata.ShareData=Data.ShareData;
	showMessage(Data.msgSender+">"+JSON.stringify(Data));
}








//function mStartSocket(){
function mCloseSocket(){
	mDataRcvd('Closing socket');
	websocket.close()
}

function showMessage(msg){mDataRcvd(msg)}

//}
/******INTERFACE********************************************/
	function mDataRcvd(txt){
    var container=document.getElementById('idTextArea');
    if (typeof txt =='string'){   //A simple text was received
      mCreateAppend(container,'br','');
			mCreateAppend(container,'textarea',txt);
      return;

    }
		for (var i = 0; i < txt.length; i++) {
			var obj=JSON.parse(txt[i])
			mCreateAppend(container,'span',obj.head.fID);
			mCreateAppend(container,'i',obj.head.fDate);
			mCreateAppend(container,'br','');
			mCreateAppend(container,'textarea',obj.body);
			mCreateAppend(container,'br','');
		}
	}
	 var mDataSend=function(user,message){
		var obj={head:{fID:user, fDate: sDate()}
			,body:message}
		var s=JSON.stringify(obj);
		sendChat(s, user);
	}
	var mCreateAppend=function(container,elementtype,text){

		var e = document.createElement(elementtype);   // Create the element
		e.innerHTML = text ;                   // Insert text
		container.appendChild(e);               // Append as last element in container
	}

/***********UTILITY FUNCTIONS****************************************/

function sChatUser(){return chatuser.innerText}
//		UTILS



	sDate=function(timestamp){   //Return a date string
		if (timestamp)	return (new Date(timestamp)).toLocaleString('it')
		return (new Date( )).toLocaleString('it')
	}

//    CORE

var instanse = false;
var state;
var mes;
var file;
