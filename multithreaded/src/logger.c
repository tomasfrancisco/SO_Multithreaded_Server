#include <stdio.h>

#include "logger.h"
#define FILENAME  ".stat"
// GLOBAL ACCESS
//LOCAL ACCESS
FILE *fp;
time_t servstart;
int totstat = 0;
int totdyna = 0;
int totreq = 0;
int queuemsgid;


void statfunc (void )
{

	// Ignore all signals

	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, 1);
	sigdelset(&mask, 2);
	sigprocmask(SIG_SETMASK, &mask, NULL);
	

	localtime(&servstart);


	fp = fopen(FILENAME, "w");
	if(fp == NULL)
	{
		printf("Error opening file in logger\n");
		exit(1);
	}
	message objrecived;
	printf("Child pid %d \n", getpid());
	signal(SIGHUP, show_stat);
	signal(SIGINT, clean_stat);
	while(1)
	{
		msgrcv(queuemsgid, &objrecived, sizeof(message), 0, 0);
		printf("%s;%s;%d;%lld;%lld\n", objrecived.type, objrecived.objct,  objrecived.nidthread, (long long ) objrecived.start, (long long) objrecived.end);
		fprintf(fp,"%s;%s;%d;%lld;%lld\n", objrecived.type, objrecived.objct,  objrecived.nidthread, (long long ) objrecived.start, (long long) objrecived.end);
		if (! strncmp(objrecived.type,"cgi-bin/",strlen("cgi-bin/")))
			totdyna++;
		else
			totreq++;
		totreq++;
	}

}

void clean_stat(int signum)
{
	fclose(fp);
	exit(0);
}

void show_stat(int signum )
{
	time_t now;
	localtime(&now);
	printf("Hora de Arranque: %lld\n", (long long ) servstart);
	printf("Hora Actual: %lld\n", (long long ) now);
	printf("Total de acessos estáticos: %d\n", totstat);
	printf("Total de acessos dinâmicos: %d\n", totdyna);
	printf("Total de acessos: %d\n", totreq);
}

void init_logger(void)
{
	//CREATE QUEUE
	queuemsgid = msgget(IPC_PRIVATE, IPC_CREAT|0700);
	if( (queuemsgid)!= 0 )
	{
		assert( queuemsgid);
		//printf("Message queue id %d\n", queuemsgid);
	}
	else
	{
		printf("Erro a criar a queue Message\n");
		exit(1);
	}
	// END QUEUE

}
