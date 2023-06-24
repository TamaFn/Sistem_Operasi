# Soal 3
Peter Griffin hendak membuat suatu sistem register pada script louis.sh dari setiap user yang berhasil didaftarkan di dalam file /users/users.txt. Peter Griffin juga membuat sistem login yang dibuat di script retep.sh

Untuk memastikan password pada register dan login aman, maka ketika proses input passwordnya harus memiliki ketentuan berikut:
* Minimal 8 karakter
* Memiliki minimal 1 huruf kapital dan 1 huruf kecil
* Alphanumeric
* Tidak boleh sama dengan username 
* Tidak boleh menggunakan kata chicken atau ernie

Setiap percobaan login dan register akan tercatat pada log.txt dengan format : YY/MM/DD hh:mm:ss MESSAGE. Message pada log akan berbeda tergantung aksi yang dilakukan user.
* Ketika mencoba register dengan username yang sudah terdaftar, maka message pada log adalah REGISTER: ERROR User already exists
* Ketika percobaan register berhasil, maka message pada log adalah REGISTER: INFO User USERNAME registered successfully
* Ketika user mencoba login namun passwordnya salah, maka message pada log adalah LOGIN: ERROR Failed login attempt on user USERNAME
Ketika user berhasil login, maka message pada log adalah LOGIN: INFO User USERNAME logged in

## Penyelesaian
## 1. louis.sh
***
### a. Membuat file /users/users.txt dan register system di file louis.sh
```R
~$ mkdir /users
~$ touch /users/users.txt

~$ nano louis.sh
```
***
### b. Memasukkan input username dan password user
```R
#!/bin/bash

date=$(date +'%Y-%m-%d %H:%M:%S')

echo -e "\nREGISTER SYSTEM"
echo "Input username:"
read -r username

echo "Input password:"
read  -r password
```
pada bagian `date=$(date +'%Y-%m-%d %H:%M:%S')` berfungsi untuk mendefinisikan date dengan format YYYY-MM-DD H:M:S   yang nantinya akan dimasukkan ke dalam file `log.txt` untuk setiap percobaan login dan register
***
### c. Membuat validate_password
```R
#fungsi untuk memvalidasi password
validate_password(){
	password=$1

	#password minimal 8 karakter
	if [ ${#password} -lt 8 ]
	then
		echo "REGISTER: ERROR Failed password length should be at least 8 characters"
		return 1
	fi

	#password hanya alphanumeric
	#jika ada simbol maka
	if ! echo "$password" | grep -q '^[[:alnum:]]*$'
	then
		echo "REGISTER: ERROR Failed password should contain alphanumeric only"
		return 1
	fi

	#password minimal 1 huruf kapital, 1 huruf kecil, dan 1 angka
	if ! [[ $password =~ [A-Z] ]] || ! [[ $password =~ [a-z] ]] || ! [[ $password =~ [0-9] ]]
	then
    		echo "REGISTER: ERROR Failed password should contain at least one uppercase letter, one lowercase letter, and one digit number"
		return 1
	fi

	return 0
}
```
Fungsi validate_password digunakan untuk mengecek password user dengan ketentuan sebagai berikut:
* Minimal 8 karakter
  ```R
  #password minimal 8 karakter
	if [ ${#password} -lt 8 ]
	then
		echo "REGISTER: ERROR Failed password length should be at least 8 characters"
		return 1
	fi
  ```
  Kode di atas akan mengecek apakah panjang string `$password` lebih kecil dari 8.
  - Jika iya, maka program akan mengeluarkan informasi error
    `REGISTER: ERROR Failed password length should be at least 8 characters` dan `validate_password` akan return 1.
  - Jika tidak, maka `$password` memenuhi syarat minimal 8 karakter dan `validate_password` akan return 0.

