#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define TEST_PIPE "test_pipe"
void main(){
    int pipe_fd = open(TEST_PIPE, O_RDONLY);
    if(pipe_fd == -1) {
        perror("open");
        exit(1);
    }
    char buffer[256];
    int bytes_read = read(pipe_fd, buffer, sizeof(buffer));
    if(bytes_read == -1) {
        perror("read");
        exit(1);
    } else if(bytes_read == 0) {
        printf("No data received\n");
    } else {
        buffer[bytes_read] = '\0';
        printf("Received data from the named pipe: %s\n", buffer);
    }
    close(pipe_fd);
}
