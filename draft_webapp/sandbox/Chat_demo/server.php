<?php
error_reporting(E_ALL);
set_time_limit(0);
ob_implicit_flush();

define('HOST_NAME',"localhost");
define('PORT',"8090");
$socketport=PORT;
$null = NULL;

//sudo kill -9 $(sudo lsof -t -i:8090)

$chatHandler = new ChatHandler();

$socketResource = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);// Creazione socket TCP/IP
socket_set_option($socketResource, SOL_SOCKET, SO_REUSEADDR, 1);//Should allow to reuse the port but doesnt seem to work
/*Function bind can be used to bind a socket to a particular address and port. It needs a sockaddr_in structure similar to connect function.*/
$res=socket_bind($socketResource, 0, PORT); //Open a port for the socket
if (!$res) { //If already running try to kill the process
  echo "Port $socketport is in use. Killing \n";
  exec("kill -9 $(sudo lsof -t -i:$socketport)");
  die("Retry, please");
}
//socket_getsockname($sock, $isendp_address, $socket_port);

//listen After binding a socket to a port the next thing we need to do is listen for connections. For this we need to put the socket in listening mode. Function socket_listen is used
//The second parameter of the function socket_listen is called backlog. It controls the number of incoming connections that are kept "waiting" if the program is already busy.
$max_clients=10;
socket_listen($socketResource,$max_clients);  //Listen for clients
$ShareData=0;
$socketselecttimeout=10;
$clientSocketArray = array($socketResource);  //[] of Active sockets
$running=true;
while ($running) {
   $newSocketArray = $clientSocketArray;//Make a copy as clientSocketArray may change
   @$num_changed_sockets =socket_select($newSocketArray, $null, $null, 0, $socketselecttimeout);  //Returns the number of sockets that have changed
   if ($num_changed_sockets ===false){
     onClientDisconnect(1);
     echo "\nsocket_select() failed, reason: " . socket_strerror(socket_last_error()) . "\n";
   } else {
      /* At least at one of the sockets something interesting happened */
   }
   if (in_array($socketResource, $newSocketArray)) {  //New connection
     echo "New connection:".socket_strerror(socket_last_error());
  	 $newSocket = socket_accept($socketResource);
  	 $clientSocketArray[] = $newSocket;
  	 $header = socket_read($newSocket, 1024);
  	 $chatHandler->doHandshake($header, $newSocket, HOST_NAME, PORT);
     //Get information about the client that connected
  	 socket_getpeername($newSocket, $client_ip_address, $clientport);
  	 $connectionACK = $chatHandler->newConnectionACK($client_ip_address.":$clientport");
  	 $chatHandler->send($connectionACK);
  	 $newSocketIndex = array_search($socketResource, $newSocketArray);
  	 unset($newSocketArray[$newSocketIndex]);
   }

   foreach ($newSocketArray as $newSocketArrayResource) {
     if (is_resource($newSocketArrayResource))
  	 while(socket_recv($newSocketArrayResource, $socketData, 1024, 0) >= 1){
       //Data is waiting to be read, socket_recv returns false if no data are present
  		 $socketMessage = $chatHandler->unseal($socketData);
       //unpack the received data
  		 $messageObj = json_decode($socketMessage);//null is returned if the json cannot be decoded or if the encoded data is deeper than the nesting limit.
       if ($messageObj==NULL){
         echo "\nsocketMessage $socketMessage|length:".strlen($socketMessage);
          onClientDisconnect(2); //RT:Seems to happen on disconnect
       }  else {
         if($messageObj->ShareData)  $ShareData=$messageObj->ShareData;  //conserve data
         $messageObj->ShareData=$ShareData;
         $sendmsg=$chatHandler->seal(json_encode($messageObj));
         $chatHandler->send($sendmsg);

       }
  		 break 2;
  	 }
  	 $socketData = @socket_read($newSocketArrayResource, 1024, PHP_NORMAL_READ);
  	 if ($socketData === false) {
       echo "socketData === false\n";
  		 socket_getpeername($newSocketArrayResource, $client_ip_address);
  		 $connectionACK = $chatHandler->connectionDisconnectACK($client_ip_address);
  		 $chatHandler->send($connectionACK);
  		 $newSocketIndex = array_search($newSocketArrayResource, $clientSocketArray);
  		 unset($clientSocketArray[$newSocketIndex]);
       checkClose($clientSocketArray);
  	 }
   }
   if ($clientSocketArray==null) {$running=false;}
   //if (count($clientSocketArray)<2) {$running=false;}
   sleep(1);  //pause a second
}  //End while
socket_close($socketResource);

