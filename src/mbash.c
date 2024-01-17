#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <errno.h>


#define MAXLI 2048
char *path;
char *dirName;
pid_t pid;

void mbash();
char** parseCmd();
void getCurrentDir();
void handle_signal();

int main(int argc, char* argv[]) {
  //Gestion de la commande
  char cmd[MAXLI];
  bool are_we_continuing = true;
  
  //signaux
  signal(SIGINT, handle_signal); //CTRL+C

  while (are_we_continuing) {
    printf("Commande: ");
    
    
    if(fgets(cmd, MAXLI, stdin) != NULL) {
     char** p_cmd = parseCmd(cmd);
     getCurrentDir();

     if(strcmp(p_cmd[0], "exit") == 0) {
        are_we_continuing = false;
     } else if(strcmp(p_cmd[0], "pwd") == 0) {
        printf("%s", path);
     } else if(strcmp(p_cmd[0], "cd") == 0) {
        chdir(p_cmd[1]);
     } else if(strcmp(p_cmd[0], "history") == 0) {

     } else {
     
      //utile pour execve, mais execvp s'en charge pour nous
      //char temp[256];
      //strcpy(temp, "/bin/");
      //strcat(temp, p_cmd[0]);
      //free(p_cmd[0]);
      //p_cmd[0] = strdup(temp);
      
      mbash(p_cmd);
     }

     for (int i = 0; p_cmd[i] != NULL; i++) {
       free(p_cmd[i]);
     }
     free(p_cmd);
    }
  }

  wait(NULL);
  
  return 0;
}

void mbash(char* p_cmd[]) {

  extern char **environ;
  int execRes;
  
  pid = fork();
  switch (pid) {
    case -1 :
    perror("fork error");
    break;

    case 0 :
    //execve -> vieille version, execvp est pareil mais en mieux
    //execRes = execve(p_cmd[0], (char *const*)p_cmd, (char *const*)environ);
    execRes = execvp(p_cmd[0], (char *const*)p_cmd);
    if(execRes == -1) perror("exec error");
    exit(0);
    break;
    
    default:
    wait(NULL);
    pid = 0;
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

void handle_signal(int signo) {
 switch (signo) {
 case SIGINT:
  if(pid > 0) {
   kill(pid, SIGTERM);
   waitpid(pid, NULL, 0);
   pid = 0;
  }
  else {
   wait(NULL);
   exit(0);
  }
  break;
  //case SIGINT:
  //break;
 }
}

//Source : https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
void getCurrentDir() {
  long path_max;
  size_t size;
  char *buf;

  path_max = pathconf(".", _PC_PATH_MAX);
  if (path_max == -1)
      size = 1024;
  else if (path_max > 10240)
      size = 10240;
  else
      size = path_max;

  
  for (buf = path = NULL; path == NULL; size *= 2) {
      if ((buf = realloc(buf, size)) == NULL) {
          perror("realloc");
          exit(EXIT_FAILURE);
      }

      path = getcwd(buf, size);
      if (path == NULL && errno != ERANGE) {
          perror("getcwd");
          exit(EXIT_FAILURE);
      }
  }

  free(buf);
}
