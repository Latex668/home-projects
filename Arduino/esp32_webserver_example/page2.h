const char* site2 = R"=====(
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
        <header>
        <h1 style="text-align: left;">Test site, nothing else.</h1>
        <ul class="navbar">
            <li><button onclick="redirectPage(0)">Index</button></li>
            <li><button onclick="redirectPage(1)">Site 2</button></li>
        </ul>

        </header>
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