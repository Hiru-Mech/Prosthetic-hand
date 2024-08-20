#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

//network credentials
const char* ssid = "DNS";
const char* password = "01234567";

AsyncWebServer server(80);

AsyncWebSocket ws("/ws");

Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver();

String message = "";
String sliderValue1 = "0";
String sliderValue2 = "0";
String sliderValue3 = "0";
String sliderValue4 = "0";
String sliderValue5 = "0";
String sliderValue6 = "0";

int dutyCycle1;
int dutyCycle2;
int dutyCycle3;
int dutyCycle4;
int dutyCycle5;
int dutyCycle6;

JSONVar sliderValues;

String getSliderValues(){
  sliderValues["sliderValue1"] = String(sliderValue1);
  sliderValues["sliderValue2"] = String(sliderValue2);
  sliderValues["sliderValue3"] = String(sliderValue3);
  sliderValues["sliderValue4"] = String(sliderValue4);
  sliderValues["sliderValue5"] = String(sliderValue5);
  sliderValues["sliderValue6"] = String(sliderValue6);

  String jsonString = JSON.stringify(sliderValues);
  return jsonString;
}

void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
   Serial.println("LittleFS mounted successfully");
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    if (message.indexOf("1s") >= 0) {
      sliderValue1 = message.substring(2);
      dutyCycle1 = sliderValue1.toInt();
      Serial.println(getSliderValues());
    }
    if (message.indexOf("2s") >= 0) {
      sliderValue2 = message.substring(2);
      dutyCycle2 = sliderValue2.toInt();
      Serial.println(getSliderValues());
    }    
    if (message.indexOf("3s") >= 0) {
      sliderValue3 = message.substring(2);
      dutyCycle3 = sliderValue3.toInt();
      Serial.println(getSliderValues());
    }
    if (message.indexOf("4s") >= 0) {
      sliderValue4 = message.substring(2);
      dutyCycle4 = sliderValue4.toInt();
      Serial.print(getSliderValues());
    }  
    if (message.indexOf("5s") >= 0) {
      sliderValue5 = message.substring(2);
      dutyCycle5 = sliderValue5.toInt();
      Serial.println(getSliderValues());
    }
    if (message.indexOf("6s") >= 0) {
      sliderValue6 = message.substring(2);
      dutyCycle6 = sliderValue6.toInt();
      Serial.println(getSliderValues());
    }      
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

int angleToPulse (int ang) {
  int pulse = map(ang, 0, 180, 125, 625);
  return pulse;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  board1.begin();
  board1.setPWMFreq(60);
  initFS();
  initWiFi();
  initWebSocket();
  
  
  for (int i = 0; i < 5; i++) {
    board1.setPWM(i, 0, 125);
  }
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });
  server.serveStatic("/", LittleFS, "/");
  server.begin();
} 

void loop() {

  board1.setPWM(0, 0, angleToPulse(dutyCycle1));
  board1.setPWM(1, 0, angleToPulse(dutyCycle2));
  board1.setPWM(2, 0, angleToPulse(dutyCycle3));
  board1.setPWM(3, 0, angleToPulse(dutyCycle4));
  board1.setPWM(4, 0, angleToPulse(dutyCycle5));
  
  while (dutyCycle6 > 0) {
    for (int i=0; i<5; i++) {
      board1.setPWM(i, 0, angleToPulse(dutyCycle6));
    }
  }

  ws.cleanupClients();
}