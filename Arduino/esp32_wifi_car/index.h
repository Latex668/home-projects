const char* homepage = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>ESP32 WiFi Car</title>
  </head>
  <style>
    body {
      zoom: 3;
      background-color: #1f1f1f;
      color: #ffd900;
    }
    button {
      background-color: #212121;
      padding: 15px 15px;
      border: solid;
      border-radius: 100vw;
      color: gold;
      text-decoration: none;
      font-size: xx-large;
    }
    button:hover {
      background-color: gold;
      color: black;
    }

    .buttons {
      position: relative;
      top: 2vw;
    }
  </style>
  <body>
    <h1>Car controls:</h1>
    <div class="buttons" style="text-align: center; width: 480px">
      <button onclick="motorControl(0)" onmouseup="motorControl(5)">UP</button
      ><br />
      <button onclick="motorControl(1)" onmouseup="motorControl(5)">LEFT</button
      ><button onclick="motorControl(3)" onmouseup="motorControl(5)">
        RIGHT</button
      ><br />
      <button onclick="motorControl(2)" onmouseup="motorControl(5)">
        DOWN
      </button>
    </div>
  </body>
  <script>
    var Socket;

    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function (event) {
        processCommand(event);
      };
    }

    function motorControl(dir) {
      let msg = {
        num: dir,
      };
      Socket.send(JSON.stringify(msg));
    }

    function processCommand(event) {
      var obj = JSON.parse(event.data);
      document.getElementById('n1').innerHTML = obj.num1;
      document.getElementById('b1').innerHTML = obj.but;
    }
    window.onload = function (event) {
      init();
    };
  </script>
</html>

)=====";