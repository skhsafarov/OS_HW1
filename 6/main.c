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

int ioManipulator(char *pipe_in_fd, char *pipe_out_fd, char *filename_in, char *filename_out) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./io_manipulator", "./io_manipulator", pipe_in_fd, pipe_out_fd, filename_in, filename_out, NULL);
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

    int fd1[2];
    int fd2[2];

    continueIfPipeIsOpen(pipe(fd1));
    continueIfPipeIsOpen(pipe(fd2));

    char manipulator_out[128];
    char transformer_in[128];
    char transformer_out[128];
    char manipulator_in[128];
    sprintf(manipulator_out, "%d", fd1[1]);
    sprintf(transformer_in, "%d", fd1[0]);
    sprintf(transformer_out, "%d", fd2[1]);
    sprintf(manipulator_in, "%d", fd2[0]);

    pid_t manipulator_pid = ioManipulator(manipulator_in, manipulator_out, argv[1], argv[2]);
    continueIfProcessStart(manipulator_pid, "manipulator");

    pid_t transformer_pid = createTransformer(transformer_in, transformer_out);
    continueIfProcessStart(transformer_pid, "transformer");

    int status = 0;
    waitpid(manipulator_pid, &status, 0);
    waitpid(transformer_pid, &status, 0);

    return 0;
}