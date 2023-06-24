#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

// Fungsi untuk membuat directory
void makeDir(char* dirname){
    pid_t pid = fork();
    int status;

    if(pid < 0){
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        // Membuat direktori dengan menggunakan fungsi mkdir
        char *argv[] = {"mkdir", "-p", dirname, NULL};
        execv("/usr/bin/mkdir", argv);
    }
    while (wait(&status) > 0);
}

// Fungsi untuk menghapus directory
void deleteDir(char *dirname) {
    pid_t pid = fork();
    int status;

    if(pid < 0){
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        // Membuat direktori dengan menggunakan fungsi rm
        char *argv[]={"rm", "-r", dirname, NULL};
        execv("/usr/bin/rm", argv);
    }
    while (wait(&status) > 0);
}

// Fungsi untuk download image pada link https://picsum.photos/
void download_img(char* dirname){
    // Deklarasi
    char filename[20], downlink[20], dirloc[50];
    char *url = {"https://picsum.photos/%d"};
    int status;

    // Download image sebanyak 15 kali
    for(int i=1; i<=15; i++){
        pid_t pid = fork();
        if(pid < 0){
            exit(EXIT_FAILURE);
        }
        if(pid == 0){
            // Ambil waktu saat ini
            time_t now = time(NULL);
            struct tm *time = localtime(&now);
            // Deklarasi size image
            int size = ((time->tm_sec) % 1000) + 50;

            // membuat format nama file gambar sesuai dengan timestamp
            strftime(filename, 20, "%Y-%m-%d %H:%M:%S.png", time);
            // membuat path direktori dengan format string dirname dan filename ke dalam variabel dirloc
            sprintf(dirloc, "%s/%s", dirname, filename);
            // membuat download link dengan format url(https://picsum.photos/) dan size gambar ke dalam variabel downlink
            sprintf(downlink, url, size);

            // download image menggunakan wget
            char *argv[]={"wget", "-qO", dirloc, downlink, NULL};
            execv("/usr/bin/wget", argv);
        }
        // Image akan di download setiap 5 detik
        sleep(5);
    }
    while (wait(&status) > 0);
}

// Fungsi untuk zip directory
void zipDir(char *dirname){
    int status;
    char zipname[50];
    sprintf(zipname, "%s.zip", dirname);

    pid_t pid = fork();
    if(pid < 0){
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        char *argv[] = {"zip", "-qr", zipname, dirname, NULL};
        execv("/usr/bin/zip", argv);
    }
    while(wait(&status) > 0);
}

// Fungsi untuk membuat killer dengan 2 mode yaitu -a dan -b
void makeKiller(char* mode) {
    
}

int main(int argc, char* argv[]){
    // Cek argumen
    if(argc != 2 || argv[1] != "-a" || argv[1] != "-b"){
        printf("Error: Argumen salah, pilih mode -a/-b!");
        exit(EXIT_FAILURE);
    }

    pid_t pid, sid;
    pid = fork();
    
    if(pid < 0){
        exit(EXIT_FAILURE);
    }
    if(pid > 0){
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if(sid < 0){
        exit(EXIT_FAILURE);
    }
    if((chdir("/home/sarahnrhsna/Documents/Sisop/'Modul 2'/Soal2") < 0)){
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    char *mode;
    makeKiller(mode);

    while(1){
        pid_t child_pid = fork();
        int status;
        char dirname[50];
        
        time_t now = time(NULL);
        struct tm *time = localtime(&now);
        strftime(dirname, 50, "%Y-%m-%d_%H:%M:%S", time);
        makeDir(dirname);

        if(child_pid < 0){
            exit(EXIT_FAILURE);
        }
        if(child_pid == 0){
            download_img(dirname);
            zipDir(dirname);
            deleteDir(dirname);
        }

        sleep(30);
        while (wait(&status) > 0);
    }
    return 0;
}

// source:
// https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/tree/master/Modul2#menjalankan-program-secara-bersamaan
// https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/tree/master/Modul2#wait-x-fork-x-exec
// https://stackoverflow.com/questions/22124413/how-to-use-wget-command-using-exec-command-in-linux