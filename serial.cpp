#include "serial.h"
#include <avr/interrupt.h>

#define FIFOSIZE 128

const char* serial_bits_str[] = {"8N1","8N2","8E1","8E2","8O1","8O2"};

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

bool txActive = false;

#if (__AVR_ATmega32U4__ == 1)
#define _USART_RX_vect USART1_RX_vect
#define _USART_UDRE_vect USART1_UDRE_vect
#define _UCSRxA UCSR1A
#define _UCSRxB UCSR1B
#define _UCSRxC UCSR1C
#define _UBRRxH UBRR1H
#define _UBRRxL UBRR1L
#define _U2Xx   U2X1
#define _UPEx   UPE1
#define _FEx    FE1
#define _DORx   DOR1
#define _UDRx   UDR1
#define _UDRIEx UDRIE1
#define _TXCx   TXC1
#define _RXENx  RXEN1
#define _TXENx  TXEN1
#define _RXCIEx RXCIE1
#define _UCSZx1 UCSZ11
#define _UCSZx0 UCSZ10
#define _USBSx  USBS1
#define _UPMx0  UPM10
#define _UPMx1  UPM11

#else
#define _USART_RX_vect USART_RX_vect
#define _USART_UDRE_vect USART_UDRE_vect
#define _UCSRxA UCSR0A
#define _UCSRxB UCSR0B
#define _UCSRxC UCSR0C
#define _UBRRxH UBRR0H
#define _UBRRxL UBRR0L
#define _U2Xx   U2X0
#define _UPEx   UPE0
#define _FEx    FE0
#define _DORx   DOR0
#define _UDRx   UDR0
#define _UDRIEx UDRIE0
#define _TXCx   TXC0
#define _RXENx  RXEN0
#define _TXENx  TXEN0
#define _RXCIEx RXCIE0
#define _UCSZx1 UCSZ01
#define _UCSZx0 UCSZ00
#define _USBSx  USBS0
#define _UPMx0  UPM00
#define _UPMx1  UPM01
#endif

ISR(_USART_RX_vect)
{
  if (!(_UCSRxA & ((1<<_UPEx) | (1<<_FEx)))) {
    if (_UCSRxA & (1<<_DORx)) {
      rxDropped++;
    }
    unsigned char c = _UDRx;
    uint8_t i = (rxFIFO.head + 1) % FIFOSIZE;
    if (i != rxFIFO.tail) {
      rxFIFO.buf[rxFIFO.head] = c;
      rxFIFO.head = i;
    } else {
      rxDropped++;
    }
  } else {
    _UDRx,rxErrors++;
  }
}

ISR(_USART_UDRE_vect)
{
  if (txFIFO.head != txFIFO.tail) {
    _UDRx = txFIFO.buf[txFIFO.tail];
    txFIFO.tail = (txFIFO.tail + 1) % FIFOSIZE;
  } else {
    _UCSRxB &= ~(1 << _UDRIEx);
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
  _UCSRxB |= (1 << _UDRIEx);
  txActive = true;
  _UCSRxA |= (1 << _TXCx);
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

void serialFlush()
{
  while (serialAvailable()) {
    serialRead();
  }
  while (txActive && !(_UCSRxA & (1<<_TXCx)));
  txActive = false;
}


// Ensure byte is placed on FIFO
void serialWriteSync(uint8_t c)
{
  while (serialWrite(c));
}

void serialInit(uint32_t baudrate, uint8_t bits)
{
  bool u2x = 1;
  uint16_t brd;

retry_with_u2x:
  brd = (F_CPU / (u2x?4:8) / baudrate - 1) / 2;
  if ((brd > 4095) && u2x) {
    u2x = 0;
    goto retry_with_u2x;
  }
  _UBRRxH = brd >> 8;
  _UBRRxL = brd & 0xff;

  _UCSRxA = (u2x ? (1<<_U2Xx) : 0);
  _UCSRxB = (1<<_RXCIEx) | (1<<_RXENx) | (1<<_TXENx);
  _UCSRxC = (1<<_UCSZx1) | (1<<_UCSZx0);

  // bits == 0=8N1 1=8N2 2=8E1 3=8E2 4=8O1 5=8O2, rest undef.
  if (bits & 1) { // 2 stop bit mode
    _UCSRxC |= _USBSx;
  }
  if (bits > 2) { //parity enabled
    _UCSRxC |= _UPMx1;
    if (bits > 4) { // odd parity
      _UCSRxC |= _UPMx0;
    }
  }
}

