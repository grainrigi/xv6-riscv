// pingpong.c for xv6 (assignment #1 for CSC.T371)
// name: Naoki Kiryu
// id: 18B05119

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

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
        fprintf(1, "usage: %s N\n", argv[0]);
        exit(1);
    }

    // # of rounds
    int n = atoi(argv[1]);

    // tick value before starting rounds
    int start_tick = uptime();

    // start
    int pid = spawn();
    if (pid > 0) {
        parent_work(n);
    } else {
        child_work(n);
    }

    // print # of ticks in nrounds
    printf("# of ticks in %d rounds: %d\n", n, uptime() - start_tick);
    exit(0);
}
