#include <Arduino.h>
#include <driver_LCD16x2.hpp>
#include <keypad_4x4.hpp>

// GLOBAL VARIABLE ------------------------------------------
volatile uint8_t  keypad_sense_flag = 0,
                  buf_clear_flag = 0;
uint8_t kypd_row = 0x0F,
        kypd_col = 0xF0;

// MAIN PROGRAM ----------------------------------------------
void setup() {
  
  // Initialize LCD
  lcd_init(&PORTA, &PORTD, &DDRA, &DDRD, PD0, PD1);
  delay(10);
  
  // Initialize keypad
  // - using PORTC
  // - interrupt INT0, falling edge trigger
  uint8_t keypad_trigger = (1 << ISC01);
  keypad_init(&PORTC, &DDRC, &PINC, INT0, keypad_trigger, 0x0F, 0xF0);

  // Initialize External Interrupt
  // INT0, falling edge mode (untuk sense keypad ditekan)
  // INT1, falling edge mode (untuk clear keypad_buffer)
  GICR |= (1 << INT1);
  MCUCR |= (1 << ISC11);

  clear_keypad_buf();
  lcd_setpos(0,0);
  lcd_string("KEYPAD  SCANNING");
  lcd_setpos(1,0);
  lcd_string(keypad_buffer);
}

void loop() {

  // clear buffer jika buffer keypad sudah penuh
  if((buf_clear_flag == 1) || (keypad_event_count >= 16) ){
    // show simple message
    lcd_setpos(0,0);
    lcd_string("Clearing Buffer!");
    lcd_setpos(1,0);
    lcd_string("                ");

    clear_keypad_buf();

    delay(2000);

    lcd_setpos(0,0);
    lcd_string("KEYPAD  SCANNING");

    buf_clear_flag = 0; 
  }

  // jika keypad sense flag = 1, lakukan scan
  if(keypad_sense_flag == 1){
    scan_keypad();
 
    // update LCD
    lcd_setpos(1,0);
    lcd_string(keypad_buffer);

    // update flag
    keypad_sense_flag = 0;
    delay(500); // prevent debounce
  }

}

// ISR ------------------------------------------------------
ISR(INT1_vect){
  buf_clear_flag = 1;
}

ISR(INT0_vect){
  keypad_sense_flag = 1;
}