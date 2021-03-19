# PRAKTIKUM SISTEM MIKRO - A6 - Keypad 4x4

- IDE: PlatformIO on VSCode (https://platformio.org/)
- Device: Atmel AVR ATMega16
- CPU: Internal 1 MHz
- Programmer: USBasp

**Datasheets**
- LCD 16x2 HD44780 : https://www.alldatasheet.com/datasheet-pdf/pdf/63673/HITACHI/HD44780.html
- ATMega16 : https://www.alldatasheet.com/datasheet-pdf/pdf/78532/ATMEL/ATMEGA16.html

**Brief Explanations**

![image](https://user-images.githubusercontent.com/55247343/111785497-44564d80-88ef-11eb-9ceb-ec821ed2f0a2.png)

![image](https://user-images.githubusercontent.com/55247343/111785681-7e275400-88ef-11eb-9438-af2855bad14c.png)

Keypad merupakan salah satu jenis input yang sering digunakan dalam desain *embedded system*. Keypad juga tersedia dalam berbagai ukuran, namun umumnya yang digunakan adalah 3x4 dan 4x4. Keypad sebenarnya adalah matrix *push button*, dimana kaki-kaki dari button terhubung di satu titik sesuai baris / kolomnya. Misalnya button yang disusun di baris 1, setiap kaki kananya terhubung ke 1 titik yang sama, 1 titik inilah BARIS dari matrix button tersebut. Lalu, kaki kiri dari setiap button terhubung ke 4 titik yang berbeda, 4 titik yang berbeda inilah KOLOM dari matrix button tersebut, *thus* kita telah membuat keypad 1x4. Apabila kita *cascade* (dirangkai bersusun) sebanyak 4 buah, maka kita akan membuat sebuah keypad 4x4.

Pada keypad, pin-pin dari baris & kolom terhubung dengan sebuah *flexible wire* yang sangat tipis, sehingga anda harus berhati-hati dalam menggunakannya agar tidak robek. Kurang lebih *flexible wire* tersebut seperti plastik mika. Di *female header* dari *flexible wire* akan terdapat label pin / penomoran pinnya. Umumnya penomoran pinout dimulai dari kanan ke kiri, dengan skema berikut:
- Pin 1 - 4 = KOLOM
- Pin 5 - 8 = BARIS

Cara *interfacing* keypad bermacam-macam tergantung desainer *embedded system*-nya. Kita dapat melakukan *scanning* berulang-ulang di loop program, menggunakan *external interrupt* untuk mendeteksi penekanan tombol, dll. Pada project ini yang digunakan adalah metode *external interrupt* (menggunakan INTERRUPT-0).

![image](https://user-images.githubusercontent.com/55247343/111787349-681a9300-88f1-11eb-97df-a3fe3c9526f2.png)

**catatan**: pada skematik saya membalik penomoran pin-nya, sehingga BARIS adalah pin 1-4, KOLOM adalah pin 5-8, agar mempermudah pemrogramannya (karena format matrix umumya adalah (baris, kolom).

Untuk metode interrupt, pada kondisi awal PORT keypad akan dijadikan OUTPUT semua, lalu PORT yang terhubung ke BARIS akan dibuat HIGH, sedangkan yang KOLOM dibuat LOW. Semua BARIS terhubung ke 1 titik yang sama melalui sebuah Diode 1N4148 (atau diode lain bisa, **dengan syarat reverse breakdown voltage-nya >= 5V**), ini untuk memastikan agar arus tidak mengalir ke pin INT0, melainkan hanya mengalir antara BARIS-KOLOM saja. Dan resistor 10k adalah untuk membatasi arus yang mengalir antara BARIS-KOLOM agar tidak melebihi *max current* MCU (ATMega16 max 40mA per pin).

Pada mulanya, BARIS dalam kondisi HIGH semua. ketika kita menekan salah satu tombol keypad, maka salah satu BARIS akan tertarik menjadi LOW, tergantung BARIS mana yang kita tekan. Akibat dari perubahan kondisi ke LOW ini, maka pada titik temu diode (yang terhubung ke pin INT0) akan menjadi LOW juga, *thus* akan memicu external interrupt INT0 (karena INT0 di set ke trigger on *falling edge*). Misalnya, jika kita menekan tombol di BARIS ke-0 keypad, maka pin PC0 akan menjadi LOW, titik temu INT0 menjadi LOW juga, namun **PIN BARIS LAIN TETAP HIGH**, hal ini karena diode-nya mengisolasi antar pin BARIS dengan memblokir arus. Apabila kita tidak menggunakan diode, maka apabila kita misalnya menekan tombol di BARIS ke-0 keypad, pin PC0 akan menjadi LOW, namun **PIN LAIN AKAN IKUT MENJADI LOW JUGA**. Hal ini karena pin baris itu bagaikan di parallel satu sama lain, bertemu di satu titik temu, yaitu di INT0. Ingat, bahwa tegangan di rangkaian parallel adalah sama. Apabila seperti ini, maka kita tidak akan bisa membaca kondisi logika BARIS & KOLOM dengan benar saat **scanning keypad** nantinya.

Ketika INT0 terpicu, maka pada ISR akan di set sebuah flag yang menandakan bahwa keypad telah ditekan (sebut saja *keypad_sense_flag*). Pada loop program, akan di cek flag tersebut, apabila flag tersebut di set oleh ISR INT0, maka keypad telah ditekan dan itu memerintahkan MCU bahwa kita harus melakukan **scanning keypad** untuk mencari tahu tombol mana yang ditekan (masuk ke subroutine).

**PSEUDOCODE SUBROUTINE SCANNING KEYPAD**
1. Increment keypad_event_count
2. Scan baris:
   1. Set port baris menjadi input dengan pull up internal
   2. Set port kolom menjadi output, set kondisi ke all LOW
   3. Baca kondisi baris, simpan hasil pembacaan di buffer baris
   4. Interpretasikan buffer yang didapat (menggunakan switch case) untuk mendapatkan nomor baris
3. Scan kolom:
   1. Set port kolom menjadi input dengan pull up internal
   2. Set port baris menjadi output, set kondisi ke all LOW
   3. Baca kondisi kolom, simpan hasil pembacaan di buffer kolom
   4. Interpretasikan buffer yang didapat (menggunakan switch case) untuk mendapatkan nomor kolom
4. hasil lookup = lookup[ nomor baris ] [ nomor kolom ]
5. Geser semua elemen pada buffer keypad ke kiri 1x
6. Set elemen terakhir (ke-15) = hasil lookup
7. Set elemen ke-16 menjadi NULL-byte terminator (dibutuhkan, karena pada fungsi lcd_string() kondisi loop adalah pointer char-nya menunjuk ke NULL-byte string terminator)
8. Set port keypad sebagai output semua
9. Set pin-pin baris menjadi HIGH semua, kolom menjadi LOW semua.

**FUNCTIONS**
1. **keypad_init()** : Inisialisasi keypad 4x4 sesuai port yang ingin digunakan
    1. *kypd_bus = ADDRESS dari register PORTx (harus port 8-bit) yang digunakan untuk keypad
    2. *kypd_dir = ADDRESS dari register DDRx port yang digunakan
    3. *kypd_in = ADDRESS dari register PINx yang digunakan. Register ini menampung kondisi PORTx, sehingga digunakan untuk membaca kondisi BARIS & KOLOM
    4. interrupt_sense = interrupt eksternal mana yang mau digunakan (INT0 atau INT1)
    5. interrupt_trig = trigger interrupt (saran: LOW)
    6. row_pins = pin BARIS keypad diletakkan di pin berapa (saran 0x0F, atau Px0 - Px3)
    7. col_pins = pin KOLOM keypad diletakkan di pin berapa (saran 0xF0, atau Px4 - Px7)
```cpp
void keypad_init(volatile uint8_t *kypd_bus,
                 volatile uint8_t *kypd_dir,
                 volatile uint8_t *kypd_in,
                 uint8_t interrupt_sense,
                 uint8_t interrupt_trig,
                 uint8_t row_pins,
                 uint8_t col_pins
);

// Ex:
uint8_t interrupt_trig = (1 << ISC01);  // falling edge mode (see datasheet)
keypad_init(&PORTC, &DDRC, &PINC, INT0, interrupt_trig, 0x0F, 0xF0)
```
2. **clear_keypad_buf()**: clear buffer berisi history input keypad.
3. **scan_keypad()** : Fungsi ini akan menjalankan scanning keypad persis seperti di PSEUDOCODE diatas. Setelah fungsi ini ini dijalankan, maka buffer_keypad akan di update. Namun, fungsi ini tidak men-display, sehingga anda harus menampilkan isi keypad_buffer secara manual setelah memanggil fungsi ini. Contohnya menggunakan lcd_string() dari **driver_LCD16x2**
```cpp
// Ex in main program loop:
if(keypad_sense_flag){
    scan_keypad();
    lcd_setpos(1,0);    // tampilkan di baris bawah LCD 16x2
    lcd_string(keypad_buffer);
    delay(500); // button debouncing, bouncing dapat menyebabkan scan_keypad()
                // terpanggil berkali-kali padahal hanya ditekan 1x
    
    keypad_sense_flag = 0;
}
```

**HOW TO TRY THIS PROJECT (THE FAST WAY)**
1. Download Visual Studio Code
2. Download PlatformIO extension (dari VSCode extension tab)
3. Clone repo ini / download repo dalam bentuk .zip & unzip di tempat yang mudah dicari
4. Di VSCode: buka PlatformIO - Home tab
5. Pilih "Open Project"
6. Cari repo yang sudah di clone / yang sudah di unzip, pilih dan Open folder berisi project-nya.
7. Sekarang anda sudah berada di environment project-nya, untuk Build project ini, bisa salah satu:
    - Klik simbol "centang" di bagian kiri bawah VSCode
    - Tekan Ctrl + Shift + B , pilih "PlatformIO: Build"
9. Jika anda mengikuti langkah-langkah diatas, seharusnya project akan build tanpa masalah (sudah diuji)
10. Untuk upload firmware-nya ke board / mikrokontroler, harus edit file "platformio.ini" dahulu.
11. Ubah "board" ID agar sesuai ke board yang anda punya (lihat: https://docs.platformio.org/en/latest/platforms/atmelavr.html untuk daftar board ID)
12. Ubah upload protocol agar sesuai USB programmer yang anda miliki (lihat: https://docs.platformio.org/en/latest/platforms/atmelavr.html untuk info lebih lanjut, author menggunakan USB-ASP)

**SIDE NOTE**
- Tidak disarankan anda build project ini diluar penggunaan PlatformIO, kecuali anda benar-benar memahami proses dalam embedded development, serta mempunyai dan dapat menggunakan toolchain untuk build project ini dari awal (biasanya menggunakan avr-g++ / avr-gcc)
- Jika anda ingin build manual, copas saja code dari main.cpp , driver_LCD16x2.cpp , driver_LCD16x2.hpp ke IDE yang anda gunakan dan silahkan setup sendiri sesuai IDE masing-masing
- Bila anda masih menggunakan Arduino IDE untuk embedded development, author sarankan untuk pindah ke PlatformIO saja karena PlatformIO support banyak varian board tidak hanya Arduino family saja. Ada mulai dari ARM STM32, Atmel AVR (pure), TI - MSP430, dan masih banyak lagi (>900 board mikrokontroler). 
- Selain karena support board-nya yang banyak, IDE ini juga ramah (code completion, pre-compile syntax check, dll) karena terintegrasi ke VSCode. Ada juga fitur version control menggunakan git yang terintegrasi pada VSCode nya

**FOR PROTEUS SIMULATOR USERS**
Apabila anda ingin mencoba simulasi pada Proteus, anda bisa melakukannya. Tetap sama seperti biasa, anda harus build project ini.
1. Pergi ke Folder Project-nya
2. Pergi ke folder .pio/build/ATMega16, di dalam folder ini terdapat file bernama **firmware.hex**, copy path dari file tersebut
3. Buka Proteus dan buat skematik seperti skematik yang disertakan pada Folder Project ini.
4. Klik 2x pada MCU-nya, anda akan mendapati tampilan properties dari MCU, kurang lebih seperti dibawah ini (sumber: google image)

![image](https://user-images.githubusercontent.com/55247343/111795758-10ccf080-88fa-11eb-9bd1-75a09c1d1b1e.png)

5. Atur clock frequency agar menjadi Internal RC 1MHz (pada bagian CKSEL), karena kita menggunakan internal CPU clock, tidak menggunakan oscillator external
6. Lalu pada bagian Program File, paste path dari file **firmware.hex** tadi. Atau anda juga dapat memilihnya secara manual dengan meng-klik icon folder di sampingnya
7. Klik tombol play untuk memulai simulasi

**Catatan keras**: Proteus +/= real hardware, karena delay max proteus juga terbatas. Banyak kasus dari praktikan lain yang mengalami output berbeda dengan ekspektasi bila program dijalankan di real hardware-nya
