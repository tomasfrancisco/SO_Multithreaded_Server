/*
 *  INSTRUÇÕES DE UTILIZAÇÃO DA BIBLIOTECA CONFIG.C
 *
 *
 *  - init_config() - Esta função deve ser sempre usada antes de
 *  qualquer utilização desta biblioteca, é ela que torna a biblioteca
 *  utilizável, inicializando a memória partilhada, e os semáforos
 *  necessários para a sincronização.
 *
 *  - config_process(const char *path) - Esta função é no fundo o processo
 *  responsável por toda a biblioteca, actualizando a memória partilhada
 *  sempre que existem alterações no ficheiro de configurações.
 *
 *  - wait_for_config() - Esta função deve ser usada pelo processo principal,
 *  logo a seguir ao lançamento do processo de configuração.
 *
 *  $ A partir da chamada da função wait_for_config() todo o acesso à
 *  $ memória partilhada deve ser feito através das funções get_*, sempre que o
 *  $ o processo principal receba um sinal do tipo SIGHUP (deve criar um handler
 *  $ do lado do processo principal).
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include "config.h"
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#define FILENAME  ".config"

// Produce debug information
#define DEBUG	  	1

//For tests
int num_threads = 5;
const char* scripts[N_SCRIPTS + 1] = {"3", "test1.sh", "test2.sh", "test3.sh"};

//Configuration
config* shm_config;
int shm_config_id;
sem_t* mutex;
sem_t* ready;

time_t last_time = 0;

FILE* get_file_to_read(const char* path);
void write_file_to_shm(FILE *fp);
int file_modified(const char* path);
void end_config_process(int sig);

/****Gets & Sets****/

void set_port(int port) {
  sem_wait(mutex);
  shm_config->port = port;
  sem_post(mutex);
}

int get_port() {
  int port;

  sem_wait(mutex);
  port = shm_config->port;
  sem_post(mutex);

  return port;
}

void set_num_threads(int threads) {
  sem_wait(mutex);
  shm_config->num_threads = threads;
  sem_post(mutex);
}

int get_num_threads() {
  int num_threads;

  sem_wait(mutex);
  num_threads = shm_config->num_threads;
  sem_post(mutex);

  return num_threads;
}

void set_scheduler(char scheduler[]) {
  sem_wait(mutex);
  sprintf(shm_config->scheduler, "%s", scheduler);
  sem_post(mutex);
}

void get_scheduler(char *scheduler) {
  sem_wait(mutex);
  sprintf(scheduler, "%s", shm_config->scheduler);
  sem_post(mutex);
}

void set_scripts(char scripts[]) {
  sem_wait(mutex);
  sprintf(shm_config->scripts, "%s", scripts);
  sem_post(mutex);
}

void get_scripts(char *scripts) {
  sem_wait(mutex);
  sprintf(scripts, "%s", shm_config->scripts);
  sem_post(mutex);
}

/****Gets & Sets****/

/*
 * Responsável por correr o processo de gestão de configuração
 * e avisar o processo principal de que o ficheiro de config
 * foi modificado, e escrito na memória partilhada
 */
void config_process(const char* path) {
  FILE *fp;

  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, 1);
  sigdelset(&mask, 2);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  signal(SIGINT, end_config_process);

  fp = get_file_to_read(path);
  write_file_to_shm(fp);
  fclose(fp);
  sem_post(ready);

  while(true) {
    if(file_modified(path)) {
      printf("Modified\n");
      fp = get_file_to_read(path);
      write_file_to_shm(fp);
      fclose(fp);
      kill(getppid(), SIGHUP);
    }
    sleep(DELAY);
  }
}

/*
 * Responsável por abrir o ficheiro de configuração existente,
 * ou criar o ficheiro por defeito no caso de não existir e
 * retornar o ponteiro em modo de leitura
 */
