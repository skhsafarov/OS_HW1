#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"


int isVowel(char letter) {
    return letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u' || letter == 'y'
        || letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U' || letter == 'Y';
}

void replaceLetters(char string[], ssize_t size) {
    for (ssize_t i = 0; i < size; ++i) {
        if (string[i] >= 'a' && !isVowel(string[i])) {
            string[i] = (char) (string[i] - 32);
        }
    }
}

char buffer[BUFFER_SIZE];
char sent_bytes[INDICATOR_SIZE];

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
    int pipe_out_fd = open(argv[2], O_WRONLY);

    int bytes_received = getAvailableBytes(pipe_in_fd);
    ssize_t read_bytes = read(pipe_in_fd, buffer, bytes_received);
    while (read_bytes > 0) {
        replaceLetters(buffer, read_bytes);

        sprintf(sent_bytes, "%ld", read_bytes);
        write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);

        write(pipe_out_fd, buffer, read_bytes);

        bytes_received = getAvailableBytes(pipe_in_fd);
        read_bytes = read(pipe_in_fd, buffer, bytes_received);
    }

    sprintf(sent_bytes, "0");
    write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);

    close(pipe_in_fd);
    close(pipe_out_fd);
    return 0;
}