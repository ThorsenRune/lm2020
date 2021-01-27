  var deviceName = 'MeCFES'
  var bleService = '783b26f8-740d-4187-9603-82281d6d7e4f'
  var bleCharacteristic = '1bfd9f18-ae1f-4bba-9fe9-0df611340195'
  var bleCharacteristicWrite = 'bb99a060-6fa8-4bba-9ef0-731634e96e88'
  var bluetoothDeviceDetected
  var gattCharacteristic
  let receivedValue = "";
  let splitString;
  let isConnected = false;


  function isWebBluetoothEnabled() {
    if (!navigator.bluetooth) {
      console.log('Web Bluetooth API is not available in this browser!')
      return false
    }
    return true
  }


  function getDeviceInfo() {
    let options = {
      optionalServices: [bleService],
      filters: [
        { "name": deviceName }
      ]
    }
    console.log('Requesting any Bluetooth Device...')
    return navigator.bluetooth.requestDevice(options).then(device => {
      bluetoothDeviceDetected = device
    }).catch(error => {
      console.log('Argh! ' + error)
    })
  }


  function readBT() {
    return (bluetoothDeviceDetected ? Promise.resolve() : getDeviceInfo())
    .then(connectGATT)
    .then(_ => {
      console.log('Reading data...')
      return gattCharacteristic.readValue()
    })
    .catch(error => {
      console.log('Waiting to start reading: ' + error)
    })
  }

  function connectGATT() {
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
    .then(characteristic => {
      gattCharacteristic = characteristic
      gattCharacteristic.addEventListener('characteristicvaluechanged',
          handleChangedValue)
//        var value=event.target.value.getUint16(0);
//        $("#Notified Value").text(""+value);
      isConnected = true;

    })
  }


 function handleChangedValue(event) {
    let decoder = new TextDecoder('utf-8');
    let value = event.target.value
    var now = new Date()
    console.log('> ' + now.getHours() + ':' + now.getMinutes() + ':' + now.getSeconds() + ' Received message is: ' + decoder.decode(value) )
    receivedValue=decoder.decode(value);
    MessageReceived = receivedValue;
    isConnected = true;
  }

  function startBT() {
    gattCharacteristic.startNotifications()
    .then(_ => {
      console.log('Start reading...')
    })
    .catch(error => {
      console.log('[ERROR] Start: ' + error)
    })
  }

  function stopBT() {
    gattCharacteristic.stopNotifications()
    .then(_ => {
      console.log('Stop reading...')
    })
    .catch(error => {
      console.log('[ERROR] Stop: ' + error)
    })
  }

//Define the operation to send the message to the ESP32
    function writeButtonBT() {
  const name = input.value();
  let encoder = new TextEncoder('utf-8');
  log('Setting Characteristic User Description...');
  gattCharacteristic.writeValue(encoder.encode(name))
  .then(_ => {
    log('> Characteristic User Description changed to: ' + name);
  })
  .catch(error => {
    log('Argh! ' + error);
  });
}
