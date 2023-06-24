# Soal 4
Johan Liebert adalah orang yang sangat kalkulatif. Oleh karena itu ia mengharuskan dirinya untuk mencatat log system komputernya. File syslog tersebut harus memiliki ketentuan : 
- Backup file log system dengan format jam:menit tanggal:bulan:tahun (dalam format .txt).
- Isi file harus dienkripsi dengan string manipulation yang disesuaikan dengan jam dilakukannya backup seperti berikut:
    - Menggunakan sistem cipher dengan contoh seperti berikut. Huruf b adalah alfabet kedua, sedangkan saat ini waktu menunjukkan pukul 12, sehingga huruf b diganti dengan huruf alfabet yang memiliki urutan ke 12+2 = 14
    - Hasilnya huruf b menjadi huruf n karena huruf n adalah huruf ke empat belas, dan seterusnya.
    - Setelah huruf z akan kembali ke huruf a
- Buat juga script untuk dekripsinya.
- Backup file syslog setiap 2 jam untuk dikumpulkan
---
## Penyelesaian
## 1. Buat file log_encrypt.sh
```R
~/Documents/Soal4$ nano log_encrypt.sh
```
---
## 2. Buat file enkripsi sesuai dengan format jam:menit tanggal:bulan:tahun dalam txt
```R
#!/bin/bash

# membuat file enkripsi sesuai dengan format jam:menit tanggal:bulan:tahun (dalam format .txt)
encrypt_file="$(date +'%H:%M %d-%m-%Y').txt"
# membuat variabel hour yang berisi jam saat ini
hour="$(date +'%H')"
# variabel hour menggunakan format 24 jam sehingga ada kemungkinan jam ditulis 01-09
# maka variabel shift digunakan untuk menghapus angka 0 di depan jam
shift="${hour#0}"
```
---
## 3. Buat loop untuk membaca setiap character pada `/var/log/syslog` dan melakukan enkripsi pada setiap character yang dibaca
```R
# melakukan loop untuk membaca file yang akan di enkripsi
# -n1 menyatakan bahwa program akan membaca setiap 1 karakter yang ada di file
while read -r -n1 char
do
    # convert setiap input huruf ke dalam bentuk ASCII
    ascii_char=$(printf '%d' "'$char'")
    # jika char merupakan huruf kapital
    if [[ $char =~ [A-Z] ]]
    then 
        # maka 
        # 1) ascii_char - 65 --> 65 adalah ASCII dari huruf A, untuk mengubah range code dari 65-90(ASCII A-Z) menjadi 0-25
        # 2) + shift --> menjalankan metode cipher
        # 3) % 26 --> di modulo 26 untuk memastikan hasilnya tidak lebih dari range 0-25
        # 4) + 65 --> agar hasilnya kembali ke dalam bentuk ASCII uppercase
        encrypt_uppercase=$(( ("$ascii_char" - 65 + "$shift")%26 + 65 ))
        # mengganti huruf pada variabel char dengan huruf yang sudah dienkripsi
        char=$(printf '%b' "$(printf '\\x%x' "$encrypt_uppercase")")
    #jika char merupakan huruf kecil    
    elif [[ $char =~ [a-z] ]] 
    then
        # maka 
        # 1) ascii_char - 97 --> 97 adalah ASCII dari huruf a, untuk mengubah range code dari 97-112(ASCII a-z) menjadi 0-25
        # 2) + shift --> menjalankan metode cipher
        # 3) % 26 --> di modulo 26 untuk memastikan hasilnya tidak lebih dari range 0-25
        # 4) + 65 --> agar hasilnya kembali ke dalam bentuk ASCII lowercase
        encrypt_lowercase=$(( ("$ascii_char" - 97 + "$shift")%26 + 97 ))
        char=$(printf '%b' "$(printf '\\x%x' "$encrypt_lowercase")")
    fi
    #print char sebagai string
    printf '%s' "$char"
#file yang ingin dienkrisi berada pada directory /var/log/syslog dan akan di simpan pada file "$encrypt_file" 
done < /var/log/syslog > /home/sarahnrhsna/Documents/Soal4/encrypt_file/"$encrypt_file"
```
Algoritma enkripsi yang digunakan adalah dengan menggunakan sistem cipher, sehingga kita harus melakukan shift setiap character dengan jam file tersebut dibuat. Untuk dapat menjumlahkan character, gunakan ASCII dari setiap character alphabet, baik uppercase (65-90) maupun lowercase (97-112).

