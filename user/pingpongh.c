// pingpong.c for xv6 (assignment #1 for CSC.T371)
// name: Naoki Kiryu
// id: 18B05119

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

int fd_tx;
int fd_rx;

int spawn() {
    int pid;
    // tx/rx of parent (rx/tx of child)
    int ptx[2], prx[2];

    if (pipe(ptx) == -1) {
        printf("failed to create ptx\n");
        exit(1);
    }
    if (pipe(prx) == -1) {
        printf("failed to create prx\n");
        exit(1);
    }
    pid = fork();

    if (pid == 0) {
        close(ptx[1]);
        close(prx[0]);
        fd_tx = prx[1];
        fd_rx = ptx[0];
    } else {
        close(ptx[0]);
        close(prx[1]);
        fd_tx = ptx[1];
        fd_rx = prx[0];
    }

    return pid;
}

void parent_work(int n) {
    unsigned char d[1] = { 0 };
    // write first
    for (int i = 0; i < n; i++) {
        // printf("parent: %d\n", *d);
        write(fd_tx, d, 1);
        read(fd_rx, d, 1);
        d[0]++;
    }
    wait(0);
}

void child_work(int n) {
    unsigned char d[1] = { 0 };
    // read first
    for (int i = 0; i < n; i++) {
        read(fd_rx, d, 1);
        d[0]++;
        // printf("child: %d\n", *d);
        write(fd_tx, d, 1);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s N\n", argv[0]);
        exit(1);
    }

    // # of rounds
    int n = atoi(argv[1]);

    // tick value before starting rounds
    clock_t start_tick = clock();

    // start
    int pid = spawn();
    if (pid > 0) {
        parent_work(n);
    } else {
        child_work(n);
    }

    // print elapsed time in n rounds
    clock_t end_tick = clock();
    u_int64_t elapsed = (end_tick - start_tick) * 1000000 / CLOCKS_PER_SEC;
    printf("total time in %d rounds: %ld us\n", n, elapsed);
    exit(0);
}
