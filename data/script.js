// Complete project details: https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
}

function getValues(){
    websocket.send("getValues");
}

function saveForm(){
//var formdata = $('form').serialize();

const msg = {
    type: "message",
    ssid1: document.getElementById("ssid1").value,
    pass1: document.getElementById("pass1").value,
    ssid2: document.getElementById("ssid2").value,
    pass2: document.getElementById("pass2").value,
    secret_key : document.getElementById("secret_key").value,
    webhook_url:document.getElementById("webhook_url").value,
  };

  // Send the msg object as a JSON-formatted string.
  websocket.send(JSON.stringify(msg));


//websocket.send(formdata);
}



function restart(){
  websocket.send("restart");
}


function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getValues();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function updateSliderPWM(element) {
    var sliderNumber = element.id.charAt(element.id.length-1);
    var sliderValue = document.getElementById(element.id).value;
    document.getElementById("sliderValue"+sliderNumber).innerHTML = sliderValue;
    console.log(sliderValue);
    websocket.send(sliderNumber+"s"+sliderValue.toString());
}

function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        
        if  (document.getElementById(key) ){
          document.getElementById(key).innerHTML = myObj[key];
        //document.getElementById("slider"+ (i+1).toString()).value = myObj[key];
        
        }
    }
}
