#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdbool.h>  


#define MAXLI 2048
char path[MAXLI];
int pathidx;

void mbash();
char** parseCmd();

int main(int argc, char* argv[]) {
  char cmd[MAXLI];
  bool are_we_continuing = true;

  while (are_we_continuing) {
    printf("Commande: ");
    if(fgets(cmd, MAXLI, stdin) != NULL) {
     char** p_cmd = parseCmd(cmd);

     if(strcmp(p_cmd[0], "exit") == 0) {
        are_we_continuing = false;
     } else {
    
      char temp[256];
      strcpy(temp, "/bin/");
      strcat(temp, p_cmd[0]);
      free(p_cmd[0]);
      p_cmd[0] = strdup(temp);
      
      mbash(p_cmd);
     }

     for (int i = 0; p_cmd[i] != NULL; i++) {
       free(p_cmd[i]);
     }
     free(p_cmd);
    }
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
    execRes = execve(p_cmd[0], (char *const*)p_cmd, (char *const*)environ);
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
