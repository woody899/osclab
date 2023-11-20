#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
// declaring some variables
pid_t id;
int logging_fd[2];
char mysteriousBuffer[100000];
int i = 0;

int create_log_process() {
    if (pipe(logging_fd) == -1) {
        perror("PIPE FAILED");
        return -1;
    }

    id = fork();

    if (id == -1) {
        perror("FORK FAILED");
        return -1;
    }
    if (id == 0) {
        FILE *log = fopen("log_gateway.log", "a");
        close(logging_fd[1]);
        while ((read(logging_fd[0], mysteriousBuffer, sizeof(mysteriousBuffer))) > 0) {
              char *event = strtok(mysteriousBuffer, "\n");
              while (event != NULL) {
                time_t mytime = time(NULL);
                char *time_str = ctime(&mytime);
                time_str[strlen(time_str) - 1] = '\0';
                fprintf(log, "%d - %s %s\n", i, time_str, event);
                i++;
                event = strtok(NULL, "\n");
              }
              //memset basically resets the buffer
              memset(mysteriousBuffer,0,sizeof(mysteriousBuffer));
        }
        fclose(log);
        close(logging_fd[0]);  // Close the read end of the logging pipe
        exit(0);
      }
    return 0;
}
int write_to_log_process(char *msg) {
    write(logging_fd[1], msg, strlen(msg));
    return 0;
}

int end_log_process(){
    close(logging_fd[1]);
    waitpid(id,NULL,0);
    return 0;
}
