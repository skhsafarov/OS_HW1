#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

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

    int fd1[2];
    int fd2[2];

    continueIfPipeIsOpen(pipe(fd1));
    continueIfPipeIsOpen(pipe(fd2));

    char reader_out[128];
    char transformer_in[128];
    char transformer_out[128];
    char writer_in[128];
    sprintf(reader_out, "%d", fd1[1]);
    sprintf(transformer_in, "%d", fd1[0]);
    sprintf(transformer_out, "%d", fd2[1]);
    sprintf(writer_in, "%d", fd2[0]);

    pid_t reader_pid = createReader(reader_out, argv[1]);
    continueIfProcessStart(reader_pid, "reader");

    pid_t transformer_pid = createTransformer(transformer_in, transformer_out);
    continueIfProcessStart(transformer_pid, "transformer");

    pid_t writer_pid = createWriter(writer_in, argv[2]);
    continueIfProcessStart(writer_pid, "writer");


    int status = 0;
    waitpid(reader_pid, &status, 0);
    waitpid(transformer_pid, &status, 0);
    waitpid(writer_pid, &status, 0);

    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);

    return 0;
}