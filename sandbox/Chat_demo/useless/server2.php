<?php
  ini_set('display_errors', 1);
  ini_set('display_startup_errors', 1);       //error messaging Todo6: remove before release
  error_reporting(~E_NOTICE);
  //header("Access-Control-Allow-Origin: *");		//Enabling CORS
	/*
    Experimenting with sockets
		This is the SERVER (holding the data)
	*/
  /* Turn on implicit output flushing so we see what we're getting
   * as it comes in. */
  ob_implicit_flush();

  $port = 5006;//kill:  sudo kill -9 $(sudo lsof -t -i:5006)
  // don't timeout!
  //set_time_limit(0);
  // create socket
  $ip_address = gethostbyname("localhost");
  $address=$ip_address;
  $max_clients = 3;
  $myData="Data ";
  if (false){
    $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    socket_bind($socket, $ip_address, $port);
    socket_recvfrom($socket, $buf, 12, 0, $from, $port);
    //$buf=socket_read($socket, 2048);
    echo "Received $buf from remote address $from and remote port $port" . PHP_EOL;
    $msg="Sikerult";
    socket_write($socket, $msg, strlen($msg));
    socket_sendto($socket, $msg, strlen($msg), 0, '127.0.0.1', 11104);
    socket_close($socket);
  } else if (false){
    $master  = WebSocket("localhost",$port);
    $sockets = array($master);
    $users   = array();
    $debug   = false;

    while(true){
      $changed = $sockets;
      socket_select($changed,$write=NULL,$except=NULL,NULL);
      foreach($changed as $socket){
        if($socket==$master){
          $client=socket_accept($master);
          if($client<0){ console("socket_accept() failed"); continue; }
          else{ connect($client); }
        }
        else{
          $bytes = @socket_recv($socket,$buffer,2048,0);
          if($bytes==0){ disconnect($socket); }
          else{
            $user = getuserbysocket($socket);
            if(!$user->handshake){ dohandshake($user,$buffer); }
            else{ process($user,$buffer); }
          }
        }
      }
    }
  } else if (false){    //Not working outdated example
    // Now create a socket
      $socket = socket_create_listen($port);
      $peer = socket_accept($socket);
      // Do the handshake and wait for an incoming message from the client
      handshake($peer);
      myLog('Got ' . receive($peer));
      // Respond!
      send1($peer, 'hi there');
      socket_close($peer);
      socket_close($socket);
  }
    else if (true){  //Working but not receiving well
      echo ("\nConnecting to ".$ip_address);
      if(!($sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)))  {
        $errorcode = socket_last_esendrror();
          $errormsg = socket_strerror($errorcode);
          die("Couldn't create socket: [$errorcode] $errormsg \n");
      }
      echo "Socket created \n";
      if ( ! socket_set_option($sock, SOL_SOCKET, SO_REUSEADDR, 1))
      {
          echo socket_strerror(socket_last_error($sock));
          exit;
      }  // Bind the source address
      /*Function bind can be used to bind a socket to a particular address and port. It needs a sockaddr_in structure similar to connect function.
      */
      if( !socket_bind($sock, $ip_address, $port) )  {
        //Use $port= 0 for port to bind a random (free) port for incoming connections:
        $errorcode = socket_last_error();
          $errormsg = socket_strerror($errorcode);
          //exec("sudo kill -9 $(sudo lsof -t -i:5006)");
          exec("kill -9 $(sudo lsof -t -i:$port)");
          die("\nCould not bind socket : [$errorcode] $errormsg \n");
      }
      socket_getsockname($sock, $isendp_address, $socket_port);
      $port=$socket_port;
      $server=$sock;
      echo "Socket bind OK  $socket, $socket_address, $socket_port\n";

      //listen After binding a socket to a port the next thing we need to do is listen for connections. For this we need to put the socket in listening mode. Function socket_listen is used
      socket_listen ($sock , $max_clients);
      //The second parameter of the function socket_listen is called backlog. It controls the number of incoming connections that are kept "waiting" if the program is already busy.
      $client = socket_accept($server);

      // Send WebSocket handshake headers.
      $request = socket_read($client, 5000);
      preg_match('#Sec-WebSocket-Key: (.*)\r\n#', $request, $matches);
      $key = base64_encode(pack(
          'H*',
          sha1($matches[1] . '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')
      ));
      $headers = "HTTP/1.1 101 Switching Protocols\r\n";
      $headers .= "Upgrade: websocket\r\n";
      $headers .= "Connection: Upgrade\r\n";
      $headers .= "Sec-WebSocket-Version: 13\r\n";
      $headers .= "Sec-WebSocket-Accept: $key\r\n\r\n";
      socket_write($client, $headers, strlen($headers));

      // Send messages into WebSocket in a loop.
      //socket_set_nonblock($client); not doing anything
      while (true) {
        $res= socket_recv($client,$data, 64, MSG_PEEK|MSG_DONTWAIT);
          if (0<$res) {
            $buf = socket_read($client, 64);
            echo "socket_read() $buf: n";
      //      if (false === ($buf = socket_read($client, 64, PHP_NORMAL_READ))) {
        //      echo "socket_read() $data: " . socket_strerror(socket_last_error($msgsock)) . "\n";
      //      break ;

        }
          /*
          if (!$buf = trim($buf)) {
              continue;
          }
          if ($buf == 'quit') {
              break;
          }
          if ($buf == 'shutdown') {
              socket_close($client);send
              break ;
          }
    //      $talkback = "PHP: You said '$buf'.\n";
      //    socket_write($client, $talkback, strlen($talkback));
          echo "$buf\n";
    */
          $content = 'Now: ' . time();
          //$input = trim(socket_read($client, 1024));
        //  $content =$input." ".$content ;
          $response = chr(129) . chr(strlen($content)) . $content;
          $ret=socket_write($client,$response);
          sleep(1);
          if ($ret<strlen($response)) break; //Not all bytes written error
          if(false===$ret) break;
      }
      echo "Closing socket\n";
      socket_close($sock);
    }

    // Just to log to console
    function myLog($msg)
    {
        echo date('m/d/Y H:i:s ', time()) . $msg . "\n";
    }

    // This will actually read and process the key-1 and key-2 variables, doing the math for them
    function getWebSocketKeyHash($key)
    {
        $digits = '';
        $spaces = 0;
        // Get digits
        preg_match_all('/([0-9])/', $key, $digits);
        $digits = implode('', $digits[0]);
        // Count spaces
        $spaces = preg_match_all("/\\s/ ", $key, $dummySpaces);
        $div = (int)$digits / (int)$spaces;
        myLog('key |' . $key . '|: ' . $digits . ' / ' . $spaces . ' = ' . $div);
        return (int)$div;
    }

    // This will read one header: value from the request header
    function getWebSocketHeader($buffer, &$lines, &$keys)
    {
        preg_match_all("/([a-zA-Z0-9\\-]*)(\\s)*:(\\s)*(.*)?\r\n/", $buffer, $headers);
        $lines = explode("\r\n", $buffer);
        $keys = array_combine($headers[1], $headers[4]);
     }

    // This is where the handshake gets done
    function handshake($peer)
    {
        $buffer = socket_read($peer, 4096, PHP_BINARY_READ);
        socket_getpeername($peer, $address, $port);
        $peerName = $address . ':' . $port;
        myLog('Got from: ' . $peerName . ': ' . $buffer);
        getWebSocketHeader($buffer, $lines, $keys);


        $code = array_pop($lines);
        // Process the result from both keys and form the response header
        $key = pack('N', $key1) . pack('N', $key2) . $code;
        myLog('1:|' . $key1 . '|- 2:|' . $key2 . '|3:|' . $code . '|4: ' . $key);
        $response = "HTTP/1.1 101 WebSocket Protocol Handshake\r\n";
        $response .= "Upgrade: WebSocket\r\n";
        $response .= "Connection: Upgrade\r\n";
        $response .= "Sec-WebSocket-Origin: " . trim($keys['Origin']) . "\r\n";
        $response .= "Sec-WebSocket-Location: ws://" . trim($keys['Host']) . "/\r\n";
        $response .= "\r\n" . md5($key, true); // this is the actual response including the hash of the result of processing both keys
        myLog($response);
        socket_write($peer, $response);
    }

    // This is where you can send a frame (delimited by 0x00 and 0xFF)
    function send1($peer, $message)
    {
        socket_write($peer, pack('c', (int)0) . utf8_encode($message) . pack('c', (int)255));
    }

    // This is where you receive a frame (delimited again by 0x00 and 0xFF)
    function receive($peer)
    {
        $buffer = socket_read($peer, 4096, PHP_BINARY_READ);
        if (empty($buffer)) {
            myLog('Error receiving from peer');
            return;
        }
        return substr($buffer, 1, -1);
    }
    //---------------------------------------------------------------
    function process($user,$msg){
      $action = unwrap($msg);
      say("< ".$action);
      switch($action){
        case "hello" : send($user->socket,"hello human");                       break;
        case "hi"    : send($user->socket,"zup human");                         break;
        case "name"  : send($user->socket,"my name is Multivac, silly I know"); break;
        case "age"   : send($user->socket,"I am older than time itself");       break;
        case "date"  : send($user->socket,"today is ".date("Y.m.d"));           break;
        case "time"  : send($user->socket,"server time is ".date("H:i:s"));     break;
        case "thanks": send($user->socket,"you're welcome");                    break;
        case "bye"   : send($user->socket,"bye");                               break;
        default      : send($user->socket,$action." not understood");           break;
      }
    }

    function send($client,$msg){
      say("> ".$msg);
      $msg = wrap($msg);
      socket_write($client,$msg,strlen($msg));
    }

    function WebSocket($address,$port){
      $master=socket_create(AF_INET, SOCK_STREAM, SOL_TCP)     or die("socket_create() failed");
      socket_set_option($master, SOL_SOCKET, SO_REUSEADDR, 1)  or die("socket_option() failed");
      socket_bind($master, $address, $port)                    or die("socket_bind() failed");
      socket_listen($master,20)                                or die("socket_listen() failed");
      echo "Server Started : ".date('Y-m-d H:i:s')."\n";
      echo "Master socket  : ".$master."\n";
      echo "Listening on   : ".$address." port ".$port."\n\n";
      return $master;
    }

    function connect($socket){
      global $sockets,$users;
      $user = new User();
      $user->id = uniqid();
      $user->socket = $socket;
      array_push($users,$user);
      array_push($sockets,$socket);
      console($socket." CONNECTED!");
    }

    function disconnect($socket){
      global $sockets,$users;
      $found=null;
      $n=count($users);
      for($i=0;$i<$n;$i++){
        if($users[$i]->socket==$socket){ $found=$i; break; }
      }
      if(!is_null($found)){ array_splice($users,$found,1); }
      $index = array_search($socket,$sockets);
      socket_close($socket);
      console($socket." DISCONNECTED!");
      if($index>=0){ array_splice($sockets,$index,1); }
    }

    function dohandshake($user,$buffer){
      console("\nRequesting handshake...");
      console($buffer);
      list($resource,$host,$origin,$strkey1,$strkey2,$data) = getheaders($buffer);
      console("Handshaking...");

      $pattern = '/[^\d]*/';
      $replacement = '';
      $numkey1 = preg_replace($pattern, $replacement, $strkey1);
      $numkey2 = preg_replace($pattern, $replacement, $strkey2);

      $pattern = '/[^ ]*/';
      $replacement = '';
      $spaces1 = strlen(preg_replace($pattern, $replacement, $strkey1));
      $spaces2 = strlen(preg_replace($pattern, $replacement, $strkey2));

      if ($spaces1 == 0 || $spaces2 == 0 || $numkey1 % $spaces1 != 0 || $numkey2 % $spaces2 != 0) {
    	socket_close($user->socket);
    	console('failed');
    	return false;
      }

      $ctx = hash_init('md5');
      hash_update($ctx, pack("N", $numkey1/$spaces1));
      hash_update($ctx, pack("N", $numkey2/$spaces2));
      hash_update($ctx, $data);
      $hash_data = hash_final($ctx,true);

      $upgrade  = "HTTP/1.1 101 WebSocket Protocol Handshake\r\n" .
                  "Upgrade: WebSocket\r\n" .
                  "Connection: Upgrade\r\n" .
                  "Sec-WebSocket-Origin: " . $origin . "\r\n" .
                  "Sec-WebSocket-Location: ws://" . $host . $resource . "\r\n" .
                  "\r\n" .
                  $hash_data;

      socket_write($user->socket,$upgrade.chr(0),strlen($upgrade.chr(0)));
      $user->handshake=true;
      console($upgrade);
      console("Done handshaking...");
      return true;
    }

    function getheaders($req){
      $r=$h=$o=null;
      if(preg_match("/GET (.*) HTTP/"   ,$req,$match)){ $r=$match[1]; }
      if(preg_match("/Host: (.*)\r\n/"  ,$req,$match)){ $h=$match[1]; }
      if(preg_match("/Origin: (.*)\r\n/",$req,$match)){ $o=$match[1]; }
      if(preg_match("/Sec-WebSocket-Key2: (.*)\r\n/",$req,$match)){ $key2=$match[1]; }
      if(preg_match("/Sec-WebSocket-Key1: (.*)\r\n/",$req,$match)){ $key1=$match[1]; }
      if(preg_match("/\r\n(.*?)\$/",$req,$match)){ $data=$match[1]; }
      return array($r,$h,$o,$key1,$key2,$data);
    }

    function getuserbysocket($socket){
      global $users;
      $found=null;
      foreach($users as $user){
        if($user->socket==$socket){ $found=$user; break; }
      }
      return $found;
    }

    function     say($msg=""){ echo $msg."\n"; }
    function    wrap($msg=""){ return chr(0).$msg.chr(255); }
    function  unwrap($msg=""){ return substr($msg,1,strlen($msg)-2); }
    function console($msg=""){ global $debug; if($debug){ echo $msg."\n"; } }

    class User{
      var $id;
      var $socket;
      var $handshake;
    }
?>
