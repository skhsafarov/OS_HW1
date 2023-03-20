#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"


#define RT_PIPE "reader-transformer.pipe"
#define TW_PIPE "transformer-writer.pipe"

int createReader(char *pipe_out, char *filename) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./reader", "./reader", pipe_out, filename, NULL);
        exit(0);
    }

    return chpid;
}

int createTransformer(char *pipe_in, char *pipe_out) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./transformer", "./transformer", pipe_in, pipe_out, NULL);
        exit(0);
    }

    return chpid;
}

int createWriter(char *pipe_in, char *filename) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./writer", "./writer", pipe_in, filename, NULL);
        exit(0);
    }

    return chpid;
}

void continueIfProcessStart(pid_t pid, char *process_name) {
    if (pid >= 0) {
        return;
    }

    printf("Can't run %s process\n", process_name);
    exit(-1);
}

void continueIfPipeIsOpen(int status) {
    if (status < 0) {
        printf("Can't open pipe\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Only 2 arguments expected: <filename_in> <filename_out>\n");
        exit(-1);
    }

    mknod(RT_PIPE, S_IFIFO | 0666, 0);
    mknod(TW_PIPE, S_IFIFO | 0666, 0);

    pid_t reader_pid = createReader(RT_PIPE, argv[1]);
    continueIfProcessStart(reader_pid, "reader");

    pid_t transformer_pid = createTransformer(RT_PIPE, TW_PIPE);
    continueIfProcessStart(transformer_pid, "transformer");

    pid_t writer_pid = createWriter(TW_PIPE, argv[2]);
    continueIfProcessStart(writer_pid, "writer");


    int status = 0;
    waitpid(reader_pid, &status, 0);
    waitpid(transformer_pid, &status, 0);
    waitpid(writer_pid, &status, 0);

    return 0;
}