/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-web-server-multiple-pages
 */

const char *HTML_CONTENT_HOME = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <title>Example code!</title>
    </head>
    <style>
        body{
            background-color: #1f1f1f;
            color:#003366;
        }
        span{
            color:#003366;
        }
    </style>
    <body>
        <h1>Random number generator</h1>
        <p>Random number is: <span id='n1'></span></p>
        <script>
            function fetchNumber(){
                fetch("/Number")
                .then(response=>response.text())
                .then(data=>{
                    document.getElementById("n1").textContent = data;
                });
            }
            fetchNumber();
            setInterval(fetchNumber,4000);
        </script>
    </body>
</html>
)=====";
