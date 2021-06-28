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
extern TaskModel tasks[];
//Bit para verificar se deve executar o callback da task
extern volatile unsigned long callTask;
//Quantidade de tasks
extern const int tasksCount;

//Flag para atualizar o horario
extern bool updateTimeFlag;
//Flag para coletar os dados do sensor
extern bool readSensorFlag;

#endif