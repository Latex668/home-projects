var Socket;

function init() {
  Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  Socket.onmessage = function (event) {
    processCommand(event);
  };
}

function redirectPage(pageNum) {
  if (pageNum == 0) {
    window.location.href = 'http://' + window.location.hostname;
  } else if (pageNum == 1) {
    window.location.href = 'http://' + window.location.hostname + '/site2';
  }
}

let state = false;
function butPres(butNum) {
  state = !state;
  let msg = {
    GPIO: 21,
    GPIOState: state,
  };
  Socket.send(JSON.stringify(msg));
  console.log(msg);
}
function processCommand(event) {
  var obj = JSON.parse(event.data);
  document.getElementById('n1').innerHTML = obj.num1;
  document.getElementById('b1').innerHTML = obj.but;
}
window.onload = function (event) {
  init();
};
