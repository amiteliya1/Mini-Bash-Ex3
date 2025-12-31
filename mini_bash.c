#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     
#include <sys/wait.h>   

#define MAX_LINE 1024
#define MAX_ARGS 64

int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    int status;

    while (1) {
        printf("mini-bash$ "); 
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            break;
        }

        line[strcspn(line, "\n")] = 0;

        int i = 0;
        args[i] = strtok(line, " \t");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            args[++i] = strtok(NULL, " \t");
        }

        if (args[0] == NULL) continue;

        if (strcmp(args[0], "exit") == 0) { 
            break;
        }
        
        if (strcmp(args[0], "cd") == 0) { 
            if (args[1] != NULL) {
                if (chdir(args[1]) != 0) {
                    perror("mini-bash");
                }
            }
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
        } 
        else if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, "%s: Unknown Command\n", args[0]);
                exit(EXIT_FAILURE);
            }
        } 
        else {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("Command exited with status %d\n", WEXITSTATUS(status));
            }
        }
    }
    return 0;
}