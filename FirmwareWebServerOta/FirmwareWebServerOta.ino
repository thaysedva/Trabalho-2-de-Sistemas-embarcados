#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include "PaginaWEB.h"

#include <NTPClient.h>
#include <WiFiUdp.h>

const String VERSION = "0.1.2";
String TIME="NULL"; 
const char* ssid = "GABRIEL";
const char* password = "99514334";

const char *servidorNTP = "a.st1.ntp.br"; // Servidor NTP para pesquisar a hora
const int fusoHorario = -10800;           // Fuso horário em segundos (-03h = -10800 seg)
const int taxaDeAtualizacao = 1800000;    // Taxa de atualização do servidor NTP em milisegundos
WiFiUDP ntpUDP; // Declaração do Protocolo UDP
NTPClient timeClient(ntpUDP, servidorNTP, fusoHorario, 60000);

bool ledState = 0;
const char ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");



void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
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
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  if(var == "time"){
    String a = timeClient.getFormattedTime();
    return a;
  }
  if(var == "VERSION"){
    return VERSION;
  }
}

void setup(){
  ArduinoOTA.onStart([]() {Serial.println("Start");});
  ArduinoOTA.onEnd([]() {Serial.println("\nEnd");});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi.. 2");
  }
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.update();
  Serial.print("Horario atualizado: ");
  Serial.println(timeClient.getFormattedTime());
  initWebSocket();

  // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.begin();
}

void loop() {
  ArduinoOTA.handle();
  TIME = timeClient.getFormattedTime();
  ws.cleanupClients(); // testar um atraso para a chamada
  digitalWrite(ledPin, ledState);
}
