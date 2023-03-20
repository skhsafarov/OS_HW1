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

int getAvailableBytes(int pipe_in_fd) {
    read(pipe_in_fd, buffer, INDICATOR_SIZE);
    return atoi(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Reader receive invalid count of arguments: 2 expected, but %d given\n", argc - 1);
        exit(-1);
    }

    int pipe_in_fd = open(argv[1], O_RDONLY);
    char *filename = argv[2];

    int fd_output = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    // Если файл не удалось открыть, выводим информационное сообщение об ошибке.
    // Но работу не заканчиваем, так как до этого нужно прочитать информацию из пайпа.
    // Потому что может возникнуть ситуация, когда пайп переполнен и write()
    // у другого потока блокирует выполнение.
    if (fd_output < 0) {
        printf("Can't open output file\n");
    }

    int bytes_received = getAvailableBytes(pipe_in_fd);
    ssize_t read_bytes = read(pipe_in_fd, buffer, bytes_received);
    while (read_bytes > 0) {
        if (fd_output >= 0) {
            write(fd_output, buffer, read_bytes);
        }

        bytes_received = getAvailableBytes(pipe_in_fd);
        read_bytes = read(pipe_in_fd, buffer, bytes_received);
    }

    close(fd_output);
    close(pipe_in_fd);

    return 0;
}