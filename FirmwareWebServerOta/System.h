#ifndef System_h
#define System_h

//Estrutura de callback da task
typedef void (*Callback)();

//Estrutura padrão de uma task (função a executar, tempo máximo, tempo atual)
typedef struct
{
	Callback callback;
	const int maxCounter;
	int currentCounter;
} TaskModel;  

//Array contendo todas as tasks do programa
extern TaskModel Tasks[];
//Bit para verificar se deve executar o callback da task
extern volatile unsigned long CallTask;
//Quantidade de tasks
extern const int TasksCount;

#endif