* Memiliki minimal 1 huruf kapital dan 1 huruf kecil
  ```R
  if ! [[ $password =~ [A-Z] ]] || ! [[ $password =~ [a-z] ]] || ! [[ $password =~ [0-9] ]]
	then
    		echo "REGISTER: ERROR Failed password should contain at least one uppercase letter, one lowercase letter, and one digit number"
		return 1
	fi

  ```
  Kode di atas akan mengecek apakah `$password` tidak memiliki minimal 1 huruf kapital, 1 huruf kecil, dan 1 digit angka.
  * Jika iya, maka program akan mengeluarkan informasi error `REGISTER: ERROR Failed password should contain at least one uppercase letter, one lowercase letter, and one digit number` dan `validate_password` akan return 1.
  - Jika tidak, maka `$password` memenuhi syarat minimal 1 huruf kapital dan 1 huruf kecil dan `validate_password` akan return 0.
* Alphanumeric
  ```R
  if ! echo "$password" | grep -q '^[[:alnum:]]*$'
	then
		echo "REGISTER: ERROR Failed password should contain alphanumeric only"
		return 1
	fi
  ```
  Kode di atas akan mengecek apakah `$password` memiliki simbol dan bukan alphanumeric.
  * Jika iya, maka program akan mengeluarkan informasi error `REGISTER: ERROR Failed password should contain alphanumeric only` dan `validate_password` akan return 1.
  - Jika tidak, maka `$password` memenuhi syarat alphanumeric dan `validate_password` akan return 0.
***
### d. Melakukan pengecekan apakah username pernah dibuat sebelumnya
```R
#Cek apakah username sudah pernah ada di /users/users.txt
if grep -q "$username" /users/users.txt
then
	echo "REGISTER: ERROR User already exists"
	echo "Please try again"
	echo "$date REGISTER: ERROR User already exists" >> log.txt
else
```
Pada kode di atas, program akan mencari string yang sesuai dengan `$password` di dalam `/users/users.txt`. Jika ditemukan, artinya username sudah pernah digunakan sebelumnya dan user tidak bisa membuat akun menggunakan username tersebut. Program akan mengeluarkan informasi error `REGISTER: ERROR User already exists` dan data waktu user melakukan percobaan register akan disimpan di dalam file `log.txt` sesuai dengan informasi error yang ditampilkan yaitu pada bagian `echo "$date REGISTER: ERROR User already exists" >> log.txt`.
***
### e. Melakukan pengecekan apakah username=password
```R
#cek apakah password = username
	if [ "$password" == "$username" ]
	then
		echo "REGISTER: ERROR Failed password cannot be the same as username"
		echo "Please try again"
```
User tidak bisa membuat password yang sama dengan username, sehingga apabila kode di atas `true`, maka program akan mengeluarkan informasi error `REGISTER: ERROR Failed password cannot be the same as username`.
***
### f. Melakukan pengecekan password=chicken dan password=ernie
```R
#cek apakah password = chicken
if [ "$password" == "chicken" ]
then
	echo "REGISTER: ERROR Failed you can't use 'chicken' as your password"
	echo "Please try again"
else
	#cek apakah password = ernie
	if [ "$password" == "ernie" ]
	then
		echo "REGISTER: ERROR Failed you can't use 'ernie' as your password"
		echo "Please try again"
```
  * Jika `password=chicken` maka program akan mengeluarkan informasi error `REGISTER: ERROR Failed you can't use 'chicken' as your password`
  * Jika `password=ernie` maka program akan mengeluarkan informasi error `REGISTER: ERROR Failed you can't use 'ernie' as your password`
***
### g. Cek validasi password dengan validate_password
```R
#cek validasi password menggunakan fungsi validate_password
if ! validate_password "$password"
then
	echo "Please try again"
else
	#REGISTER BERHASIL
  	echo "REGISTER: INFO User $username registered successfully"
	echo "$date REGISTER: INFO User $username registered successfully" >> log.txt
	echo "username: $username, password: $password" >> /users/users.txt
fi
```
Pada kode di atas tanda `!` menandakan bahwa validate_password akan nilai returnnya adalah false atau 0. Artinya password memenuhi semua persyaratan yang ada di validate_password. Apabila semua persyaratan telah terpenuhi, maka user berhasil melakukan registrasi. Log akan ditambahkan sesuai dengan waktu dan informasi yang dikeluarkan serta akan disimpan di dalam file `log.txt`. Terakhir, username dan password user akan disimpan di dalam `users/users.txt`.

