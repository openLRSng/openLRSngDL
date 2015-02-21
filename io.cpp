#include "io.h"
#include <avr/io.h>

static volatile uint8_t *ddrs[3]  = {&DDRB,  &DDRC,  &DDRD};
static volatile uint8_t *ports[3] = {&PORTB, &PORTC, &PORTD};
static volatile uint8_t *pins[3]  = {&PINB,  &PINC,  &PIND};

void _pinMode(uint8_t pin, bool output)
{
  volatile uint8_t *reg = ddrs[pin>>4];
  if (output) {
    *reg |= (1<<(pin & 0x07));
  } else {
    *reg &= ~(1<<(pin & 0x07));
  }
}

void _digitalWrite(uint8_t pin, bool state)
{
  volatile uint8_t *reg = ports[pin>>4];
  if (state) {
    *reg |= (1<<(pin & 0x07));
  } else {
    *reg &= ~(1<<(pin & 0x07));
  }
}

bool _digitalRead(uint8_t pin)
{
  volatile uint8_t *reg = pins[pin>>4];
  return  (*reg & (1<<(pin & 0x07)));
}
