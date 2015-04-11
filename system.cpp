#include<avr/interrupt.h>
#include "system.h"
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
  // copy these to local variables so they can be stored in registers
  // (volatile variables must be read from memory on every access)
  unsigned long m = timer0_millis;
  unsigned char f = timer0_fract;

  m += MILLIS_INC;
  f += FRACT_INC;
  if (f >= FRACT_MAX) {
    f -= FRACT_MAX;
    m += 1;
  }

  timer0_fract = f;
  timer0_millis = m;
  timer0_overflow_count++;
}

unsigned long millis()
{
  unsigned long m;
  uint8_t oldSREG = SREG;
  cli();
  m = timer0_millis;
  SREG = oldSREG;
  return m;
}

unsigned long micros()
{
  unsigned long m;
  uint8_t oldSREG = SREG, t;

  cli();
  m = timer0_overflow_count;
  t = TCNT0;

  if ((TIFR0 & _BV(TOV0)) && (t < 255)) {
    m++;
  }

  SREG = oldSREG;

  return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void delay(unsigned long ms)
{
  uint16_t start = (uint16_t)micros();
  while (ms > 0) {
    if (((uint16_t)micros() - start) >= 1000) {
      ms--;
      start += 1000;
    }
  }
}

void init()
{
  sei();
  // Setup timer0 for micros/millis
  TCCR0A |= (1 << WGM01) | (1 << WGM00);
  TCCR0B |= (1 << CS01) |(1 << CS00);
  TIMSK0 |= (1 << TOIE0);

#ifdef ADC_ENABLED
  // Init ADC
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
#endif
  // release USART pins
#if defined(UCSR0B)
  UCSR0B = 0;
#endif
#if defined(UCSR1B)
  UCSR1B = 0;
#endif
}
