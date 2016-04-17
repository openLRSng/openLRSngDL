// Generic definitions needed always

#define Available 0
#define Transmit 1
#define Transmitted 2
#define Receive 3
#define Received 4

volatile uint8_t RF_Mode = 0;

//####### Board Pinouts #########

#if (BOARD_TYPE == 0) // Flytron M1 TX
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT A5
// #define RF_OUT_INDICATOR A4
#define BUZZER_ACT       9
#define BTN              10

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    digitalWrite(BUZZER_ACT,HIGH);
  } else {
    digitalWrite(BUZZER_ACT,LOW);
  }
}

#define buzzerOff(foo) buzzerOn(0)

#define Red_LED    12
#define Green_LED  11

#define Red_LED_ON   PORTB |= _BV(4);
#define Red_LED_OFF  PORTB &= ~_BV(4);

#define Green_LED_ON   PORTB |= _BV(3);
#define Green_LED_OFF  PORTB &= ~_BV(3);


//## RFM22B Pinouts for Public Edition (M1 or Rx v1)
#define  nIRQ_1 (PIND & _BV(3))
#define  nIRQ_0 (!(PIND & _BV(3)))

#define  nSEL_on  PORTD |= _BV(4)
#define  nSEL_off PORTD &= ~_BV(4)

#define  SCK_on  PORTD |= _BV(2)
#define  SCK_off PORTD &= ~_BV(2)

#define  SDI_on  PORTC |= _BV(1)
#define  SDI_off PORTC &= ~_BV(1)

#define  SDO_1   (PINC & _BV(0))
#define  SDO_0   (!(PINC & _BV(0)))

#define SDO_pin  PIN_PC0
#define SDI_pin  PIN_PC1
#define SCLK_pin PIN_PD2
#define IRQ_pin  PIN_PD3
#define nSel_pin PIN_PD4

void setupSPI()
{
  pinMode(SDO_pin,  INPUT);
  pinMode(SDI_pin,  OUTPUT);
  pinMode(SCLK_pin, OUTPUT);
  pinMode(IRQ_pin,  INPUT);
  pinMode(nSel_pin, OUTPUT);
}

#define IRQ_interrupt 1
void setupRfmInterrupt()
{
  attachInterrupt(IRQ_interrupt, RFM22B_Int, FALLING);
}

#endif

#if (BOARD_TYPE == 1) // Flytron M1 RX
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#if (COMPILE_TX != 1)
#error M1 RX not verified yet
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT PIN_PD5
#define BUZZER_ACT   PIN_PD7
#define BTN          PIN_PB0

#define Red_LED      PIN_PC3
#define Green_LED    PIN_PC2

#define Red_LED_ON   PORTC &= ~_BV(2);PORTC |= _BV(3);
#define Red_LED_OFF  PORTC &= ~_BV(2);PORTC &= ~_BV(3);

#define Green_LED_ON   PORTC &= ~_BV(3);PORTC |= _BV(2);
#define Green_LED_OFF  PORTC &= ~_BV(3);PORTC &= ~_BV(2);

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    digitalWrite(BUZZER_ACT,HIGH);
  } else {
    digitalWrite(BUZZER_ACT,LOW);
  }
}

#define buzzerOff(foo) buzzerOn(0)

//## RFM22B Pinouts for Public Edition (M1 or Rx v1)
#define  nIRQ_1 (PIND & _BV(3))
#define  nIRQ_0 (!(PIND & _BV(3)))

#define  nSEL_on  PORTD |= _BV(4) //D4
#define  nSEL_off PORTD &= ~_BV(4)

#define  SCK_on  PORTD |= _BV(2)
#define  SCK_off PORTD &= ~_BV(2)

#define  SDI_on  PORTC |= _BV(1)
#define  SDI_off PORTC &= ~_BV(1)

#define  SDO_1 (PINC & _BV(0))
#define  SDO_0 (!(PINC & _BV(0)))

#define SDO_pin  PIN_PC0
#define SDI_pin  PIN_PC1
#define SCLK_pin PIN_PD2
#define IRQ_pin  PIN_PD3
#define nSel_pin PIN_PD4

