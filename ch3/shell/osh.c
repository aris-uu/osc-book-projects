#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80

int main(void) {
    char *args[MAX_LINE/2 + 1];
    int should_run = 1;
    char history[MAX_LINE];
    history[0] = '\0';


    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        char input[MAX_LINE];
        fgets(input, sizeof(input), stdin);
        
        if (strncmp(input, "!!", 2) == 0) {
            if (history[0] == '\0') {
                printf("No commands in history\n");
                continue;
            }
            printf("Last command: %s\n", history);
            strcpy(input, history);
        } else {
            strncpy(history, input, MAX_LINE);
            history[MAX_LINE - 1] = '\0';
        };

        char *token = strtok(input, " \n");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " \n");
        }
        args[i] = NULL;

        for (int j = 0; j < i; j++) {
            printf("args[%d]: %s\n", j, args[j]);
        }

        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
            break;
        }

        

        int background = 0;
        if (strcmp(args[i-1], "&") == 0) {
            args[i-1] = NULL;
            background = 1;
        }
        
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
        } else if (pid > 0) {
            // Parent process
            int status;
            if (!background) {
                waitpid(pid, &status, 0);
            } else {
                printf("Background process started with PID: %d\n", pid);
            }
            // waitpid(pid, &status, 0);
        } else {
            // Child process
            execvp(args[0], args);
            perror("Exec failed");
            return 1;
        }

    }
}