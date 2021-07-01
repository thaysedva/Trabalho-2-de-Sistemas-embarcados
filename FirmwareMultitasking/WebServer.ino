#include "./Pins.h"
#include "./System.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

String Processor(const String& var)
{
	if(var == "LED1")
		return led1State ? "ON" : "OFF";
	else if(var == "LED2")
		return led2State ? "ON" : "OFF";
	else if(var == "TIME")
		return ntpClient.getFormattedTime().c_str();
	else if(var == "VERSION")
		return VERSION;
	else if(var == "RSSI")
		return String(WiFi.RSSI());
	else if(var == "IP")
		return WiFi.localIP().toString();
	else if(var == "MAC")
		return WiFi.macAddress().c_str();
}

//Eventos do web socket
void OnEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
	switch (type)
	{
		case WS_EVT_CONNECT:
			Serial.printf("[WebSocket] Cliente #%u conectado %s\n", client->id(), client->remoteIP().toString().c_str());
			//Sempre que um cliente é conectado notifica o estado pra ele
			NotifyLed1State();
			NotifyLed2State();
			break;
		case WS_EVT_DISCONNECT:
			Serial.printf("[WebSocket] Cliente #%u desconectado\n", client->id());
			break;
		case WS_EVT_DATA:
			HandleWebSocketMessage(arg, data, len);
			break;
		case WS_EVT_PONG:
			break;
		case WS_EVT_ERROR:
			break;
	}
}

//Gerencia os comandos recebidos pelo web socket
void HandleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
	AwsFrameInfo *info = (AwsFrameInfo*)arg;
	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
	{
		data[len] = 0;
		//Se o comando for toggle, muda o estado do led e notifica todos os clientes
		if (strcmp((char*)data, "toggleLed1") == 0)
		{
			led1State = !led1State;
			NotifyLed1State();
		}
		else if (strcmp((char*)data, "toggleLed2") == 0)
		{
			led2State = !led2State;
			NotifyLed2State();
		}
	}
}

//Envia para todos os clientes o estado do led1
void NotifyLed1State()
{
	ws.textAll("{\"led1\":\"" + String(led1State) + "\"}");
}

//Envia para todos os clientes o estado do led2
void NotifyLed2State()
{
	ws.textAll("{\"led2\":\"" + String(led2State) + "\"}");
}

//Notifica alterações no horario
void NotifyTime()
{
	ws.textAll("{\"time\":\"" + String(ntpClient.getFormattedTime().c_str()) + "\"}");
}

//Notifica alterações do sensor
void NotifySensor(int value)
{
	ws.textAll("{\"sensor\":\"" + String(value) + "\"}");
}