/*********
N3EPICO
*********/
// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Servo.h>

// Replace with your network credentials
const char* ssid     = "N3E Robotics";
const char* password = "Inferno69.";
  
  
int LightSensorValue = 0;
unsigned long LightSensorThreshhold = 0;

int ReflectSensorValue = 0;
unsigned long ReflectSensorThreshhold = 0;

int Function = 0; 
String sliderValue = "0";

const char* PARAM_INPUT = "value";
const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

//RC

Servo Throttle;

int16_t speed = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void calibration()
{
  speed = 0;
  Throttle.write(speed);
  delay(2000);
  Serial.println("max speed");
  speed = 2000;
  Throttle.write(speed);
  delay(4000); 

  Serial.println("break speed");
  speed = 1000;
  Throttle.write(speed);
  delay(5000);
  
  Serial.println("idle speed");
  speed = 1300;
  Throttle.write(speed);
  delay(4000);
    Serial.println("exit");
  return;
}


void launch(){
  digitalWrite(4,LOW);
  digitalWrite(0,HIGH);
  delay(2000);
  int i = 0;
  Serial.println("Reading ambient light");
  while(i<20){
  LightSensorThreshhold = LightSensorThreshhold + analogRead(A0);
  i++;
  delay(10);
  }
  LightSensorThreshhold = LightSensorThreshhold/20;
  Serial.println(LightSensorThreshhold);
  
  digitalWrite(0,LOW);
  digitalWrite(4,HIGH);
  delay(2000);
  i = 0;
  Serial.println("Reading floor's reflective light");
  while(i<20){
  ReflectSensorThreshhold = ReflectSensorThreshhold + analogRead(A0);
  i++;
  delay(10);
  }
 ReflectSensorThreshhold = ReflectSensorThreshhold/20;
 Serial.println(ReflectSensorThreshhold); 
 
 digitalWrite(4,LOW);
 digitalWrite(0,HIGH);
 int Debounce_value = 8;
 Serial.println("Waiting for Trigger");
 while(1){
  LightSensorValue = analogRead(A0);
  if(LightSensorValue >= (LightSensorThreshhold + Debounce_value)) break;
  delay(1);
 }
 Serial.println(LightSensorValue);
 Serial.println("LAUNCH");
  /*for(int i = 0; i <= 100; i++){
    speed = map(i, 0, 100, 1300, 2000);
    Serial.println(i);
    Throttle.write(speed);
    delay(100);   
  }*/
  speed = map(10, 0, 100, 1300, 2000);
  Serial.println(20);
  Throttle.write(speed);
  delay(100);
  speed = map(100, 0, 100, 1300, 2000);
  Serial.println(100);
  Throttle.write(speed);

  digitalWrite(0,LOW);
  digitalWrite(4,HIGH);
  Debounce_value = 200;
  Serial.println("Waiting for Finish Line");
  while(1){
    ReflectSensorValue = analogRead(A0);
    if(ReflectSensorValue <= (ReflectSensorThreshhold - Debounce_value)) break;
    delay(1);
  }
  Serial.println(ReflectSensorValue);
  Serial.println("BREAK");
  
  speed = 1300;
  Throttle.write(speed);
  digitalWrite(0,LOW);
  digitalWrite(4,LOW);
  Serial.println("Launch terminado");
  return;
}

void breaking()
{
  Serial.println("Breaking");
  speed = 1300;
  Throttle.write(speed);
  Serial.println("Breaking termidado");
  return;
}
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>

  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">

  <style>
    html {font-family: Cambria, Cochin, Georgia, Times, 'Times New Roman', serif; display: inline-block; text-align: center; background-color: #fffff2;}
    h2 {font-size: 3.0rem; color: #000000;}
    p {font-size: 3.0rem; color: #000000;}
    body {max-width: 1000px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    .slider2 { -webkit-appearance: none; margin: 14px; width: 600px; height: 25px; background: linear-gradient(90deg, #1aa305, #d6c61a, #b30000);
      outline: none; -webkit-transition: .2s; transition: opacity .2s; border-radius: 15px;}
    .slider2::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 15px; height: 40px; background: #000000; cursor: pointer; border-radius: 20px;}
    .slider2::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }
  </style>

</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="100" value="%SLIDERVALUE%" step="1" class="slider2"></p>
  <p><span id="textSliderValue">%SLIDERVALUE%</span>%</p>

<script>

function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}

function updateSliderPWM(element) {
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+sliderValue, true);
  xhr.send();
}

</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Calibration</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Launch</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Break</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"0\" " + outputState(0) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  
  if (var == "SLIDERVALUE"){
    return sliderValue;
  }
  return String();
}

String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  //read do butões 
  pinMode(10, INPUT);
  pinMode(9, INPUT);
  
  //Controlo dos sensores.
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); //  Controlo do Light Sensor
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW); //  Controlo do Light Sensor
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW); // Controlo do Reflect Sensor
  
  Throttle.attach(5); 
  delay(500);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("\nWiFi connected.");
  Serial.println("\nIP address: ");
  Serial.println(WiFi.localIP());
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      if(inputMessage1.toInt() == 2 && inputMessage2.toInt() == HIGH){
        Function = 1;
      }else if(inputMessage1.toInt() == 4 && inputMessage2.toInt() == HIGH){
        Function = 2;
      }else if(inputMessage1.toInt() == 0 && inputMessage2.toInt() == HIGH){
        Function = 3;
      }else Function = 0;
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    /*Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);*/
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
      speed = map(sliderValue.toInt(), 0, 100, 1300, 2000);
      Throttle.write(speed);
    }
    else {
      inputMessage = "No message sent";
    }
    //Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  server.begin();
}

void loop() {
  if(Function == 1 /*|| digitalRead(10)*/){
    calibration();
    Function == 0;
  }else if(Function == 2 /*|| digitalRead(9)*/){
    launch();
    Function == 0;
  }else if(Function == 3){
    breaking();
    Function == 0;
  }
}
