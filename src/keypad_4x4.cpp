#include  <keypad_4x4.hpp>

volatile uint8_t *KEYPAD_BUS, *KEYPAD_DIR, *KEYPAD_IN;
uint8_t KEYPAD_ROW, KEYPAD_COLUMN;
uint8_t keypad_interrupt;
uint8_t scanned_row, scanned_column;
int keypad_event_count = 0;
char current_input;
char keypad_buffer[17] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};
char LOOKUP_KEYPAD[4][4] = {
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

/*
Initialize keypad using PORT specified.
In this particular library, the pin mapping is as shown:

    Px0 - Px3 -> KEYPAD_ROW
    Px4 - Px7 -> KEYPAD_COLUMN

You are free to change the pin mapping as needed, but it's preferable
if you don't. Otherwise, if you really need to, adjust the scan_keypad() function
to suit your needs.
*/
void keypad_init(volatile uint8_t *kypd_bus,
                 volatile uint8_t *kypd_dir,
                 volatile uint8_t *kypd_in,
                 uint8_t interrupt_sense,
                 uint8_t interrupt_trig,
                 uint8_t row_pins,
                 uint8_t col_pins
)
{
    KEYPAD_BUS = kypd_bus;
    KEYPAD_DIR = kypd_dir;
    KEYPAD_IN = kypd_in; 
    KEYPAD_ROW = row_pins;
    KEYPAD_COLUMN = col_pins;

    GICR |= (1 << interrupt_sense);
    MCUCR |= interrupt_trig;
}

/*
Scan the keypad. After this function finishes,
the char array keypad_buffer[] will be updated.
This function by itself does not display it on the LCD,
it only scan-update the array.
You need to display it on the LCD to see the changes.

*NOTE: The buffer is RIGHT ALIGNED, meaning it behaves like
an 'old calculator' on the display LCD
*/
void scan_keypad(){
  uint8_t row_buf, col_buf;
  keypad_event_count++;

  // SCAN ROW ---------------------------------------------------------------
  // ubah baris menjadi input enable pull up, kolom = out, set all kolom LOW
  *KEYPAD_DIR = ~KEYPAD_ROW;
  *KEYPAD_BUS = KEYPAD_ROW;

  // baca kondisi row
  delayMicroseconds(16);
  row_buf = *KEYPAD_IN & KEYPAD_ROW;

  // interpretasi row mana yang ditekan
  switch(row_buf){
    case 0b1110:
      scanned_row = 0;
      break;
    case 0b1101:
      scanned_row = 1;
      break;
    case 0b1011:
      scanned_row = 2;
      break;
    case 0b0111:
      scanned_row = 3;
      break;
  }

  // SCAN COLUMN ------------------------------------------------------------
  // ubah kolom menjadi input enable pull up, baris = out, set all baris LOW
  *KEYPAD_DIR = ~KEYPAD_COLUMN;
  *KEYPAD_BUS = KEYPAD_COLUMN;

  // baca kondisi kolom
  delayMicroseconds(16);
  col_buf = (*KEYPAD_IN & KEYPAD_COLUMN) >> 4;

  // interpretasi kolom mana yang ditekan
  switch(col_buf){
    case 0b1110:
      scanned_column = 0;
      break;
    case 0b1101:
      scanned_column = 1;
      break;
    case 0b1011:
      scanned_column = 2;
      break;
    case 0b0111:
      scanned_column = 3;
      break;
  }

  int n;

  // get current input (berdasarkan lookup array)
  current_input = LOOKUP_KEYPAD[scanned_row][scanned_column];
   
  // shift buffer to left (agar seperti kalkulator)
  for(n = 0; n < 16; n++){
    keypad_buffer[n] = keypad_buffer[n + 1];
  }
   
  // assign current input to last index
  keypad_buffer[15] = current_input;
 
  // set ulang null-byte terminator, karena fungsi yg di definisikan di driver LCD
  // membutuhkan null-byte terminator pada fungsi lcd_string
  keypad_buffer[16] = '\0';
  
  // set semua menjadi output lagi, pull row high
  *KEYPAD_DIR = 0xFF;
  *KEYPAD_BUS = KEYPAD_ROW;
}

// clear buffer
void clear_keypad_buf(){

  int n;
  for(n = 0; n < 16; n++){
    keypad_buffer[n] = ' ';
  }

  // assign null byte terminator
  keypad_buffer[16] = '\0';

  // reset event count
  keypad_event_count = 0;
}