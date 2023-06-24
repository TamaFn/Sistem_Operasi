#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]) {
    // Deklarasi
    // Convert string ke integer menggunakan fungsi atoi()
    int jam = atoi(argv[1]);
    int menit = atoi(argv[2]);
    int detik = atoi(argv[3]);
    char path[200];
    // copy isi argumen pada argv[4] ke dalam variabel path
    strcpy(path, argv[4]);

    // Cek banyak argumen(argc) yang diterima
    if (argc != 5) {
        printf("Error: Argumen salah!\n");
        exit(1);
    }

    // Cek Jam 
    // Jika argumen 1 merupakan tanda asterik [*] maka nilai jam = -1
    if (strcmp(argv[1], "*") == 0) {
        jam = -1;
    } else {
        // Jika jam tidak berada di range 0-23 maka akan error
        if (jam < 0 || jam > 23) {
            printf("Error: Jam salah!\n");
            exit(1);
        }
    }

    // Cek Menit
    // Jika argumen 2 merupakan tanda asterik [*] maka nilai menit = -1
    if (strcmp(argv[2], "*") == 0) {
        menit = -1;
    } else {
        // Jika menit tidak berada di range 0-59 maka akan error
        if (menit < 0 || menit > 59) {
            printf("Error: Menit salah!\n");
            exit(1);
        }
    }

    // Cek Detik
    // Jika argumen 3 merupakan tanda asterik [*] maka nilai detik = -1
    if (strcmp(argv[3], "*") == 0) {
        detik = -1;
    } else {
        // Jika detik tidak berada di range 0-59 maka akan error
        if (detik < 0 || detik > 59) {
            printf("Error: Detik salah!\n");
            exit(1);
        }
    }

    // Cek Path
    if (access(path, F_OK) == -1) {
        printf("Error: Path salah!\n");
        exit(1);
    }

    // Membuat Daemon Process
    pid_t pid = fork();
    umask(0);
    // Cek pid
    if (pid < 0) {
        // fork gagal
        printf("Error : Fork gagal!");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        printf("Success: Program cron job berhasil!!!\n");
        exit(0);
    } else {
        // Child process
        // Membuat new session
        setsid();
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        while (1) {
            // Ambil waktu saat ini
            time_t now = time(NULL);
            struct tm* waktu = localtime(&now);

            if ( 
                (jam == -1 || waktu->tm_hour == jam) &&
                (menit == -1 || waktu->tm_min == menit) &&
                (detik == -1 || waktu->tm_sec == detik)
                ){
                // Eksekusi file script bash
                if (execl(path, path, (char *)NULL) == -1) {
                    printf("Error\n");
                    exit(1);
                }
            }
            // Program akan tidur selama 1 detik jika waktu sekarang tidak sesuai dengan argumen yang diinginkan
            sleep(1);
        }
    }
    return 0;
}