# Soal 2
Sucipto adalah seorang seniman terkenal yang berasal dari Indonesia. Karya nya sudah terkenal di seluruh dunia, dan lukisannya sudah dipajang di berbagai museum mancanegara. Tetapi, akhir-akhir ini sucipto sedang terkendala mengenai ide lukisan ia selanjutnya. Sebagai teman yang jago sisop, bantu sucipto untuk melukis dengan mencarikannya gambar-gambar di internet sebagai referensi !

a. Pertama-tama, buatlah sebuah folder khusus, yang dalamnya terdapat sebuah program C yang per 30 detik membuat sebuah folder dengan nama timestamp [YYYY-MM-dd_HH:mm:ss].

b. Tiap-tiap folder lalu diisi dengan 15 gambar yang di download dari https://picsum.photos/ , dimana tiap gambar di download setiap 5 detik. Tiap gambar berbentuk persegi dengan ukuran (t%1000)+50 piksel dimana t adalah detik Epoch Unix. Gambar tersebut diberi nama dengan format timestamp [YYYY-mm-dd_HH:mm:ss].

c. Agar rapi, setelah sebuah folder telah terisi oleh 15 gambar, folder akan di zip dan folder akan di delete(sehingga hanya menyisakan .zip).

d. Karena takut program tersebut lepas kendali, Sucipto ingin program tersebut men-generate sebuah program "killer" yang siap di run(executable) untuk menterminasi semua operasi program tersebut. Setelah di run, program yang menterminasi ini lalu akan mendelete dirinya sendiri.

e. Buatlah program utama bisa dirun dalam dua mode, yaitu MODE_A dan MODE_B. untuk mengaktifkan MODE_A, program harus dijalankan dengan argumen -a. Untuk MODE_B, program harus dijalankan dengan argumen -b. Ketika dijalankan dalam MODE_A, program utama akan langsung menghentikan semua operasinya ketika program killer dijalankan. Untuk MODE_B, ketika program killer dijalankan, program utama akan berhenti tapi membiarkan proses di setiap folder yang masih berjalan sampai selesai(semua folder terisi gambar, terzip lalu di delete).

Catatan :
- Tidak boleh menggunakan system()
- Proses berjalan secara daemon
- Proses download gambar pada beberapa folder dapat berjalan secara bersamaan (overlapping)

---
## Penyelesaian
## 1. Buat file lukisan.c
```R
~$ nano lukisan.c
```
---
## 2. Buat fungsi untuk membuat folder
```R
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
```
- Fungsi akan membuat direktori dengan memanfaatkan fungsi `mkdir` yang berada pada path `/usr/bin/mkdir` dan kemudian akan dieksekusi menggunakan execv. 
- Fungsi akan menunggu semua child process selesai dijalankan terlebih dahulu, setelahnya baru akan menjalankan parent process.
---
## 3. Buat fungsi untuk menghapus folder
```R
// Fungsi untuk menghapus directory
void deleteDir(char *dirname) {
    pid_t pid = fork();
    int status;

    if(pid < 0){
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        // Menghapus direktori dengan menggunakan fungsi rm
        char *argv[]={"rm", "-r", dirname, NULL};
        execv("/usr/bin/rm", argv);
    }
    while (wait(&status) > 0);
}
```
- Fungsi akan menghapus direktori dengan menggunakan fungsi rm yang berada pada path `/usr/bin/rm` dan menggunakan option `-r` yaitu untuk menghapus direktori beserta isinya secara rekursif. Kemudian akan dijalankan dengan menggunakan execv.
- Fungsi akan menunggu semua child process selesai dijalankan terlebih dahulu, setelahnya baru akan menjalankan parent process.

