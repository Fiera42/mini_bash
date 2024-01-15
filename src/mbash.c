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

int main(int argc, char* argv[]) {
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

void mbash(char* p_cmd[]) {

  extern char **environ;
  pid_t pid;
  pid = fork();
  int execRes;

  switch (pid) {
    case -1 :
    perror("fork error");
    break;

    case 0 :
    execRes = execve(p_cmd[0], (const char **)p_cmd, (const char **)environ);
    if(execRes == -1) perror("exec error");
    break;
    
    default:
    wait(NULL);
    break;
  }
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