void setupSPI()
{
  pinMode(SDO_pin,  INPUT);
  pinMode(SDI_pin,  OUTPUT);
  pinMode(SCLK_pin, OUTPUT);
  pinMode(IRQ_pin,  INPUT);
  pinMode(nSel_pin, OUTPUT);
}

#define IRQ_interrupt 0
void setupRfmInterrupt()
{
  attachInterrupt(IRQ_interrupt, RFM22B_Int, FALLING);
}

#endif

#if (BOARD_TYPE == 2)
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define SLAVE_SELECT     PIN_PD3
// #define RF_OUT_INDICATOR PIN_PC0
#define BUZZER_ACT       PIN_PB2
#define BTN              PIN_PB3
// #define TX_AIN0          PIN_PC4 // SDA
// #define TX_AIN1          PIN_PC5 // SCL

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    digitalWrite(BUZZER_ACT,HIGH);
  } else {
    digitalWrite(BUZZER_ACT,LOW);
  }
}

#define buzzerOff(foo) buzzerOn(0)

#define TelemetrySerial Serial

#define Red_LED    PIN_PB5
#define Green_LED  PIN_PB4

#define Red_LED_ON   PORTB |= _BV(5);
#define Red_LED_OFF  PORTB &= ~_BV(5);

#define Green_LED_ON   PORTB |= _BV(4);
#define Green_LED_OFF  PORTB &= ~_BV(4);

//## RFM22B Pinouts for Public Edition (M2)
#define  nIRQ_1 (PIND & _BV(2))
#define  nIRQ_0 (!(PIND & _BV(2)))

#define  nSEL_on  PORTD |= _BV(4)
#define  nSEL_off PORTD &= ~_BV(4)

#define  SCK_on  PORTD |= _BV(7)
#define  SCK_off PORTD &= ~_BV(7)

#define  SDI_on  PORTB |= _BV(0)
#define  SDI_off PORTB &= ~_BV(0)

#define  SDO_1 (PINB & _BV(1))
#define  SDO_0 (!(PINB & _BV(1)))

#define SDO_pin  PIN_PB1
#define SDI_pin  PIN_PB0
#define SCLK_pin PIN_PD7
#define IRQ_pin  PIN_PD0
#define nSel_pin PIN_PD4

void setupSPI()
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);   //SDI
  pinMode(SCLK_pin, OUTPUT);   //SCLK
  pinMode(IRQ_pin, INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
  EICRA &= ~(1<<ISC00);
  EICRA |= (1<<ISC01);
  EIMSK |= (1<<INT0);
}

ISR(INT0_vect)
{
  if (RF_Mode == Transmit) {
    RF_Mode = Transmitted;
  }

  if (RF_Mode == Receive) {
    RF_Mode = Received;
  }
}

#endif

#if (BOARD_TYPE == 3)
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT PIN_PB3

//#define TX_AIN0 PIN_PD4 // SDA
//#define TX_AIN1 PIN_PD5 // SCL

#define BUZZER_ACT PIN_PD6
#define BUZZER_PAS PIN_PD3
#define BTN PIN_PB4

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
  TCCR2A = (1<<WGM21); // mode=CTC
#if (F_CPU == 16000000)
  TCCR2B = (1<<CS22) | (1<<CS20); // prescaler = 128
#elif (F_CPU == 8000000)
  TCCR2B = (1<<CS22); // prescaler = 64
#else
#errror F_CPU Invalid
#endif
  pinMode(BUZZER_PAS, OUTPUT);
  digitalWrite(BUZZER_PAS, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    digitalWrite(BUZZER_ACT,HIGH);
    uint32_t ocr = 125000L / freq;
    if (ocr>255) {
      ocr=255;
    }
    if (!ocr) {
      ocr=1;
    }
    OCR2A = ocr;
    TCCR2A |= (1<<COM2B0); // enable output on buzzer2
  } else {
    digitalWrite(BUZZER_ACT,LOW);
    TCCR2A &= ~(1<<COM2B0); // disable output buzzer2
  }
}

