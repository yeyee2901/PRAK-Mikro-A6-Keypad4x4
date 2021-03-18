#ifndef KEYPAD_4x4
#define KEYPAD_4x4

#include <Arduino.h>

// GLOBAL VARIABLE ----------------------------------------
extern volatile uint8_t *KEYPAD_BUS, *KEYPAD_DIR, *KEYPAD_IN;
extern uint8_t KEYPAD_ROW, KEYPAD_COLUMN;
extern uint8_t keypad_interrupt;
extern uint8_t scanned_row, scanned_column;
extern char current_input, keypad_buffer[17], LOOKUP_KEYPAD[4][4];
extern int keypad_event_count;

void keypad_init(volatile uint8_t *kypd_bus,
                 volatile uint8_t *kypd_dir,
                 volatile uint8_t *kypd_in,
                 uint8_t interrupt_sense,
                 uint8_t interrupt_trig,
                 uint8_t row_pins,
                 uint8_t col_pins
);
void scan_keypad();
void clear_keypad_buf();

#endif