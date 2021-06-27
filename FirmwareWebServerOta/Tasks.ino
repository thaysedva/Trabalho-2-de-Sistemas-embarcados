#include "./Pins.h"
#include "./System.h"

//Array com as tasks do sistema
TaskModel tasks[] =
{
	//Task1 executa a cada 500ms
	{task_0, 500, 500}
};

//Quantidade de tasks
const int tasksCount = (sizeof (tasks))/ sizeof (tasks[0]);

//Teste de task1
void task_0()
{
  return;
}