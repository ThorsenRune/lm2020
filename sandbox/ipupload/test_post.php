<?php
  $filename="lastip.txt"; //Temporary file for saving the ip of the last connected device
  $message_received = "";
    if ($_SERVER["REQUEST_METHOD"] == "POST"){
        $message_received = $_POST["message_sent"];
        echo "ESP32 sent : \n" . $message_received;
        file_put_contents($filename,  $message_received);

    }
    else {
        echo file_get_contents($filename);
    }
?>
