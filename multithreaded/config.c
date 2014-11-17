#include <stdio.h>

#define FILENAME  ".config"

// Produce debug information
#define DEBUG	  	1

void get_file() {
  FILE *fp;
  char line[80];
  printf("HERE");
  fp = fopen(FILENAME, "r");
  if(fp != NULL) {
    #if DEBUG
    printf("Reading the configuration file...");
    #endif
    while(fgets(line, 80, fp) != NULL) {
      printf("%s\n", line);
    }
  }
  else {
    #if DEBUG
    printf("Created default configuration file");
    #endif
    fprintf(fp, "Hello");
  }
}
