#include "./System.h"
#include <ESP8266TimerInterrupt.h>

//Bit para verificar se deve executar o callback da task
volatile unsigned long CallTask;
//Timer0 do esp
ESP8266Timer Timer;

//A cada tick do timer subtrai 1 de cada task e atualiza o calltask
void IRAM_ATTR TimerHandler()
{
	unsigned long bitPos = 1;

	for (byte i = 0; i < TasksCount; i++)
	{
		Tasks[i].currentCounter--;

		if (!Tasks[i].currentCounter)
		{
			Tasks[i].currentCounter = Tasks[i].maxCounter;
			CallTask = CallTask | bitPos;
		}

		bitPos = bitPos << 1;
	}
	
	return;
}

//Função para gerenciar a chamada dos callbacks de cada task
void HandleTasks()
{
	unsigned long bitPos = 1;

	for (byte i = 0; i < TasksCount; i++)
	{
		if (CallTask & bitPos)
		{
			Tasks[i].callback();

			cli();
			CallTask = CallTask & ~bitPos;
			sei();
		}
		bitPos = bitPos << 1;
	}
	
	return;
}

//Setup do timer
void InitCountdown(void)
{
	Timer.attachInterruptInterval(1000, TimerHandler);
	return;
}