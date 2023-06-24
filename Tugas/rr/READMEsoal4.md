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