<html>
<head>
	<meta charset="utf-8">
	<meta name="viewport" 	content="width=device-width, initial-scale=1">
  <title>WEBSOCKET Client</title>
  <link rel="stylesheet" href="style.css" type="text/css" />
  <script type="text/javascript" src="chat.js"></script>

</head>

<body onload="mInit()">
    <span id="page-wrap">
        <h2>Communications</h2> <p id="idName"></p>
        <div id="chat-wrap">
					<div id="idTextArea"></div>
				</div>
				<p>Your message: </p>
				<textarea id="sendie"  ></textarea>
        <form id="send-message-area">
        </form>
    </span>
		<div>
			<hr>
			<button onclick='mStartServer()'>Start Server</button>
			<button onclick='idTextArea.innerHTML=""'>clear</button>
			<button onclick='mCloseSocket())'>stop</button>
		</div>
</body>

<script>
		function mStartServer(){
			window.open("http://localhost/Downloads/ws/php-socket.php","_blank");
		}
	/*
			CLIENT Script that requires that server.php has been called.

//Code by: Nabi KAZ <www.nabi.ir>
var port=5006
var host ='ws://localhost:'+port+'/public/lm2020/draft_webapp/sandbox/Chat_demo/server.php';
let  socket = new WebSocket(host);// Open the socket
  socket.onopen = function(event) {
			mDataRcvd("Connected")
			socket.send("Message to send");
  		// Send an initial message
      // Listen for messages
		};
    socket.onmessage = function(event) {
          mDataRcvd('< ' + event.data);
    };
  	// Listen for socket closes
    socket.onclose = function(event) {
          mDataRcvd('Client notified socket has closed:'+JSON.stringify(event));
    };

      // To close the socket....
      //socket.close()

*/
	//setInterval('chat.update()', 1000)


</script>

</html>

<?php
  ini_set('display_errors', 1);
  ini_set('display_startup_errors', 1);       //error messaging Todo6: remove before release
  header("Access-Control-Allow-Origin: *");		//Enabling CORS
	/*
		This is the CLIENT (browser page)
		prototyping a socket replacement for data.php.
		ref: https://www.binarytides.com/php-socket-programming-tutorial/
		$sock = socket_create(AF_INET, SOCK_STREAM, 0);
				Address Family : AF_INET (this is IP version 4)
				Type : SOCK_STREAM (this means connection oriented TCP protocol)
						:todo: use SOCK_DGRAM
				Protocol : 0 [ or IPPROTO_IP This is IP protocol]

			If any of the socket functions fail then the error information can be retrieved using the socket_last_error and socket_strerror functions.




	*/
	/*
	$port = 25003	;
	$domain = '127.0.0.1';
	$ip_address = gethostbyname("localhost");

	$host    = $ip_address;
	$message = "Hello Server";
	echo "Message To server :".$message;
	// create socket
	$socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
	// connect to server
	$result = socket_connect($socket, $host, $port) or die("Could not connect to server\n");
	// send string to server
	socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");
	// get server response
	$result = socket_read ($socket, 1024) or die("Could not read server response\n");
	echo "Reply From Server  :".$result;
	// close socket
	socket_close($socket);
	/*

	echo ("Connecting to ".$ip_address);

	if (($sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
				echo 'socket_creat() 失败原因 Shībài yuányīn'.socket_strerror(socket_last_error()) . "\n";
				die('\nProblem creating socket\n');
		} else {
			echo '<p>socket created  成功 Chénggōng';
		}

	if(!socket_connect($sock , $ip_address , 80))
	{
		$errorcode = socket_last_error();
	    $errormsg = socket_strerror($errorcode);
	    die("Could not connect: [$errorcode] $errormsg \n");
	}

 	echo "<p>\nConnection established \n";


	$message = "GET / HTTP/1.1\r\n\r\n";

	//Send the message to the server
	if( ! socket_send ( $sock , $message , strlen($message) , 0))
	{
		$errorcode = socket_last_error();
	    $errormsg = socket_strerror($errorcode);

	    die("Could not send data: [$errorcode] $errormsg \n");
	}

	echo "Message send successfully \n";

	//Now receive reply from server
	if(socket_recv ( $sock , $buf , 2045 , MSG_WAITALL ) === FALSE)
	{
		$errorcode = socket_last_error();
	    $errormsg = socket_strerror($errorcode);

	    die("Could not receive data: [$errorcode] $errormsg \n");
	}

	//print the received message
	echo $buf;
*/

?>
