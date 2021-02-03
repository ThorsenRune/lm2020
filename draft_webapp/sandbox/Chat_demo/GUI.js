//GUI.js   <-  index.html
//	API for index
		name=window.localStorage.getItem('chatusername');
    	if (!name || name === ' ')     	   name = "Guest";	        // default name is 'Guest'
        // ask user for name with popup prompt    
        var name = prompt("Enter your chat name:", name);
    	// strip tags
    	var kName = name.replace(/(<([^>]+)>)/ig,"");
        window.localStorage.setItem('chatusername',name);//Save the name
    	// display name on page
		$( document ).ready(function() {
			mInit();
			window.maxLength = $(this).attr("maxlength");  
			window.maxLength=10000;
		});
		function mInit(){
			$("#idName").html("You are: <span>" + name + "</span>");
 		}
    	// kick off chat
        var chat =  new Chat();			//Create the chat object update, send & getstate
    	$(function() {
			chat.getState();     		// watch textarea for key presses
            $("#sendie").keydown(function(event) {  
                var keyCode1 = event.which;  
				if (keyCode1 == 13) {
					mSend(this);					//Send
					event.preventDefault();  		//Stop key
				}
			});
		});
		function mSend(that){
			var text = $(that).val();
			var length = text.length; 
			chat.send(kName,text);				// send 
			$(that).val("");
		}
    