---
## 4. Buat fungsi untuk download image
```R
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
```
- Fungsi akan melakukan loop sebanyak 15 kali karena dalam satu folder akan ada 15 gambar.
- Ambil waktu saat ini yang akan digunakan sebagai nama file gambarnya dengan menggunakan kode `time_t now = time(NULL);`
- Pecah unsur-unsur waktu(jam,menit,detik) pada waktu saat ini dengan menggunakan `struct tm`. Ini akan digunakan untuk menentukan size gambar.
- Deklarasi size gambar yaitu,
    ```R
    int size = ((time->tm_sec) % 1000) + 50;
    ```
    size gambar yang diinginkan yaitu (t%1000)+50 piksel dimana t adalah detik. Ambil detik dengan menggunakan `time->tm_sec`.
- Buat format nama file gambar yang disesuaikan dengan waktu saat ini:
    ```R
    strftime(filename, 20, "%Y-%m-%d %H:%M:%S.png", time);
    ```
    Fungsi `strftime` akan mengubah waktu yang tersimpan pada `time` menjadi sebuah string sesuai dengan format yang diinginkan yaitu `"%Y-%m-%d %H:%M:%S.png"` dan akan disimpan ke dalam variabel filename.
- Buat path directory untuk tempat image akan di download
    ```R
    sprintf(dirloc, "%s/%s", dirname, filename);
    ```
    Fungsi `sprintf` akan mencetak path yaitu `dirname/filename` ke dalam variabel `dirloc`.
- Buat link download gambar beserta format size yang diinginkan
    ```R
    sprintf(downlink, url, size);
    ```
    Fungsi `sprintf` akan mencetak url link download dengan ketentuan format gambar sesuai dengan `size` yaitu `((time->tm_sec) % 1000) + 50`.
- Eksekusi menggunakan execv
    ```R
    char *argv[]={"wget", "-qO", dirloc, downlink, NULL};
    execv("/usr/bin/wget", argv);
    ```
    Gunakan fungsi `wget` untuk bisa mendownload gambar dari link yang diinginkan yaitu `downlink` dan menyimpan gambar dalam bentuk file (-O) ke dalam folder yang kita inginkan yaitu `dirloc`.
- Fungsi akan menunggu selama 5 detik sebelum mendownload gambar lainnya
- Fungsi akan menunggu semua child process selesai dijalankan terlebih dahulu, setelahnya baru akan menjalankan parent process.
---
## 5. Buat fungsi zip folder
```R
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
```
- Buat nama file zip sesuai yang diinginkan menggunakan fungsi `sprintf`
    ```R
    sprintf(zipname, "%s.zip", dirname);
    ```
    Fungsi sprintf akan mencetak nama file zip dengan format `dirname.zip` dan akan disimpan di dalam variabel `zipname`.
- Eksekusi menggunakan execv
    ```R
    char *argv[] = {"zip", "-qr", zipname, dirname, NULL};
    execv("/usr/bin/zip", argv);
    ```
    Gunakan fungsi `zip` yang berada pada path `/usr/bin/zip` untuk melakukan zip folder.
---
## 6. Buat killer program
```R
// Fungsi untuk membuat killer dengan 2 mode yaitu -a dan -b
void makeKiller(char* mode) {
    
}
```
---
## 7. Buat fungsi main
```R
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
```                         

---
---
## Dokumentasi Output
---
Kendala pada poin d dan e yaitu untuk membuat killer program yang bisa berjalan dengan 2 mode -a/-b.




# Soal 4
Banabil adalah seorang mahasiswa yang rajin kuliah dan suka belajar. Namun naasnya Banabil salah mencari teman, dia diajak ke toko mainan oleh teman-temannya dan teracuni untuk membeli banyak sekali mainan dan kebingungan memilih mainan mana yang harus dibeli. Hal tersebut menyebabkan Banabil kehilangan fokus dalam pengerjaan tugas-tugas yang diberikan oleh dosen nya. Untuk mengembalikan fokusnya, Banabil harus melatih diri sendiri dalam membuat program untuk menjalankan script bash yang menyerupai crontab dan menggunakan bahasa C karena baru dipelajari olehnya.