#define buzzerOff(foo) buzzerOn(0)

#define Red_LED    PIN_PC3
#define Green_LED  PIN_PB5

#define Red_LED2   PIN_PB1
#define Green_LED2 PIN_PB2
#define Red_LED_ON  { PORTC |= _BV(3); PORTB |= _BV(1); }
#define Red_LED_OFF { PORTC &= ~_BV(3); PORTB &= ~_BV(1); }
#define Green_LED_ON  PORTB |= (_BV(5) | _BV(2));
#define Green_LED_OFF PORTB &= ~(_BV(5) | _BV(2));

//## RFM22B Pinouts for Public Edition (Rx v2)
#define  nIRQ_1 (PIND & 0x04)==0x04 //D2
#define  nIRQ_0 (PIND & 0x04)==0x00 //D2

#define  nSEL_on PORTD |= (1<<4) //D4
#define  nSEL_off PORTD &= 0xEF //D4

#define  SCK_on PORTC |= (1<<2) //A2
#define  SCK_off PORTC &= 0xFB //A2

#define  SDI_on PORTC |= (1<<1) //A1
#define  SDI_off PORTC &= 0xFD //A1

#define  SDO_1 (PINC & 0x01) == 0x01 //A0
#define  SDO_0 (PINC & 0x01) == 0x00 //A0

#define SDO_pin  PIN_PC0
#define SDI_pin  PIN_PC1
#define SCLK_pin PIN_PC2
#define IRQ_pin  PIN_PD2
#define nSel_pin PIN_PD4

void setupSPI()
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);   //SDI
  pinMode(SCLK_pin, OUTPUT);   //SCLK
  pinMode(IRQ_pin, INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
  EICRA &= ~(1<<ISC00);
  EICRA |= (1<<ISC01);
  EIMSK |= (1<<INT0);
}

ISR(INT0_vect)
{
  if (RF_Mode == Transmit) {
    RF_Mode = Transmitted;
  }

  if (RF_Mode == Receive) {
    RF_Mode = Received;
  }
}

#endif

#if (BOARD_TYPE == 4) // kha:s openLRSngTX & clones
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT PIN_PB0 // ICP1

//#define TX_AIN0 A4 // SDA
//#define TX_AIN1 A5 // SCL

//#define TX_MODE1 A1
//#define TX_MODE2 A2

#define BUZZER_PAS PIN_PD3 // OCR2B
#define BTN PIN_PC0
#define Red_LED PIN_PD6
#define Green_LED PIN_PD5

//#define RF_OUT_INDICATOR A3 // only used for Futaba

void buzzerInit()
{
  TCCR2A = (1<<WGM21); // mode=CTC
#if (F_CPU == 16000000)
  TCCR2B = (1<<CS22) | (1<<CS20); // prescaler = 128
#elif (F_CPU == 8000000)
  TCCR2B = (1<<CS22); // prescaler = 64
#else
#errror F_CPU Invalid
#endif
  pinMode(BUZZER_PAS, OUTPUT);
  digitalWrite(BUZZER_PAS, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    uint32_t ocr = 125000L / freq;
    if (ocr>255) {
      ocr=255;
    }
    if (!ocr) {
      ocr=1;
    }
    OCR2A = ocr;
    TCCR2A |= (1<<COM2B0); // enable output
  } else {
    TCCR2A &= ~(1<<COM2B0); // disable output
  }
}

#define buzzerOff(foo) buzzerOn(0)

#define Red_LED_ON   PORTD |= _BV(6);
#define Red_LED_OFF  PORTD &= ~_BV(6);

#define Green_LED_ON   PORTD |= _BV(5);
#define Green_LED_OFF  PORTD &= ~_BV(5);

// #define RF_OUT_INDICATOR A3 // only used for Futaba

//## RFM22B Pinouts for Public Edition (M2)
#define  nIRQ_1 (PIND & _BV(2))
#define  nIRQ_0 (!(PIND & _BV(2)))

