#include "./Pins.h"
#include "./System.h"

//Array com as tasks do sistema
TaskModel tasks[] =
{
	//Task para atualizar o horario em todos os clientes, executa a cada 1s
	{SetTimeFlag, 1000, 1000}
};

//Quantidade de tasks
const int tasksCount = (sizeof (tasks))/ sizeof (tasks[0]);

//Flag para atualizar o time dos clientes
bool updateTime = false;
void SetTimeFlag()
{
	updateTime = true;
	return;
}