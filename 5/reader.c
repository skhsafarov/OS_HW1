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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Reader receive invalid count of arguments: 2 expected, but %d given\n", argc - 1);
        exit(-1);
    }

    int pipe_out_fd = open(argv[1], O_WRONLY);
    char *filename = argv[2];

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        printf("Can't open input file\n");
        sprintf(sent_bytes, "0");
        write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);
        exit(-1);
    }

    ssize_t read_bytes = read(fd, buffer, BUFFER_SIZE);
    while (read_bytes > 0) {
        sprintf(sent_bytes, "%ld", read_bytes);
        write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);

        write(pipe_out_fd, buffer, read_bytes);

        read_bytes = read(fd, buffer, BUFFER_SIZE);
    }

    sprintf(sent_bytes, "%ld", read_bytes);
    write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);

    close(pipe_out_fd);

    return 0;
}