#define  nSEL_on  PORTD |= _BV(4)
#define  nSEL_off PORTD &= ~_BV(4)

#define  SCK_on  PORTB |= _BV(5)
#define  SCK_off PORTB &= ~_BV(5)

#define  SDI_on  PORTB |= _BV(3)
#define  SDI_off PORTB &= ~_BV(3)

#define  SDO_1 (PINB & _BV(4))
#define  SDO_0 (!(PINB & _BV(4)))

#define SDO_pin PIN_PB4
#define SDI_pin PIN_PB3
#define SCLK_pin PIN_PB5
#define IRQ_pin PIN_PD2
#define nSel_pin PIN_PD4
#define SDN_pin PIN_PB1

void setupSPI()
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);   //SDI
  pinMode(SCLK_pin, OUTPUT);   //SCLK
  pinMode(IRQ_pin, INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
  EICRA &= ~(1<<ISC00);
  EICRA |= (1<<ISC01);
  EIMSK |= (1<<INT0);
}

ISR(INT0_vect)
{
  if (RF_Mode == Transmit) {
    RF_Mode = Transmitted;
  }

  if (RF_Mode == Receive) {
    RF_Mode = Received;
  }
}
#define SWAP_GPIOS

#endif

#if (BOARD_TYPE == 5) // openLRSngRX-4ch
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT PIN_PB0 // ICP1

//#define TX_AIN0 PIN_PC4 // SDA
//#define TX_AIN1 PIN_PC5 // SCL

#define BUZZER_PAS  PIN_PD3  // OCR2B
#define BUZZER_ACT  PIN_PC5
#define BTN         PIN_PC4

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
  TCCR2A = (1<<WGM21); // mode=CTC
#if (F_CPU == 16000000)
  TCCR2B = (1<<CS22) | (1<<CS20); // prescaler = 128
#elif (F_CPU == 8000000)
  TCCR2B = (1<<CS22); // prescaler = 64
#else
#errror F_CPU Invalid
#endif
  pinMode(BUZZER_PAS, OUTPUT);
  digitalWrite(BUZZER_PAS, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    uint32_t ocr = 125000L / freq;
    digitalWrite(BUZZER_ACT,HIGH);
    if (ocr>255) {
      ocr=255;
    }
    if (!ocr) {
      ocr=1;
    }
    OCR2A = ocr;
    TCCR2A |= (1<<COM2B0); // enable output
  } else {
    digitalWrite(BUZZER_ACT,LOW);
    TCCR2A &= ~(1<<COM2B0); // disable output
  }
}

#define Red_LED   PIN_PD6
#define Green_LED PIN_PD5

#define Red_LED2   PIN_PC0
#define Green_LED2 PIN_PC1
#define Red_LED_ON    { PORTD |=  _BV(6); PORTC |=  _BV(0); }
#define Red_LED_OFF   { PORTD &= ~_BV(6); PORTC &= ~_BV(0); }
#define Green_LED_ON  { PORTD |=  _BV(5); PORTC |=  _BV(1); }
#define Green_LED_OFF { PORTD &= ~_BV(5); PORTC &= ~_BV(1); }

#define buzzerOff(foo) buzzerOn(0)

//## RFM22B Pinouts for Public Edition (M2)
#define  nIRQ_1 (PIND & _BV(2))
#define  nIRQ_0 (!(PIND & _BV(2))) //D2

#define  nSEL_on  PORTD |= _BV(4) //D4
#define  nSEL_off PORTD &= ~_BV(4)

#define  SCK_on  PORTB |= _BV(5)
#define  SCK_off PORTB &= ~_BV(5)

#define  SDI_on  PORTB |= _BV(3)
#define  SDI_off PORTB &= ~_BV(3)

#define  SDO_1 (PINB & _BV(4))
#define  SDO_0 (!(PINB & _BV(4)))

#define SDO_pin  PIN_PB4
#define SDI_pin  PIN_PB3
#define SCLK_pin PIN_PB5
#define IRQ_pin  PIN_PD2
#define nSel_pin PIN_PD4

