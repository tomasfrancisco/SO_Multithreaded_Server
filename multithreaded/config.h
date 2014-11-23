#define N_SCRIPTS 3
#define true 1
#define false 0
#define DELAY 5

typedef struct shared_mem_model {
  int port;
  int num_threads;
  char scheduler[100];
  char scripts[100];
} config;

void init_config();
void cleanup_config();
void config_process(const char* path);
void wait_for_config();
int get_port();
int get_num_threads();
void get_scheduler(char *scheduler);
void get_scripts(char *scripts);
