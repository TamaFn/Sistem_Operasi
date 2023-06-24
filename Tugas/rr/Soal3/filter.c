#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

int main() {
    pid_t pid;
    DIR *dir;
    struct dirent *entry;

    // Download file database dari URL menggunakan wget
    pid = fork();
    if (pid == 0) {
        execl("/usr/bin/wget", "wget", "—no-check-certificate", "https://drive.google.com/uc?export=download&id=1zEAneJ1-0sOgt13R1gL4i1ONWfKAtwBF", "-O", "players.zip", NULL);
        exit(0);
    } else {
        wait(NULL);
    }

    // Ekstrak file players.zip menggunakan unzip
    pid = fork();
    if (pid == 0) {
        execl("/usr/bin/unzip", "unzip", "-q", "players.zip", "-d", ".", NULL);
        exit(0);
    } else {
        wait(NULL);
    }

    // Hapus file players.zip dan database.zip menggunakan rm
    pid = fork();
    if (pid == 0) {
        execl("/usr/bin/rm", "rm", "-f", "players.zip", NULL);
        exit(0);
    } else {
        wait(NULL);
    }
    return 0;
}