void setupSPI()
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);   //SDI
  pinMode(SCLK_pin, OUTPUT);   //SCLK
  pinMode(IRQ_pin, INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
  EICRA &= ~(1<<ISC00);
  EICRA |= (1<<ISC01);
  EIMSK |= (1<<INT0);
}

ISR(INT0_vect)
{
  if (RF_Mode == Transmit) {
    RF_Mode = Transmitted;
  }

  if (RF_Mode == Receive) {
    RF_Mode = Received;
  }
}
#endif

#if (BOARD_TYPE == 6) // DTF UHF DeluxeTX
#if (__AVR_ATmega32U4__ != 1)
#error Wrong board selected, select Arduino Leonardo
#endif

#define TelemetrySerial Serial1

#define SLAVE_SELECT PIN_PD4 // ICP1

//#define TX_AIN_IS_DIGITAL
//#define TX_AIN0 PIN_PD1 // SDA
//#define TX_AIN1 PIN_PD0 // SCL

#define BUZZER_PAS PIN_PB6
#define BTN        PIN_PF7
#define Red_LED    PIN_PD7
#define Green_LED  PIN_PC6

void buzzerInit()
{
  TCCR4B = (1<<CS43); // prescaler = 128
  pinMode(BUZZER_PAS, OUTPUT);
  digitalWrite(BUZZER_PAS, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    uint32_t ocr = 125000L / freq;
    if (ocr>255) {
      ocr=255;
    }
    if (!ocr) {
      ocr=1;
    }
    OCR4C = ocr;
    TCCR4A |= (1<<COM4B0); // enable output
  } else {
    TCCR4A &= ~(1<<COM4B0); // disable output
  }
}

#define buzzerOff(foo) buzzerOn(0)

#define Red_LED_ON  PORTD |= (1<<PORTD7);
#define Red_LED_OFF  PORTD &= ~(1<<PORTD7);

#define Green_LED_ON   PORTC |= (1<<PORTC6);
#define Green_LED_OFF  PORTC &= ~(1<<PORTC6);

//## RFM22B Pinouts for Public Edition (M2)
#define  nIRQ_1 (PINB & (1<<PINB7)) //PB7
#define  nIRQ_0 (!(PINB & (1<<PINB7))) //PB7

#define  nSEL_on PORTD |= (1<<PORTD6) //PD6
#define  nSEL_off PORTD &= ~(1<<PORTD6) //PD6

#define  SCK_on  PORTB |= (1<<PORTB1)  //PB1
#define  SCK_off PORTB &= ~(1<<PORTB1) //PB1

#define  SDI_on  PORTB |= (1<<PORTB2)  //PB2 MOSI
#define  SDI_off PORTB &= ~(1<<PORTB2) //PB2 MOSI

#define  SDO_1 (PINB & (1<<PINB3)) //PB3 MISO
#define  SDO_0 (!(PINB & (1<<PINB3)))  //PB3 MISO

#define SDO_pin  PIN_PB3
#define SDI_pin  PIN_PB2
#define SCLK_pin PIN_PB1
#define IRQ_pin  PIN_PB7
#define nSel_pin PIN_PD6


void setupSPI()
{
  pinMode(SCLK_pin, OUTPUT);
  pinMode(SDI_pin,  OUTPUT);
  pinMode(SDO_pin,  INPUT);
  pinMode(IRQ_pin,  INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
}

void setupRfmInterrupt()
{
  PCMSK0 |= (1<<PCINT7); //enable pin change interrupt
  PCICR |= (1<<PCIE0);
}

ISR(PCINT0_vect)
{
  if(nIRQ_0) { //check if pin is low
    if (RF_Mode == Transmit) {
      RF_Mode = Transmitted;
    }
    if (RF_Mode == Receive) {
      RF_Mode = Received;
    }
  }
}

#define SWAP_GPIOS

#endif

#if (BOARD_TYPE == 7) // PowerTowerRX
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT 8 // ICP1

//#define TX_AIN0 A4 // SDA
//#define TX_AIN1 A5 // SCL

#define BUZZER_ACT A1
#define BTN     A5 // Shorting SCL to GND will bind

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
}

