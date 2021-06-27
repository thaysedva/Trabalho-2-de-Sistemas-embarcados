#include "./System.h"
#include <ESP8266TimerInterrupt.h>

//Bit para verificar se deve executar o callback da task
volatile unsigned long callTask;
//Timer0 do esp
ESP8266Timer timer;

//A cada tick do timer subtrai 1 de cada task e atualiza o calltask
void IRAM_ATTR TimerHandler()
{
	unsigned long bitPos = 1;

	for (byte i = 0; i < tasksCount; i++)
	{
		tasks[i].currentCounter--;

		if (!tasks[i].currentCounter)
		{
			tasks[i].currentCounter = tasks[i].maxCounter;
			callTask = callTask | bitPos;
		}

		bitPos = bitPos << 1;
	}
	
	return;
}

//Função para gerenciar a chamada dos callbacks de cada task
void HandleTasks()
{
	unsigned long bitPos = 1;

	for (byte i = 0; i < tasksCount; i++)
	{
		if (callTask & bitPos)
		{
			tasks[i].callback();

			cli();
			callTask = callTask & ~bitPos;
			sei();
		}
		bitPos = bitPos << 1;
	}
	
	return;
}

//Setup do timer
void InitCountdown(void)
{
	timer.attachInterruptInterval(1000, TimerHandler);
	return;
}
