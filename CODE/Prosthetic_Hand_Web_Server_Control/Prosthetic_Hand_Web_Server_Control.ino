#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>

//network credentials
const char* ssid = "DNS";
const char* password = "01234567";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object

AsyncWebSocket ws("/ws");

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

// setting PWM properties
const int freq = 50;
const int resolution = 16;

//Define servo motor pins
#define SERVO_PIN1 16
#define SERVO_PIN2 17
#define SERVO_PIN3 18
#define SERVO_PIN4 19
#define SERVO_PIN5 21

uint32_t duty1;
uint32_t duty2;
uint32_t duty3;
uint32_t duty4;
uint32_t duty5;
uint32_t duty6;

//Json Variable to Hold Slider Values
JSONVar sliderValues;

//Get Slider Values
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

// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
   Serial.println("LittleFS mounted successfully");
  }
}

// Initialize WiFi
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

void notifyClients(String sliderValues) {
  ws.textAll(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    if (message.indexOf("1s") >= 0) {
      sliderValue1 = message.substring(2);
      dutyCycle1 = map(sliderValue1.toInt(), 0, 180, 0, 180);
      Serial.println(dutyCycle1);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("2s") >= 0) {
      sliderValue2 = message.substring(2);
      dutyCycle2 = map(sliderValue2.toInt(), 0, 180, 0, 180);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }    
    if (message.indexOf("3s") >= 0) {
      sliderValue3 = message.substring(2);
      dutyCycle3 = map(sliderValue3.toInt(), 0, 180, 0, 180);
      Serial.println(dutyCycle3);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("4s") >= 0) {
      sliderValue4 = message.substring(2);
      dutyCycle4 = map(sliderValue4.toInt(), 0, 180, 0, 180);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }  
    if (message.indexOf("5s") >= 0) {
      sliderValue5 = message.substring(2);
      dutyCycle5 = map(sliderValue5.toInt(), 0, 180, 0, 180);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("6s") >= 0) {
      sliderValue6 = message.substring(2);
      dutyCycle6 = map(sliderValue6.toInt(), 0, 180, 0, 180);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }      
    if (strcmp((char*)data, "getValues") == 0) {
      notifyClients(getSliderValues());
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

void setup() {
  Serial.begin(115200);

  //set servo pins as output
  pinMode(SERVO_PIN1, OUTPUT);
  pinMode(SERVO_PIN2, OUTPUT);
  pinMode(SERVO_PIN3, OUTPUT);
  pinMode(SERVO_PIN4, OUTPUT);
  pinMode(SERVO_PIN5, OUTPUT);

  initFS();
  initWiFi();

  // Set up servo pins - pwm
  ledcAttach(SERVO_PIN1, freq, resolution);
  ledcAttach(SERVO_PIN2, freq, resolution);
  ledcAttach(SERVO_PIN3, freq, resolution);
  ledcAttach(SERVO_PIN4, freq, resolution);
  ledcAttach(SERVO_PIN5, freq, resolution);

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

  // convert 0-180 degrees to 0-65536
  duty1 = (((dutyCycle1/180.0)*2000)/20000.0*65536.0) + 1634;
  duty2 = (((dutyCycle2/180.0)*2000)/20000.0*65536.0) + 1634;
  duty3 = (((dutyCycle3/180.0)*2000)/20000.0*65536.0) + 1634;
  duty4 = (((dutyCycle4/180.0)*2000)/20000.0*65536.0) + 1634;
  duty5 = (((dutyCycle5/180.0)*2000)/20000.0*65536.0) + 1634;

  while (dutyCycle6 != 0) {
    duty6 = (((dutyCycle6/180.0)*2000)/20000.0*65536.0) + 1634;
    ledcWrite(SERVO_PIN1, duty6);
    ledcWrite(SERVO_PIN2, duty6);
    ledcWrite(SERVO_PIN3, duty6);
    ledcWrite(SERVO_PIN4, duty6);
    ledcWrite(SERVO_PIN5, duty6);
  }
  
  ledcWrite(SERVO_PIN1, duty1);
  ledcWrite(SERVO_PIN2, duty2);
  ledcWrite(SERVO_PIN3, duty3);
  ledcWrite(SERVO_PIN4, duty4);
  ledcWrite(SERVO_PIN5, duty5);

  ws.cleanupClients();
}