void buzzerOn(uint16_t freq)
{
  // Leaving freq in since it is being used in code already.
  if (freq) {
    digitalWrite(BUZZER_ACT,HIGH);
  } else {
    digitalWrite(BUZZER_ACT,LOW);
  }
}

#define Red_LED 6
#define Green_LED 5

#define Red_LED2   A0
#define Green_LED2 A1
#define Red_LED_ON    { PORTD |=  _BV(6); PORTC |=  _BV(0); }
#define Red_LED_OFF   { PORTD &= ~_BV(6); PORTC &= ~_BV(0); }
#define Green_LED_ON  { PORTD |=  _BV(5); PORTC |=  _BV(1); }
#define Green_LED_OFF { PORTD &= ~_BV(5); PORTC &= ~_BV(1); }

#define buzzerOff(foo) buzzerOn(0)

//## RFM22B Pinouts for Public Edition (M2)
#define  nIRQ_1 (PIND & 0x04)==0x04 //D2
#define  nIRQ_0 (PIND & 0x04)==0x00 //D2

#define  nSEL_on PORTD |= (1<<4) //D4
#define  nSEL_off PORTD &= 0xEF //D4

#define  SCK_on  PORTB |= _BV(5)  //B5
#define  SCK_off PORTB &= ~_BV(5) //B5

#define  SDI_on  PORTB |= _BV(3)  //B3
#define  SDI_off PORTB &= ~_BV(3) //B3

#define  SDO_1 (PINB & _BV(4)) == _BV(4) //B4
#define  SDO_0 (PINB & _BV(4)) == 0x00  //B4

#define SDO_pin 12
#define SDI_pin 11
#define SCLK_pin 13
#define IRQ_pin 2
#define nSel_pin 4

void setupSPI()
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);   //SDI
  pinMode(SCLK_pin, OUTPUT);   //SCLK
  pinMode(IRQ_pin, INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
}

#define IRQ_interrupt 0
void setupRfmInterrupt()
{
  attachInterrupt(IRQ_interrupt, RFM22B_Int, FALLING);
}

#endif

#if (BOARD_TYPE == 8) // openLRSmicroRX
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT 8 // ICP1

//#define TX_AIN0 A4 // SDA
//#define TX_AIN1 A5 // SCL

#define BUZZER_PAS  3  // OCR2B
#define BUZZER_ACT A5
#define BTN     A4

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
  TCCR2A = (1<<WGM21); // mode=CTC
#if (F_CPU == 16000000)
  TCCR2B = (1<<CS22) | (1<<CS20); // prescaler = 128
#elif (F_CPU == 8000000)
  TCCR2B = (1<<CS22); // prescaler = 64
#else
#errror F_CPU Invalid
#endif
  pinMode(BUZZER_PAS, OUTPUT);
  digitalWrite(BUZZER_PAS, LOW);
}

void buzzerOn(uint16_t freq)
{
  if (freq) {
    uint32_t ocr = 125000L / freq;
    digitalWrite(BUZZER_ACT,HIGH);
    if (ocr>255) {
      ocr=255;
    }
    if (!ocr) {
      ocr=1;
    }
    OCR2A = ocr;
    TCCR2A |= (1<<COM2B0); // enable output
  } else {
    digitalWrite(BUZZER_ACT,LOW);
    TCCR2A &= ~(1<<COM2B0); // disable output
  }
}

#define Red_LED A3
#define Green_LED A2

#define Red_LED_ON    PORTC |=  _BV(3);
#define Red_LED_OFF   PORTC &= ~_BV(3);
#define Green_LED_ON  PORTC |=  _BV(2);
#define Green_LED_OFF PORTC &= ~_BV(2);

#define buzzerOff(foo) buzzerOn(0)

//## RFM22B Pinouts for Public Edition (M2)
#define  nIRQ_1 (PIND & 0x04)==0x04 //D2
#define  nIRQ_0 (PIND & 0x04)==0x00 //D2

