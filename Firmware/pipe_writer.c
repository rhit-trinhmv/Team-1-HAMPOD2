#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define PIPE_NAME "test_pipe"

void main(){
    if(mkfifo(PIPE_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }
    printf("Good time to check blocking processor usage\n");
    int pipe_fd = open(PIPE_NAME, O_WRONLY);
    if(pipe_fd == -1) {
        perror("open");
        exit(1);
    }
    char *msg = "Testing message";
    write(pipe_fd, msg, strlen(msg));
    close(pipe_fd);
    printf("We done here\n");
}