function onClientDisconnect($reason){
  global $newSocketArrayResource,$clientSocketArray,$newSocketIndex;
  echo "\nDisconnect($reason)\n";
  socket_close($newSocketArrayResource);//RT: from example
  unset($clientSocketArray[$newSocketIndex]); //RT: close the socket
  checkClose($clientSocketArray);

}
function checkClose($clientSocketArray){
  global $running;
  if (count($clientSocketArray)<2) {
    echo "\nNo more clients. Shutting down\n";
    $running=false;
  } else {
    echo "\n Good. count:".count($clientSocketArray);
  }
}
/***********************************************************/
class ChatHandler {
	function send($message) {
		global $clientSocketArray;
		$messageLength = strlen($message);
		foreach($clientSocketArray as $clientSocket)
		{
			@socket_write($clientSocket,$message,$messageLength);
		}
		return true;
	}

	function unseal($socketData) {
		$length = ord($socketData[1]) & 127;
		if($length == 126) {
			$masks = substr($socketData, 4, 4);
			$data = substr($socketData, 8);
		}
		elseif($length == 127) {
			$masks = substr($socketData, 10, 4);
			$data = substr($socketData, 14);
		}
		else {
			$masks = substr($socketData, 2, 4);
			$data = substr($socketData, 6);
		}
		$socketData = "";
		for ($i = 0; $i < strlen($data); ++$i) {
			$socketData .= $data[$i] ^ $masks[$i%4];
		}
		return $socketData;
	}

	function seal($socketData) {
		$b1 = 0x80 | (0x1 & 0x0f);
		$length = strlen($socketData);

		if($length <= 125)
			$header = pack('CC', $b1, $length);
		elseif($length > 125 && $length < 65536)
			$header = pack('CCn', $b1, 126, $length);
		elseif($length >= 65536)
			$header = pack('CCNN', $b1, 127, $length);
		return $header.$socketData;
	}

	function doHandshake($received_header,$client_socket_resource, $host_name, $port) {
		$headers = array();
		$lines = preg_split("/\r\n/", $received_header);
		foreach($lines as $line)
		{
			$line = chop($line);
			if(preg_match('/\A(\S+): (.*)\z/', $line, $matches))
			{
				$headers[$matches[1]] = $matches[2];
			}
		}

		$secKey = $headers['Sec-WebSocket-Key'];
		$secAccept = base64_encode(pack('H*', sha1($secKey . '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')));
		$buffer  = "HTTP/1.1 101 Web Socket Protocol Handshake\r\n" .
		"Upgrade: websocket\r\n" .
		"Connection: Upgrade\r\n" .
		"WebSocket-Origin: $host_name\r\n" .
		"WebSocket-Location: ws://$host_name:$port/demo/shout.php\r\n".
		"Sec-WebSocket-Accept:$secAccept\r\n\r\n";
		socket_write($client_socket_resource,$buffer,strlen($buffer));
	}

	function newConnectionACK($ACKMsg) {
		$message = 'New client ' . $ACKMsg;
		$messageArray = array('message'=>$message,'msgSender'=>'Info:');
		$ACK = $this->seal(json_encode($messageArray));
		return $ACK;
	}

	function connectionDisconnectACK($client_ip_address) {
		$message = 'Client ' . $client_ip_address.' disconnected';
		$messageArray = array('message'=>$message,'message_type'=>'chat-connection-ack');
		$ACK = $this->seal(json_encode($messageArray));
		return $ACK;
	}

	function createChatBoxMessage($chat_user,$chat_box_message) {
		$message = $chat_user . "Data:" . $chat_box_message;
		$messageArray = array('message'=>$message,'message_type'=>'textmsg');
		$chatMessage = $this->seal(json_encode($messageArray));
		return $chatMessage;
	}
}
?>
