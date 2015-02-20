#include "serial.h"
#include <avr/interrupt.h>

#define FIFOSIZE 128

struct fifo {
  uint8_t head;
  uint8_t tail;
  uint8_t buf[FIFOSIZE];
};

volatile struct fifo rxFIFO = {.head=0, .tail=0};
volatile struct fifo txFIFO = {.head=0, .tail=0};

volatile uint32_t rxDropped = 0;
volatile uint32_t txDropped = 0;
volatile uint32_t rxErrors = 0;

inline bool fifoWrite(volatile struct fifo *fifo, uint8_t d)
{
  uint8_t i = (fifo->head + 1) % FIFOSIZE;
  if (i != fifo->tail) {
    fifo->buf[fifo->head] = d;
    fifo->head = i;
    return false;
  } else {
    return true; // overrun
  }
}

inline bool fifoAvail(volatile struct fifo *fifo)
{
  return (fifo->head != fifo->tail);
}

inline uint8_t fifoRead(volatile struct fifo *fifo)
{
  uint8_t d = 0;
  if (fifoAvail(fifo)) {
    d = fifo->buf[fifo->tail];
    fifo->tail = (fifo->tail + 1) % FIFOSIZE;
  }
  return d;
}

ISR(USART_RX_vect)
{
  if (!(UCSR0A & ((1<<UPE0) | (1<<FE0)))) {
    if (UCSR0A & (1<<DOR0)) {
      rxDropped++;
    }
    unsigned char c = UDR0;
    if (fifoWrite(&rxFIFO,c)) {
      rxDropped++;
    }
  } else {
    UDR0,rxErrors++;
  }
}

ISR(USART_UDRE_vect)
{
  if (fifoAvail(&txFIFO)) {
    UDR0 = fifoRead(&txFIFO);
  } else {
    UCSR0B &= ~(1 << UDRIE0);
  }
}


// This may drop if FIFO is full, returns true if so
bool serialWrite(unsigned char  c)
{
  if (fifoWrite(&txFIFO,c)) {
    return 1;
  }
  UCSR0B |= (1 << UDRIE0);
  return 0;
}

bool serialAvailable()
{
  return fifoAvail(&rxFIFO);
}

uint8_t serialRead()
{
  return fifoRead(&rxFIFO);
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

