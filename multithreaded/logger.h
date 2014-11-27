#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define STRINGSIZE 10

typedef struct structmessage{
	char type[STRINGSIZE];
	char objct[STRINGSIZE];
	int nidthread;
	time_t start;
	time_t end;
} message;

extern int queuemsgid;


void statfunc (void);
void clean_stat (int signum);
void show_stat(int signum );
void init_logger(void);