Untuk menambah tantangan agar membuatnya semakin terfokus, Banabil membuat beberapa ketentuan custom yang harus dia ikuti sendiri. Ketentuan tersebut berupa:
- Banabil tidak ingin menggunakan fungsi system(), karena terlalu mudah.
- Dalam pelatihan fokus time managementnya, Banabil harus bisa membuat program yang dapat menerima argumen berupa Jam (0-23), Menit (0-59), Detik (0-59), Tanda asterisk [ * ] (value bebas), serta path file .sh.
- Dalam pelatihan fokus untuk ketepatan pilihannya, Banabil ingin programnya dapat mengeluarkan pesan “error” apabila argumen yang diterima program tidak sesuai. Pesan error dapat dibentuk sesuka hati oleh pembuat program. terserah bagaimana, yang penting tulisan error.
- Terakhir, dalam pelatihan kesempurnaan fokusnya, Banabil ingin program ini berjalan dalam background dan hanya menerima satu config cron.
- Bonus poin apabila CPU state minimum.

Contoh untuk run: /program \* 44 5 /home/Banabil/programcron.sh

---
## Penyelesaian
## 1. Buat file mainan.c
```R
~$ nano mainan.c
```
---
## 2. Deklarasi argumen jam, menit, detik, dan path
```R
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
```
1. `int jam = atoi(argv[1]);` 
    
    Deklarasi jam. Isi dari variabel jam adalah argumen 1. Karena jam merupakan integer sementara argumen 1 yang diterima adalah string, maka perlu ada konversi dari string ke integer dengan menggunakan fungsi `atoi();`. Konversi juga berlaku untuk variabel menit dan detik.
2. `int menit = atoi(argv[2]);` 

    Deklarasi menit. Isi dari variabel menit adalah argumen 2.

3. `int detik = atoi(argv[3]);`

    Deklarasi detik. Isi dari variabel detik adalah argumen 3.

4. `char path[200];`
    
    Deklarasi path yang memiliki panjang maksimum 200.

5. `strcpy(path, argv[4]);`

    copy isi dari argumen 4 ke dalam variabel path dengan menggunakan `strcpy();`
---
## 3. Validasi jumlah argumen
```R
// Cek banyak argumen(argc) yang diterima
    if (argc != 5) {
        printf("Error: Argumen salah!\n");
        exit(1);
    }
```
Cek jika jumlah argumen yang diterima != 5, maka program error karena argumen yang diterima ada 5 yaitu program yang akan di run (./mainan), jam, menit, detik, dan path.

Contoh input:   
./mainan \* \* 1 /home/sarahnrhsna/Documents/Sisop/'Modul 2'/programcron.sh

---
## Validasi argumen jam
```R
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
```
1. Jika jam adalah tanda asterik, maka set value jam = -1
2. Jika jam berada di luar range 0-23 maka akan error
---
## 5. Validasi argumen menit
```R
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
```
1. Jika menit adalah tanda asterik, maka set value menit = -1
2. Jika menit berada di luar range 0-59 maka akan error
---
## 6. Validasi argumen detik
```R
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
```
1. Jika detik adalah tanda asterik, maka set value detik = -1
2. Jika detik berada di luar range 0-59 maka akan error
---
## 7. Validasi path
```R
    // Cek Path
    if (access(path, F_OK) == -1) {
        printf("Error: Path salah!\n");
        exit(1);
    }
```                         
- cek dengan menggunakan `access();` apakah path tersedia. Jika path tidak tersedia maka access akan mengembalikan nilai -1 dan program akan error
---
## 8. Membuat daemon process
```R
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
```

Pada loop while

1. Ambil waktu saat ini
    ```R
    time_t now = time(NULL);
    struct tm* waktu = localtime(&now);
    ```
    pecah unsur-unsur waktu(jam,menit,detik) pada waktu saat ini dengan menggunakan `struct tm`.
2. Eksekusi file script bash
    ```R
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
    ```
---
---
## Dokumentasi Output
## 1. Program berhasil

## 2. Error argumen salah

## 3. Error jam salah

## 4. Error menit salah

## 5. Error detik salah

## 6. Error path salah

---
Tidak ada kendala saat mengerjakan soal 4