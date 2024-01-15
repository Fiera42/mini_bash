#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>


#define MAXLI 2048
char path[MAXLI];
int pathidx;

void mbash();
char** parseCmd();

int main(int argc, char** argv) {
  char cmd[MAXLI];

  while (1) {
    printf("Commande: ");
    fgets(cmd, MAXLI, stdin);

    char** p_cmd = parseCmd(cmd);

    mbash(p_cmd);

    for (int i = 0; p_cmd[i] != NULL; i++) {
      free(p_cmd[i]);
    }
    free(p_cmd);
  }
  return 0;
}

void mbash(char** p_cmd) {
  printf("Execute: %s ", p_cmd[0]);

  int p_cmd_size = 0;
  while (p_cmd[p_cmd_size] != NULL) {
    p_cmd_size++;
  }

  printf(" size : %i", p_cmd_size);
  printf("\n");

  /*
  
  if(p_cmd_size > 1) {
    for(int i = 1; i < p_cmd_size; i++) {
      printf("arg %i : %s", i, p_cmd[i]);
    }
  }
  */

  //system(p_cmd[0]);
}

char** parseCmd(char* cmd) {
  //Allocation pour le nouveau tableau
  char** p_cmd = (char**)malloc(MAXLI * sizeof(char*));
  for (int i = 0; i < MAXLI; i++) {
    p_cmd[i] = (char*)malloc(MAXLI * sizeof(char));
  }

  //Parse
  char* token = strtok(cmd, " \t\n");

  //Ecriture
  int i = 0;
  while (token != NULL) {
    strcpy(p_cmd[i], token);
    i++;
    token = strtok(NULL, " \t\n");
  }
  p_cmd[i] = NULL;

  return p_cmd;
}