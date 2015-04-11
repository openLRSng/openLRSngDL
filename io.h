#ifndef IO_H
#define IO_H
#include <stdint.h>
#define PIN_PB0 0x00
#define PIN_PB1 0x01
#define PIN_PB2 0x02
#define PIN_PB3 0x03
#define PIN_PB4 0x04
#define PIN_PB5 0x05
#define PIN_PB6 0x06
#define PIN_PB7 0x07
#define PIN_PC0 0x10
#define PIN_PC1 0x11
#define PIN_PC2 0x12
#define PIN_PC3 0x13
#define PIN_PC4 0x14
#define PIN_PC5 0x15
#define PIN_PC6 0x16
#define PIN_PC7 0x17
#define PIN_PD0 0x20
#define PIN_PD1 0x21
#define PIN_PD2 0x22
#define PIN_PD3 0x23
#define PIN_PD4 0x24
#define PIN_PD5 0x25
#define PIN_PD6 0x26
#define PIN_PD7 0x27
#if (__AVR_ATmega32U4__ == 1)
#define PIN_PE0 0x30
#define PIN_PE1 0x31
#define PIN_PE2 0x32
#define PIN_PE3 0x33
#define PIN_PE4 0x34
#define PIN_PE5 0x35
#define PIN_PE6 0x36
#define PIN_PE7 0x37
#define PIN_PF0 0x30
#define PIN_PF1 0x31
#define PIN_PF2 0x32
#define PIN_PF3 0x33
#define PIN_PF4 0x34
#define PIN_PF5 0x35
#define PIN_PF6 0x36
#define PIN_PF7 0x37
#endif

#define INPUT  0
#define OUTPUT 1

#define LOW  0
#define HIGH 1

void pinMode(uint8_t pin, bool output);
void digitalWrite(uint8_t pin, bool state);
bool digitalRead(uint8_t pin);

#endif
