#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"


char buffer[BUFFER_SIZE];
char sent_bytes[INDICATOR_SIZE];

int getAvailableBytes(int pipe_in_fd) {
    read(pipe_in_fd, buffer, INDICATOR_SIZE);
    return atoi(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Reader receive invalid count of arguments: 2 expected, but %d given\n", argc - 1);
        exit(-1);
    }

    int pipe_in_fd = atoi(argv[1]);
    int pipe_out_fd = atoi(argv[2]);
    char *filename_in = argv[3];
    char *filename_out = argv[4];

    int fd_in = open(filename_in, O_RDONLY);
    int fd_out = open(filename_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    ssize_t read_bytes = read(fd_in, buffer, BUFFER_SIZE);
    while (read_bytes > 0) {
        sprintf(sent_bytes, "%ld", read_bytes);
        write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);

        write(pipe_out_fd, buffer, read_bytes);

        int bytes_received = getAvailableBytes(pipe_in_fd);
        read_bytes = read(pipe_in_fd, buffer, bytes_received);
        write(fd_out, buffer, read_bytes);

        read_bytes = read(fd_in, buffer, BUFFER_SIZE);
    }

    sprintf(sent_bytes, "%ld", read_bytes);
    write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);

    close(pipe_out_fd);
    close(pipe_in_fd);
    close(fd_in);
    close(fd_out);

    return 0;
}