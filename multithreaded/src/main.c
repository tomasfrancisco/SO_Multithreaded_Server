/*
 * Multithreaded HTTP Server
 * Sistemas Operativos 2014/2015
 * Joel Oliveira Vasco // Tomás Francisco
 */

#include "main.h"
#include "buffer.h"

// Produce debug information
#define DEBUG	  	1

void catch_ctrlc(int);
void catch_sighup(int sig);

pid_t config_id, receiver_id;
int status;

int main(int argc, char ** argv)
{
	signal(SIGINT,catch_ctrlc);
	signal(SIGHUP, catch_sighup);

	init_config();
	if((config_id = fork()) == 0) {
		config_process("../.config");
	}
	else {
		wait_for_config();
	}

	if((receiver_id = fork()) == 0) {
		receiver_process(get_port(), get_num_threads() * 2);
	}

	waitpid(receiver_id, &status, WUNTRACED);
}

// Shutdown every child process
void catch_ctrlc(int sig)
{
	printf("Server terminating\n");
	kill(receiver_id, SIGINT);
	kill(config_id, SIGINT);
	exit(0);
}

// Update configuration information
void catch_sighup(int sig) {
	printf("Updating information\n");
}