FILE* get_file_to_read(const char* path) {
  struct stat file_stat;
  FILE* fp;
  int i;

  fp = fopen(path, "r");
  if(fp != NULL) {
    #if DEBUG
    printf("Reading the configuration file...\n");
    #endif
  }
  else {
    #if DEBUG
    printf("Creating default configuration file...\n");
    #endif
    fp = fopen(path, "w");
    fprintf(fp, "%d\n", num_threads);
    for(i = 1; i <= atoi(scripts[0]); i++) {
      fprintf(fp, "%s\n", scripts[i]);
    }
    fclose(fp);
  }

  fp = fopen(path, "r");

  stat(path, &file_stat);
  last_time = file_stat.st_mtime;

  return fp;
}

/*
 * Escreve o ficheiro para a memória partilhada
 */
void write_file_to_shm(FILE *fp) {
  char line[100];
  char *ptr;
  #if DEBUG
  printf("Writing file to shared memory...\n");
  #endif

  while(fgets(line, 100, fp) != NULL) {

    ptr = strtok(line, " ;");
    if(ptr != NULL) {
      if(!strcmp(ptr, "#port:")) {
        if((ptr = strtok(NULL, " ;")) != NULL) {
          printf("(R) #port: %d\n", atoi(ptr));
          set_port(atoi(ptr));
        }
        else {
          printf("(R) #port: NULL");
        }
      }
      else if(!strcmp(ptr, "#threads:")) {
        if((ptr = strtok(NULL, " ;")) != NULL) {
          printf("(R) #threads: %s\n", ptr);
          set_num_threads(atoi(ptr));
        }
        else {
          printf("(R) #threads: NULL");
        }
      }
      else if(!strcmp(ptr, "#scheduler:")) {
        if((ptr = strtok(NULL, " ;")) != NULL) {
          printf("(R) #scheduler: %s\n", ptr);
          set_scheduler(ptr);
        }
        else {
          printf("(R) #scheduler: NULL");
        }
      }
      else if(!strcmp(ptr, "#scripts:")) {
        if((ptr = strtok(NULL, " ;")) != NULL) {
          printf("(R) #scripts: %s\n", ptr);
          set_scripts(ptr);
        }
        else {
          printf("(R) #scripts: NULL");
        }
      }
      else {
        printf("Error parsing file\n");
      }
    }
  }
}


/*
 * Verifica o estado do ficheiro, retorna 1 se modificado
 * 0 se não. Faz uso da variável global last_time que se
 * inicia a 0
 */
int file_modified(const char* path) {
  struct stat file_stat;
  int state;

  int err = stat(path, &file_stat);
  if(err != 0) {
    perror("[file_modified] stat");
    exit(errno);
  }
  state = file_stat.st_mtime > last_time;
  last_time = file_stat.st_mtime;

  return state;
}

/*
 * Inicializa as estruturas
 */
void init_config() {
  #if DEBUG
    printf("Creating shared memory to configuration manager process\n");
  #endif
  shm_config_id = shmget(IPC_PRIVATE, sizeof(config), IPC_CREAT|0700);
  shm_config = (config*) shmat(shm_config_id, NULL, 0);
  #if DEBUG
    printf("Creating shared memory semaphore\n");
  #endif
  sem_unlink("MUTEX");
  mutex = sem_open("MUTEX", O_CREAT|O_EXCL, 0700, 1);
  sem_unlink("READY");
  ready = sem_open("READY", O_CREAT|O_EXCL, 0700, 0);
}

/*
 * Aguarda pela primeira configuração disponibilizada
 * para arrancar com o servidor
 */
void wait_for_config() {
  sem_wait(ready);
  kill(getpid(), SIGHUP);
}

/*
 * Liberta todas as estruturas utilizadas no processo
 */
void cleanup_config() {
  #if DEBUG
    printf("Closing everything\n");
  #endif
  sem_close(mutex);
  shmdt(shm_config);
  shmctl(shm_config_id, IPC_RMID, NULL);
}

/*
 * Termina o processo de gestão de configuração em segurança
 * e eficiência
 */
void end_config_process(int sig)
{
  printf("Configuration manager terminating\n");
  exit(0);
}
