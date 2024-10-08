
const char* homepage = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <title>Example code!</title>
    </head>
    <style>
        body{
            background-color: #1f1f1f;
            color:#ffd900;
        }
        .but1{
            border:none;
            color:#ffd900;
            padding:5px 5px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            cursor: pointer;
            background-color: #151515;
        }
    </style>
    <body>
        <h1>Random number generator</h1>
        <ul class="navbar">
            <li><button onclick="redirectPage(0)">Index</button></li>
            <li><button onclick="redirectPage(1)">Site 2</button></li>
        </ul>
        <p>Random number is: <span id='n1'>%number%</span></p>
        <p>Button1 state is: <span id="b1">%value%</span></p>
        <button class="but1" onclick="butPres(1)">Turn on LED</button>

    </body>
    <script>
        var Socket;

        function init(){
            Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
            Socket.onmessage = function(event){
            processCommand(event);
         };
        }

        function redirectPage(pageNum){
            if(pageNum == 0){
                window.location.href ="http://" + window.location.hostname;
            }else if(pageNum == 1){
                window.location.href ="http://" + window.location.hostname + "/site2";
            }
        }

        let state = false;
        function butPres(butNum){
            state = !state;
            let msg = {
                GPIO: 21,
                GPIOState: state
            };
            Socket.send(JSON.stringify(msg))
            console.log(msg);
        }
        function processCommand(event){
            var obj = JSON.parse(event.data);
            document.getElementById('n1').innerHTML = obj.num1;
            document.getElementById("b1").innerHTML = obj.but;
        }
        window.onload = function(event){
            init();
        }
     </script>
</html>
)=====";