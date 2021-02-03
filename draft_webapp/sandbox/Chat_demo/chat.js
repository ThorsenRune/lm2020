/* Development of collaborative platform 
Template Created by: Kenrick Beckett   Name: Chat Engine
*/
 
/******INTERFACE********************************************/
	function mDataRcvd(txt){
		for (var i = 0; i < txt.length; i++) {
			var obj=JSON.parse(txt[i])
			var container=document.getElementById('idTextArea');
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
		sendChat(s, user)
	}
	var mCreateAppend=function(container,elementtype,text){
		
		var e = document.createElement(elementtype);   // Create the element
		e.innerHTML = text ;                   // Insert text
		container.appendChild(e);               // Append as last element in container
	}

/***************************************************/

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

function Chat () {
    this.update = updateChat;
    this.send = mDataSend;
	this.getState = getStateOfChat;
}

//gets the state of the chat
function getStateOfChat(){
	if(!instanse){
		 instanse = true;
		 $.ajax({
			   type: "POST",
			   url: "process.php",
			   data: {  
			   			'function': 'getState',
						'file': file
						},
			   dataType: "json",
			   success: function(data){
				   state = data.state;
				   instanse = false;
			   },
			});
	}	 
}

//Updates the chat
function updateChat(){
	 if(!instanse){
		 instanse = true;
	     $.ajax({
			   type: "POST",
			   url: "process.php",
			   data: {  
			   			'function': 'update',
						'state': state,
						'file': file
						},
			   dataType: "json",
			   success: function(data){
				   if(data.text){
					   mDataRcvd(data.text);
				   }
				   document.getElementById('idTextArea').scrollTop = document.getElementById('idTextArea').scrollHeight;
				   instanse = false;
				   state = data.state;
			   },
			});
	 }
	 else {
		 setTimeout(updateChat, 1500);
	 }
}

//send the message
function sendChat(message, nickname)
{       
    updateChat();
     $.ajax({
		   type: "POST",
		   url: "process.php",
		   data: {  
		   			'function': 'send',
					'message': message,
					'nickname': nickname,
					'file': file
				 },
		   dataType: "json",
		   success: function(data){
			   updateChat();
		   },
		});
}