#define  nSEL_on PORTD |= (1<<4) //D4
#define  nSEL_off PORTD &= 0xEF //D4

#define  SCK_on  PORTB |= _BV(5)  //B5
#define  SCK_off PORTB &= ~_BV(5) //B5

#define  SDI_on  PORTB |= _BV(3)  //B3
#define  SDI_off PORTB &= ~_BV(3) //B3

#define  SDO_1 (PINB & _BV(4)) == _BV(4) //B4
#define  SDO_0 (PINB & _BV(4)) == 0x00  //B4

#define SDO_pin 12
#define SDI_pin 11
#define SCLK_pin 13
#define IRQ_pin 2
#define nSel_pin 4

void setupSPI()
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);   //SDI
  pinMode(SCLK_pin, OUTPUT);   //SCLK
  pinMode(IRQ_pin, INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
}

#define IRQ_interrupt 0
void setupRfmInterrupt()
{
  attachInterrupt(IRQ_interrupt, RFM22B_Int, FALLING);
}

#endif

#if (BOARD_TYPE == 9) // BroversityRX
#if (__AVR_ATmega328P__ != 1) || (F_CPU != 16000000)
#warning Possibly wrong board selected, select Arduino Pro/Pro Mini 5V/16MHz w/ ATMega328
#endif

#define TelemetrySerial Serial

#define SLAVE_SELECT 8 // ICP1

//#define TX_AIN0 A4 // SDA
//#define TX_AIN1 A5 // SCL

#define BUZZER_ACT 10
#define BTN     A5 // Shorting SCL to GND will bind

void buzzerInit()
{
  pinMode(BUZZER_ACT, OUTPUT);
  digitalWrite(BUZZER_ACT, LOW);
}

void buzzerOn(uint16_t freq)
{
  // Leaving freq in since it is being used in code already.
  if (freq) {
    digitalWrite(BUZZER_ACT,HIGH);
  } else {
    digitalWrite(BUZZER_ACT,LOW);
  }
}

#define Red_LED 6
#define Green_LED 5

#define Red_LED2   A0
#define Green_LED2 A1
#define Red_LED_ON    { PORTD |=  _BV(6); PORTC |=  _BV(0); }
#define Red_LED_OFF   { PORTD &= ~_BV(6); PORTC &= ~_BV(0); }
#define Green_LED_ON  { PORTD |=  _BV(5); PORTC |=  _BV(1); }
#define Green_LED_OFF { PORTD &= ~_BV(5); PORTC &= ~_BV(1); }

#define buzzerOff(foo) buzzerOn(0)

#define  nIRQ_1 (PIND & 0x04)==0x04 //D2
#define  nIRQ_0 (PIND & 0x04)==0x00 //D2

#define  nSEL_on PORTD |= (1<<4) //D4
#define  nSEL_off PORTD &= 0xEF //D4

#define  SCK_on  PORTB |= _BV(5)  //B5
#define  SCK_off PORTB &= ~_BV(5) //B5

#define  SDI_on  PORTB |= _BV(3)  //B3
#define  SDI_off PORTB &= ~_BV(3) //B3

#define  SDO_1 (PINB & _BV(4)) == _BV(4) //B4
#define  SDO_0 (PINB & _BV(4)) == 0x00  //B4

#define SDO_pin 12
#define SDI_pin 11
#define SCLK_pin 13
#define IRQ_pin 2
#define nSel_pin 4

void setupSPI()
{
  pinMode(SDO_pin, INPUT);   //SDO
  pinMode(SDI_pin, OUTPUT);   //SDI
  pinMode(SCLK_pin, OUTPUT);   //SCLK
  pinMode(IRQ_pin, INPUT);   //IRQ
  pinMode(nSel_pin, OUTPUT);   //nSEL
}

#define IRQ_interrupt 0
void setupRfmInterrupt()
{
  attachInterrupt(IRQ_interrupt, RFM22B_Int, FALLING);
}

#endif
