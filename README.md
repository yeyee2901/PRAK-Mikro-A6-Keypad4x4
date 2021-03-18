# PRAKTIKUM SISTEM MIKRO - A6 - Keypad 4x4

- IDE: PlatformIO on VSCode (https://platformio.org/)
- Device: Atmel AVR ATMega16
- CPU: Internal 1 MHz
- Programmer: USBasp

**Datasheets**
- LCD 16x2 HD44780 : https://www.alldatasheet.com/datasheet-pdf/pdf/63673/HITACHI/HD44780.html
- ATMega16 : https://www.alldatasheet.com/datasheet-pdf/pdf/78532/ATMEL/ATMEGA16.html

**Brief Explanations**

**PSEUDOCODE**

**FUNCTIONS**

**HOW TO USE THIS PROJECT (THE FAST WAY)**
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
