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
	
	//Atualiza os leds
	digitalWrite(LED1PIN, !led1State);
	digitalWrite(LED2PIN, !led2State);
	
	//Checa a flag de atualizar o horario
	if(updateTimeFlag)
	{
		//Notifica o novo tempo baseado no NTP
		NotifyTime();
		
		//Reseta a flag
		updateTimeFlag = false;
	}
	
	//Checa a flag de coletar os dados
	if(readSensorFlag)
	{
		//Coleta o dado do sensor e notifica o novo valor
		int ldr = analogRead(LDRPIN);
		NotifySensor(ldr);
		
		//Reseta a flag
		readSensorFlag = false;
	}
	
	//Checa a flag para ler os botões
	if(readButtonFlag)
	{
		//Checa se o botão 1 foi pressionado
		int button1State = digitalRead(BUTTON1PIN);
		if(button1State)
		{
			//Inverte o estado do led e notifica a mudança
			led1State = !led1State;
			NotifyLed1State();
		}
		
		//Checa se o botão 2 foi pressionado
		int button2State = digitalRead(BUTTON2PIN);
		if(button2State)
		{
			//Inverte o estado do led e notifica a mudança
			led2State = !led2State;
			NotifyLed2State();
		}
		
		//Reseta a flag
		readButtonFlag = false;
	}
}