Contoh:

Jika ascii_char = A dan jam = 2 maka

ascii untuk huruf A adalah 65

encrypt_uppercase = (65 - 65 + 2 + 26)%26 + 65 = 2 + 65 = 67

ascii 67 adalah C.

---
## 4. Buat file log_decrypt.sh
```R
~/Documents/Soal4$ nano log_decrypt.sh
```
---
## 5. Masukkan input nama file yang ingin di dekripsi
```R
#!/bin/bash

#masukkan input nama file yang ingin di deskripsi
echo "Type the filename:"
read -r filename
```
---
## 6. Buat file dekripsi
```R
decrypt_file="$(date +'%H:%M %d-%m-%Y')_decrypted-$filename"
# jam yangd digunakan adalah jam pada file enkripsi sehingga harus melakukan cut simulai dari simbol ":" agar hasil yang didapat hanya bagian hour saja
hour="$(echo "$filename" | cut -d ':' -f 1)"
shift="${hour#0}"
```
Untuk hour yang digunakan untuk deskripsi file harus menggunakan data jam pada filename enkripsi, sehingga harus mengambil data jam saja pada filename enkripsi dengan melakukan `hour="$(echo "$filename" | cut -d ':' -f 1)"`. 

Contoh:

nama file yang akan dienkripsi : `09:33 09-03-2023.txt`

maka file akan di cut dimulai dari simbol `:` sampai akhir sehingga hanya tersisa 09 saja. Agar angka 0 di depan angka 9 hilang, maka dilakukan `"${hour#0}"`.

---
## 7. Buat loop untuk membaca setiap character pada file yang ingin di enkripsi dan melakukan dekripsi pada setiap character yang dibaca
```R
while read -r -n1 char
do
    ascii_char=$(printf '%d' "'$char'")
    if [[ $char =~ [A-Z] ]]
    then 
        decrypt_uppercase=$(( ("$ascii_char" - 65 - "$shift" + 26 )% 26 + 65  ))
        char=$(printf '%b' "$(printf '\\x%x' "$decrypt_uppercase")")
    elif [[ $char =~ [a-z] ]] 
    then
        decrypt_lowercase=$(( ("$ascii_char" - 97 - "$shift" + 26 )% 26 + 97 ))
        char=$(printf '%b' "$(printf '\\x%x' "$decrypt_lowercase")")
    fi
    printf '%s' "$char"
done < /home/sarahnrhsna/Documents/Soal4/encrypt_file/"$filename" > /home/sarahnrhsna/Documents/Soal4/decrypt_file/"$decrypt_file"
```
Pada deskripsi, code untuk melakukan looping sama saja seperti pada enkripsi, hanya berbeda pada bagian 
```R
decrypt_uppercase=$(( ("$ascii_char" - 65 - "$shift" + 26 )% 26 + 65  ))

decrypt_lowercase=$(( ("$ascii_char" - 97 - "$shift" + 26 )% 26 + 97 ))
```
untuk mengembalikan character seperti semula maka harus dilakukan `- "$shift" + 26`

Contoh:

Jika ascii_char = C dan jam = 2 maka

ascii untuk huruf C adalah 67

decrypt_uppercase = (67 - 65 - 2 + 26)%26 + 65 = 0 + 65 = 65

ascii 65 adalah A.

---
## 8. Buat cron jobs agar file enkripsi dapat di jalankan setiap 2 jam
```R
# cron jobs:
~$ crontab -e

0 */2 * * * /home/sarahnrhsna/Documents/Soal4/encrypt_file/log_encrypt.sh
```
---
---
## Dokumentasi Output
## 1. Output log_encrypt

## 2. Output log_decrypt

## 3. Cron Jobs

---
Kendala yang terjadi dalam mengerjakan soal ini yaitu pada saat melakukan enkripsi dan dekripsi, output yang dihasilkan tidak sesuai dengan yang diinginkan karena masih ada kesalahan pada code yang dibuat.