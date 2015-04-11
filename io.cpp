#include "io.h"
#include <avr/io.h>

#if defined(PIN_PF0)
static volatile uint8_t *ddrs[]  = {&DDRB,  &DDRC,  &DDRD,  &DDRE,  &DDRE};
static volatile uint8_t *ports[] = {&PORTB, &PORTC, &PORTD, &PORTE, &PORTF};
static volatile uint8_t *pins[]  = {&PINB,  &PINC,  &PIND,  &PINE,  &PINF};
#elif defined(PIN_PE0)
static volatile uint8_t *ddrs[]  = {&DDRB,  &DDRC,  &DDRD,  &DDRE};
static volatile uint8_t *ports[] = {&PORTB, &PORTC, &PORTD, &PORTE};
static volatile uint8_t *pins[]  = {&PINB,  &PINC,  &PIND,  &PINE};
#else
static volatile uint8_t *ddrs[]  = {&DDRB,  &DDRC,  &DDRD};
static volatile uint8_t *ports[] = {&PORTB, &PORTC, &PORTD};
static volatile uint8_t *pins[]  = {&PINB,  &PINC,  &PIND};
#endif

void pinMode(uint8_t pin, bool output)
{
  volatile uint8_t *reg = ddrs[pin>>4];
  if (output) {
    *reg |= (1<<(pin & 0x07));
  } else {
    *reg &= ~(1<<(pin & 0x07));
  }
}

void digitalWrite(uint8_t pin, bool state)
{
  volatile uint8_t *reg = ports[pin>>4];
  if (state) {
    *reg |= (1<<(pin & 0x07));
  } else {
    *reg &= ~(1<<(pin & 0x07));
  }
}

bool digitalRead(uint8_t pin)
{
  volatile uint8_t *reg = pins[pin>>4];
  return  (*reg & (1<<(pin & 0x07)));
}
