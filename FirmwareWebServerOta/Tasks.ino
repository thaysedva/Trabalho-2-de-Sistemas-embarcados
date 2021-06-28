#include "./Pins.h"
#include "./System.h"

//Array com as tasks do sistema
TaskModel tasks[] =
{
	//Task para atualizar o horario em todos os clientes, executa a cada 1s
	{SetTimeFlag, 1000, 1000},
	//Task para coletar os dados do sensor, executa a cada 500ms
	{ReadSensorData, 500, 500}
};

//Quantidade de tasks
const int tasksCount = (sizeof (tasks))/ sizeof (tasks[0]);

//Flag para atualizar o time dos clientes
bool updateTimeFlag = false;
void SetTimeFlag()
{
	updateTimeFlag = true;
	return;
}

//Coleta os dados do sensor
bool readSensorFlag = false;
void ReadSensorData()
{
	readSensorFlag = true;
	return;
}