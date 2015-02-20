#include "serial.h"
#include <avr/interrupt.h>

#define FIFOSIZE 128

struct fifo {
  uint8_t head;
  uint8_t tail;
  uint8_t buf[FIFOSIZE];
};

struct fifo rxFIFO = {.head=0, .tail=0};
struct fifo txFIFO = {.head=0, .tail=0};

uint32_t rxDropped = 0;
uint32_t txDropped = 0;
uint32_t rxErrors = 0;

ISR(USART_RX_vect)
{
  if (!(UCSR0A & ((1<<UPE0) | (1<<FE0)))) {
    if (UCSR0A & (1<<DOR0)) {
      rxDropped++;
    }
    unsigned char c = UDR0;
    uint8_t i = (rxFIFO.head + 1) % FIFOSIZE;
    if (i != rxFIFO.tail) {
      rxFIFO.buf[rxFIFO.head] = c;
      rxFIFO.head = i;
    } else {
      rxDropped++;
    }
  } else {
    UDR0,rxErrors++;
  }
}

ISR(USART_UDRE_vect)
{
  if (txFIFO.head != txFIFO.tail) {
    UDR0 = txFIFO.buf[txFIFO.tail];
    txFIFO.tail = (txFIFO.tail + 1) % FIFOSIZE;
  } else {
    UCSR0B &= ~(1 << UDRIE0);
  }
}


// This may drop if FIFO is full, returns true if so
bool serialWrite(unsigned char  c)
{
  uint8_t i = (txFIFO.head + 1) % FIFOSIZE;
  if (i != txFIFO.tail) {
    txFIFO.buf[txFIFO.head] = c;
    txFIFO.head = i;
  } else {
    return true; // overrun
  }
  UCSR0B |= (1 << UDRIE0);
  return 0;
}

bool serialAvailable()
{
  return (rxFIFO.head != rxFIFO.tail);
}

uint8_t serialRead()
{
  uint8_t d = 0;
  if (rxFIFO.head != rxFIFO.tail) {
    d = rxFIFO.buf[rxFIFO.tail];
    rxFIFO.tail = (rxFIFO.tail + 1) % FIFOSIZE;
  }
  return d;
}

// Ensure byte is placed on FIFO
void serialWriteSync(uint8_t c)
{
  while (serialWrite(c));
}

void serialInit(uint32_t baudrate)
{
  bool u2x = 1;
  uint16_t brd;

retry_with_u2x:
  brd = (F_CPU / (u2x?4:8) / baudrate - 1) / 2;
  if ((brd > 4095) && u2x) {
    u2x = 0;
    goto retry_with_u2x;
  }
  UBRR0H = brd >> 8;
  UBRR0L = brd & 0xff;

  UCSR0A = (u2x?(1<<U2X0):0);
  UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

}

