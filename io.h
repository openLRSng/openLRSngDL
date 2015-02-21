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

void _pinMode(uint8_t pin, bool output);
void _digitalWrite(uint8_t pin, bool state);
bool _digitalRead(uint8_t pin);

#endif
