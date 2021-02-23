<?php
$connect = 'tcp://0.0.0.0:8952';
$socket = new Socket($connect);
$socket->start();

class MyEvent
{
    protected $eventBase;
    protected $allEvents = [];

    public function __construct()
    {
        if (!extension_loaded('event')) {
            echo 'event extension is require' . PHP_EOL;
            exit(250);
        }
        $this->eventBase = new \EventBase();
    }

    public function add($fd, $flag, $func, $args = array())
    {
        $fd_key = (int)$fd;
        $event = new \Event($this->eventBase, $fd, $flag | \Event::PERSIST, $func, $fd);
        if (!$event || !$event->add()) {
            return false;
        }
        $this->allEvents[$fd_key][$flag] = $event;
        return true;
    }

    public function del($fd, $flag)
    {
        $fd_key = (int)$fd;
        if (isset($this->allEvents[$fd_key][$flag])) {
            $this->allEvents[$fd_key][$flag]->del();
            unset($this->allEvents[$fd_key][$flag]);
        }
    }

    public function loop()
    {
        $this->eventBase->loop();
    }
}
class Socket
{
    const READ_BUFFER_SIZE = 65535;
    protected $mainSocket;
    protected $context;
    protected $socketName;
    protected static $connectPools = [];
    protected $sendBuffer;
    public $reusePort = false;
    protected $eventBase;
    protected $event;


    public function __construct($socketName, $context = [])
    {
        $this->socketName = $socketName;
        $this->context = stream_context_create($context);
        $this->event = new MyEvent();
    }

    public function start()
    {
        if ($this->reusePort) {
            stream_context_set_option($this->context, 'socket', 'so_reuseport', 1);
        }

        $local_socket = $this->socketName;
        $errorNo = 0;
        $errorMsg = '';
        $this->mainSocket = stream_socket_server($local_socket, $errorNo, $errorMsg, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $this->context);

        if (function_exists('socket_import_stream')) {
            $socket = socket_import_stream($this->mainSocket);
            @socket_set_option($socket, SOL_SOCKET, SO_KEEPALIVE, 1);
            @socket_set_option($socket, SOL_TCP, TCP_NODELAY, 1);
        }

        stream_set_blocking($this->mainSocket, 0);

        if (function_exists('stream_set_read_buffer')) {
            stream_set_read_buffer($this->mainSocket, 0);
        }

        $flags = \Event::READ;

        $this->event->add($this->mainSocket, $flags, [$this, 'accept']);
        $this->event->loop();
    }


    public function baseRead($socket)
    {
        $this->connect($socket);

        $buffer = fread($socket, self::READ_BUFFER_SIZE);

        if ($buffer === '' || $buffer === false) {
           $this->disconnect($socket);
            return;
        }
        if (false === self::$connectPools[(int)$socket]['handshake']) {
            self::$connectPools[(int)$socket]['handshake'] = $this->toHandshake($socket, $buffer);
        } else {
            $buffer = $this->decode($buffer);
            $this->send($socket, $buffer);
        }
    }


    public function accept($_socket)
    {
        $socket = @stream_socket_accept($_socket, 0, $remote_address);

        if (!$socket) {
            return;
        }
        stream_set_blocking($socket, 0);

        if (function_exists('stream_set_read_buffer')) {
            stream_set_read_buffer($socket, 0);
        }

        $flags = \Event::READ;
        $this->event->add($socket, $flags, [$this, 'baseRead']);
    }

    public function baseError($buffer, $error, $id)
    {<html> <head> <title>WebSocket</title> <style> html,body{font:normal 0.9em arial,helvetica;} #log {width:440px; height:200px; border:1px solid #7F9DB9; overflow:auto;} #msg {width:330px;} </style> <script>
var socket; function init(){ var host = "ws://127.0.0.1:1222";
try{ socket = new WebSocket(host); log('WebSocket - status '+socket.readyState); socket.onopen = function(msg){ log("Welcome - status "+this.readyState); }; socket.onmessage = function(msg){ log("Received: "+msg.data); }; socket.onclose = function(msg){ log("Disconnected - status "+this.readyState); }; } catch(ex){ log(ex); } $("msg").focus(); } function send(){ var txt,msg; txt = $("msg"); msg = txt.value; if(!msg){ alert("Message can not be empty"); return; } txt.value=""; txt.focus(); try{ socket.send(msg); log('Sent: '+msg); } catch(ex){ log(ex); } } function quit(){ log("Goodbye!"); socket.close(); socket=null; } // Utilities function $(id){ return document.getElementById(id); } function log(msg){ $("log").innerHTML+="n"+msg; } function onkey(event){ if(event.keyCode==13){ send(); } } </script> </head> <body onload="init()"> <h3>WebSocket v2.00</h3> <div id="log"></div> <input id="msg" type="textbox" onkeypress="onkey(event)"/> <button onclick="send()">Send</button> <button onclick="quit()">Quit</button> <div>Commands: hello, hi, name, age, date, time, thanks, bye</div> </body> </html>


    }

