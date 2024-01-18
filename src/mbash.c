#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>


#define MAXLI 2048
char * current_path;
int path_size;
pid_t pid;

void mbash();
char** parseCmd();
void getCurrentDir();
void handle_signal();
void printhelp();

int main(int argc, char* argv[]) {
  path_size = 20;
  //Gestion de la commande
  char* cmd;
  
  //signaux
  signal(SIGINT, handle_signal); //CTRL+C

  //historique
  using_history();
  read_history("mbashHistory");

  while (1) {  
    getCurrentDir();

    
    //sprintf(prompt, "mbash:%s :", prompt);

    char* prompt = (char*)malloc((strlen(current_path) + 16) * sizeof(char));
    strcpy(prompt, current_path);

    int length = strlen(current_path);
    if(path_size < length) {
     sprintf(prompt, "mbash : ...%.*s : ", path_size, current_path + (length - path_size));
    } else sprintf(prompt, "mbash : %s : ", current_path);
    

    cmd = readline(prompt);
    add_history(cmd);


    if(cmd != NULL) {
     char** p_cmd = parseCmd(cmd);
     

     if(strcmp(p_cmd[0], "exit") == 0) {
        handle_signal(SIGINT);
     } else if(strcmp(p_cmd[0], "pwd") == 0) {
        printf("%s\n", current_path);
     } else if(strcmp(p_cmd[0], "cd") == 0) {
        chdir(p_cmd[1]);
     } else if(strcmp(p_cmd[0], "help") == 0) {
        printhelp();
     } else if(strcmp(p_cmd[0], "lprompt") == 0) {
        if(p_cmd[1] != NULL) {
           path_size = atoi(p_cmd[1]);      
	}
        printf("path_size set to %d\n", path_size);
     } else if(strcmp(p_cmd[0], "history") == 0) {
	HIST_ENTRY **historique = history_list();
        if(historique != NULL) {
	 for(int i = 0; historique[i] != NULL; i++) {
	  printf("%d %s \n", i, historique[i]->line);
         }
        }
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
     free(prompt);
    }
  }

  wait(NULL);
  write_history("mbashHistory");
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
    p_cmd[i] = strdup(token);
    if(strcmp(p_cmd[i], "$$") == 0) sprintf(p_cmd[i], "%d", getpid());

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
   write_history("mbashHistory");
   printf("\n");
   exit(0);
  }
  break;
  //case SIGINT:
  //break;
 }
}

void printhelp() {
  printf("----Mbash HELP\n");
  printf("cd : change directory\n");
  printf("pwd : show current directory\n");
  printf("exit : leave mbash\n");
  printf("lprompt : change the lenght of the prompt\n");
  printf("history : show the command history\n");
  printf("$$ : converted into PID, try \"echo $$\"\n");
  printf("for more commands, type \"exit\" then \"help\"\n");
  printf("\n");
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

  
  for (buf = current_path = NULL; current_path == NULL; size *= 2) {
      if ((buf = realloc(buf, size)) == NULL) {
          perror("realloc");
          exit(EXIT_FAILURE);
      }

      char* temp = getcwd(buf, size);
      if (temp == NULL && errno != ERANGE) {
          perror("getcwd");
          exit(EXIT_FAILURE);
      }
      else current_path = strdup(temp);
  }

  free(buf);
}
