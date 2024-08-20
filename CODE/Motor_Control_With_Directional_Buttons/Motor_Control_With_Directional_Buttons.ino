// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"

// Replace with your network credentials
const char* ssid = "DNS";
const char* password = "01234567";

bool upState = 0;
bool downState = 0;
bool leftState = 0;
bool rightState = 0;

//define motor pins
const int enA = 14; // Enable pin for motor A
const int in1 = 26; // Input 1 for motor A
const int in2 = 25; // Input 2 for motor A
const int enB = 27; // Enable pin for motor B
const int in3 = 33; // Input 1 for motor B
const int in4 = 32; // Input 2 for motor B

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients() {
  ws.textAll(String(upState));
  ws.textAll(String(downState));
  ws.textAll(String(leftState));
  ws.textAll(String(rightState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggleUp") == 0) {
      upState = !upState;
      notifyClients();
    } else if (strcmp((char*)data, "toggleLeft") == 0) {
      leftState = !leftState;
      notifyClients();
    } else if (strcmp((char*)data, "toggleRight") == 0) {
      rightState = !rightState;
      notifyClients();
    } else if (strcmp((char*)data, "toggleDown") == 0) {
      downState = !downState;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
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

String processor(const String& var){
  Serial.println(var);
  if(var == "up"){
      return upState ? "ON" : "OFF";
  } else if (var == "left"){
      return leftState ? "ON" : "OFF";
  } else if(var == "right"){
      return rightState ? "ON" : "OFF";
  } else if(var == "down"){
      return downState ? "ON" : "OFF";
  }
    return String();
}

void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
   Serial.println("LittleFS mounted successfully");
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  //set dc motor pins as output
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Initialize LittleFS
  initFS();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });
  
  server.serveStatic("/", LittleFS, "/");

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();

  if (upState == 1) { //forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(enA, 255);
    analogWrite(enB, 255);
  } else if (downState == 1) { //backward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, 255);
    analogWrite(enB, 255);
  } else if (leftState == 1) { //left
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(enA, 255);
    analogWrite(enB, 255);
  } else if (rightState == 1) { //right
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, 255);
    analogWrite(enB, 255);
  } else{ //stop motors
    analogWrite(enA, 0);
    analogWrite(enB, 0);
  }
}