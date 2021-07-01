#include "./System.h"
#include "Pins.h"
#include "WifiConfig.h"
#include <ArduinoOTA.h>

void setup()
{
	//Executa os setups
	InitSerial();
	InitPinMode();
	
	//Conecta o ESP na rede
	WiFi.begin(SSID, PASSWORD);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("[ESP] Conectando a rede wifi");
	}

	Serial.println("[ESP] IP Adquirido:");
	Serial.println(WiFi.localIP());
	
	//Executa os setups
	InitOTA();
	InitNtp();
	InitWebServer();
	InitCountdown();
}

void loop()
{
	//Gerenciando o OTA
	ArduinoOTA.handle();
	
	//Limpa os clientes
	ws.cleanupClients();

	//Atualiza o ntp
	ntpClient.update();
	
	//Executa o gerenciamento das tasks
	HandleTasks();
	
	//Atualiza o led
	digitalWrite(LED1PIN, !led1State);
	digitalWrite(LED2PIN, !led2State);
	
	//Checa a flag de atualizar o horario
	if(updateTimeFlag)
	{
		NotifyTime();
		updateTimeFlag = false;
	}
	
	//Checa a flag de coletar os dados
	if(readSensorFlag)
	{
		NotifySensor(random(0, 1024));
		readSensorFlag = false;
	}
}