    //Package function return frame processing
    protected function frame($buffer)
    {
        $len = strlen($buffer);
        if ($len <= 125) {

            return "\x81" . chr($len) . $buffer;
        } else if ($len <= 65535) {

            return "\x81" . chr(126) . pack("n", $len) . $buffer;
        } else {

            return "\x81" . char(127) . pack("xxxxN", $len) . $buffer;
        }
    }


    //Decoding parse data frame
    protected function decode($buffer)
    {
        $masks = $data = $decoded = null;
        $len = ord($buffer[1]) & 127;

        if ($len === 126) {
            $masks = substr($buffer, 4, 4);
            $data = substr($buffer, 8);
        } else if ($len === 127) {
            $masks = substr($buffer, 10, 4);
            $data = substr($buffer, 14);
        } else {
            $masks = substr($buffer, 2, 4);
            $data = substr($buffer, 6);
        }
        for ($index = 0; $index < strlen($data); $index++) {
            $decoded .= $data[$index] ^ $masks[$index % 4];
        }
        return $decoded;
    }

    protected function toHandshake($socket, $buffer)
    {
        list($resource, $host, $origin, $key) = $this->getHeaders($buffer);
        $upgrade = "HTTP/1.1 101 Switching Protocol\r\n" .
            "Upgrade: websocket\r\n" .
            "Connection: Upgrade\r\n" .
            "Sec-WebSocket-Accept: " . $this->calcKey($key) . "\r\n\r\n";  //Must end with two carriage returns

        $this->send($socket, $upgrade,false);
        return true;
    }

    public function send($socket, $buffer,$frame = true)
    {
        if($frame){
            $buffer = $this->frame($buffer);
        }
        $this->sendBuffer = $buffer;
        $this->event->add($socket, \Event::WRITE, [$this, 'baseWrite']);
    }

    public function baseWrite($socket)
    {
        $len = @fwrite($socket, $this->sendBuffer, 8192);
        if ($len === strlen($this->sendBuffer)) {
            $this->event->del($socket, \Event::WRITE);
        }
    }


    protected function getHeaders($req)
    {
        $r = $h = $o = $key = null;
        if (preg_match("/GET (.*) HTTP/", $req, $match)) {
            $r = $match[1];
        }
        if (preg_match("/Host: (.*)\r\n/", $req, $match)) {
            $h = $match[1];
        }
        if (preg_match("/Origin: (.*)\r\n/", $req, $match)) {
            $o = $match[1];
        }
        if (preg_match("/Sec-WebSocket-Key: (.*)\r\n/", $req, $match)) {
            $key = $match[1];
        }
        return [$r, $h, $o, $key];
    }

    protected function calcKey($key)
    {
        //Based on websocket version 13
        $accept = base64_encode(sha1($key . '258EAFA5-E914-47DA-95CA-C5AB0DC85B11', true));
        return $accept;
    }

    protected function connect($socket)
    {
        $fd_key = (int)$socket;
        if (!isset(self::$connectPools[$fd_key])) {
            $connect['handshake'] = false;
            $connect['fd'] = $socket;
            self::$connectPools[$fd_key] = $connect;
        }
    }

    protected function disconnect($socket)
    {
        $fd_key = (int)$socket;
        if (isset(self::$connectPools[$fd_key])) {
            unset(self::$connectPools[$fd_key]);
            $this->event->del($socket, \Event::READ);
            $this->event->del($socket, \Event::WRITE);
            @fclose($socket);
        }

    }
}
?>