=====================================================================================
## 2. retep.sh
***
### a. Membuat login system di file retep.sh
```R
~$ nano retep.sh
```
***
### b. Memasukkan input username dan password dari user
```R
#!/bin/bash

date=$(date +'%Y-%m-%d %H:%M:%S')

echo "Hello! :3"
echo "Do you have an account?y/n?"
echo -n "Answer: "
read -r answer

#jika user menjawab n maka user akan langsung menuju REGISTER SYSTEM
if [ "$answer" == n ]
then
	bash louis.sh
else
	echo -e "\nLOGIN SYSTEM"
	echo "Please input your username:"
	read -r username

	echo "Please input your password:"
	read -r -s password
```
Pada kode di atas, user akan diberi pertanyaan terlebih dahulu, apakah user sudah memiliki akun atau belum.
- Jika n, maka user akan langsung menuju register system pada louis.sh untuk melakukan registrasi.
- jika y, maka user akan memasukkan input username dan password
***
### c. Melakukan pengecekan kembali apakah username benar-benar ada pada file /users/users.txt
```R
#Cek kembali apakah user benar-benar sudah memiliki akun
	Check_Username=$(awk '/'"$username"'/ {print 1}' /users/users.txt)
	if [[ $Check_Username -ne 1 ]]
	then
		echo "LOGIN: ERROR Failed you haven't registered yet/wrong username"
		echo "$date LOGIN: ERROR Failed login attempt on user $username" >> log.txt

		echo -e "\nGo to register system? y/n?"
		echo -n "Answer: "
		read -r command

		if [ "$command" == y ]
		then
			bash louis.sh
		else
			exit
		fi
```
Pada kode di atas, pengecekan username menggunakan `awk` untuk mencari string `$username` pada /users/users.txt dan jika ditemukan maka `Check_Username` akan bernilai 1.
- Jika `Check_Username -ne 1`, artinya Check_Username tidak sama dengan 1, maka program akan mengeluarkan informasi error `LOGIN: ERROR Failed you haven't registered yet/wrong username` dan informasi tersebut akan disimpan ke dalam file `log.txt` sesuai dengan `date` nya. User juga bisa menuju register system untuk membuat akun baru.
***
### d. Melakukan pengecekan password
```R
else
		#Cek apakah password sesuai
        	if grep -q "$username" /users/users.txt && grep -q "$password" /users/users.txt
        	then
			#LOGIN BERHASIL
                	echo "LOGIN: INFO User $username logged in"
                	echo "$date LOGIN: INFO User $username logged in" >> log.txt
        	else
                	echo "LOGIN: ERROR Failed login attempt on user $username"
                	echo "$date LOGIN: ERROR Failed login attempt on user $username" >> log.txt
		fi
	fi
fi
```
Kode di atas akan dieksekusi jika `Check_Username = 1` dan digunakan untuk mengecek apakah password yang dimasukkan oleh user sudah sesuai dengan data yang ada pada `/users/users.txt`. 
- Jika sesuai, maka login berhasil dan program akan mengeluarkan informasi `LOGIN: INFO User $username logged in`. Data waktu dan informasi login akan disimpan ke dalam file `log.txt`.
- Jika tidak sesuai, maka program akan mengeluarkan informasi error `LOGIN: ERROR Failed login attempt on user $username`. Data waktu dan informasi error login akan disimpan ke dalam file `log.txt.`
---
---
## Dokumentasi Output
## 1. Output script louis.sh
 - Register Success
 - Register Failed
## 2. Output script retep.sh
 - Login Success
 - Login Failed
## 3. Output file log.txt
## 4. Output file /users/users.txt
---
Kendala saat mengerjakan soal 3 yaitu masih belum terlalu familiar dengan syntax linux sehingga ada banyak error karena kesalahan menulis syntax khususnya di bagian date dan ada kendala ketika input password masih bisa menggunakan symbol meskipun sudah melakukan pengecekan apakah input memiliki huruf dan angka.

