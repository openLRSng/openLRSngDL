//####### COMMON FUNCTIONS #########

void rfmSetCarrierFrequency(uint32_t f);
void rfmSetPower(uint8_t p);
uint8_t rfmGetRSSI(void);
void RF22B_init_parameter(void);
uint8_t spiReadRegister(uint8_t address);
void spiWriteRegister(uint8_t address, uint8_t data);
void tx_packet(uint8_t* pkt, uint8_t size);
void to_rx_mode(void);

uint32_t getInterval(struct bind_data *bd)
{
  uint32_t ret;
  // Sending a x byte packet on bps y takes about (emperical)
  // usec = (x + 15) * 8200000 / baudrate
#define BYTES_AT_BAUD_TO_USEC(bytes, bps, div) ((uint32_t)((bytes) + (div?20:15)) * 8200000L / (uint32_t)(bps))

  ret = 2 * BYTES_AT_BAUD_TO_USEC(bd->packetSize, modem_params[bd->modem_params].bps,0) + 3000;

  // round up to ms
  ret = ((ret + 999) / 1000) * 1000;

  return ret;
}

uint8_t countSetBits(uint16_t x)
{
  x  = x - ((x >> 1) & 0x5555);
  x  = (x & 0x3333) + ((x >> 2) & 0x3333);
  x  = x + (x >> 4);
  x &= 0x0F0F;
  return (x * 0x0101) >> 8;
}

// Spectrum analyser 'submode'
void scannerMode(void)
{
  char c;
  uint32_t nextConfig[4] = { 0, 0, 0, 0 };
  uint32_t startFreq = MIN_RFM_FREQUENCY, endFreq = MAX_RFM_FREQUENCY, nrSamples = 500, stepSize = 50000;
  uint32_t currentFrequency = startFreq;
  uint32_t currentSamples = 0;
  uint8_t nextIndex = 0;
  uint8_t rssiMin = 0, rssiMax = 0;
  uint32_t rssiSum = 0;
  printStrLn("scanner mode");
  to_rx_mode();

  while (startFreq != 1000) { // if startFreq == 1000, break (used to exit scannerMode)
    while (serialAvailable()) {
      c = serialRead();

      switch (c) {
      case 'D':
        printC('D');
        printUL(MIN_RFM_FREQUENCY);
        printC(',');
        printUL(MAX_RFM_FREQUENCY);
        printStrLn(",");
        break;

      case 'S':
        currentFrequency = startFreq;
        currentSamples = 0;
        break;

      case '#':
        nextIndex = 0;
        nextConfig[0] = 0;
        nextConfig[1] = 0;
        nextConfig[2] = 0;
        nextConfig[3] = 0;
        break;

      case ',':
        nextIndex++;

        if (nextIndex == 4) {
          nextIndex = 0;
          startFreq = nextConfig[0] * 1000UL; // kHz -> Hz
          endFreq   = nextConfig[1] * 1000UL; // kHz -> Hz
          nrSamples = nextConfig[2]; // count
          stepSize  = nextConfig[3] * 1000UL;   // kHz -> Hz

          // set IF filtter BW (kha)
          if (stepSize < 20000) {
            spiWriteRegister(0x1c, 0x32);   // 10.6kHz
          } else if (stepSize < 30000) {
            spiWriteRegister(0x1c, 0x22);   // 21.0kHz
          } else if (stepSize < 40000) {
            spiWriteRegister(0x1c, 0x26);   // 32.2kHz
          } else if (stepSize < 50000) {
            spiWriteRegister(0x1c, 0x12);   // 41.7kHz
          } else if (stepSize < 60000) {
            spiWriteRegister(0x1c, 0x15);   // 56.2kHz
          } else if (stepSize < 70000) {
            spiWriteRegister(0x1c, 0x01);   // 75.2kHz
          } else if (stepSize < 100000) {
            spiWriteRegister(0x1c, 0x03);   // 90.0kHz
          } else {
            spiWriteRegister(0x1c, 0x05);   // 112.1kHz
          }
        }

        break;

      default:
        if ((c >= '0') && (c <= '9')) {
          c -= '0';
          nextConfig[nextIndex] = nextConfig[nextIndex] * 10 + c;
        }
      }
    }

    if (currentSamples == 0) {
      // retune base
      rfmSetCarrierFrequency(currentFrequency);
      rssiMax = 0;
      rssiMin = 255;
      rssiSum = 0;
      delay(1);
    }

    if (currentSamples < nrSamples) {
      uint8_t val = rfmGetRSSI();
      rssiSum += val;

      if (val > rssiMax) {
        rssiMax = val;
      }

      if (val < rssiMin) {
        rssiMin = val;
      }

      currentSamples++;
    } else {
      printUL(currentFrequency / 1000UL);
      printC(',');
      printUL(rssiMax);
      printC(',');
      printUL(rssiSum / currentSamples);
      printC(',');
      printUL(rssiMin);
      printStrLn(",");
      currentFrequency += stepSize;

      if (currentFrequency > endFreq) {
        currentFrequency = startFreq;
      }

      currentSamples = 0;
    }
  }
}

#define NOP() __asm__ __volatile__("nop")

#define RF22B_PWRSTATE_POWERDOWN    0x00
#define RF22B_PWRSTATE_READY        0x01
#define RF22B_PACKET_SENT_INTERRUPT 0x04
#define RF22B_PWRSTATE_RX           0x05
#define RF22B_PWRSTATE_TX           0x09

#define RF22B_Rx_packet_received_interrupt   0x02

uint8_t ItStatus1, ItStatus2;

void spiWriteBit(uint8_t b);

void spiSendCommand(uint8_t command);
void spiSendAddress(uint8_t i);
uint8_t spiReadData(void);
void spiWriteData(uint8_t i);

void to_sleep_mode(void);
void rx_reset(void);

// **** SPI bit banging functions

void spiWriteBit(uint8_t b)
{
  if (b) {
    SCK_off;
    NOP();
    SDI_on;
    NOP();
    SCK_on;
    NOP();
  } else {
    SCK_off;
    NOP();
    SDI_off;
    NOP();
    SCK_on;
    NOP();
  }
}

uint8_t spiReadBit(void)
{
  uint8_t r = 0;
  SCK_on;
  NOP();

  if (SDO_1) {
    r = 1;
  }

  SCK_off;
  NOP();
  return r;
}

void spiSendCommand(uint8_t command)
{
  nSEL_on;
  SCK_off;
  nSEL_off;

  for (uint8_t n = 0; n < 8 ; n++) {
    spiWriteBit(command & 0x80);
    command = command << 1;
  }

  SCK_off;
}

void spiSendAddress(uint8_t i)
{
  spiSendCommand(i & 0x7f);
}

void spiWriteData(uint8_t i)
{
  for (uint8_t n = 0; n < 8; n++) {
    spiWriteBit(i & 0x80);
    i = i << 1;
  }

  SCK_off;
}

uint8_t spiReadData(void)
{
  uint8_t Result = 0;
  SCK_off;

  for (uint8_t i = 0; i < 8; i++) {   //read fifo data byte
    Result = (Result << 1) + spiReadBit();
  }

  return(Result);
}

uint8_t spiReadRegister(uint8_t address)
{
  uint8_t result;
  spiSendAddress(address);
  result = spiReadData();
  nSEL_on;
  return(result);
}

void spiWriteRegister(uint8_t address, uint8_t data)
{
  address |= 0x80; //
  spiSendCommand(address);
  spiWriteData(data);
  nSEL_on;
}

// **** RFM22 access functions

void rfmSetChannel(uint8_t ch)
{
  uint8_t magicLSB = (bind_data.rf_magic & 0xff) ^ ch;
  spiWriteRegister(0x79, bind_data.hopchannel[ch]);
  spiWriteRegister(0x3a + 3, magicLSB);
  spiWriteRegister(0x3f + 3, magicLSB);
}

uint8_t rfmGetRSSI(void)
{
  return spiReadRegister(0x26);
}

uint16_t rfmGetAFCC(void)
{
  return (((uint16_t)spiReadRegister(0x2B) << 2) | ((uint16_t)spiReadRegister(0x2C) >> 6));
}

void setModemRegs(struct rfm22_modem_regs* r)
{

  spiWriteRegister(0x1c, r->r_1c);
  spiWriteRegister(0x1d, r->r_1d);
  spiWriteRegister(0x1e, r->r_1e);
  spiWriteRegister(0x20, r->r_20);
  spiWriteRegister(0x21, r->r_21);
  spiWriteRegister(0x22, r->r_22);
  spiWriteRegister(0x23, r->r_23);
  spiWriteRegister(0x24, r->r_24);
  spiWriteRegister(0x25, r->r_25);
  spiWriteRegister(0x2a, r->r_2a);
  spiWriteRegister(0x6e, r->r_6e);
  spiWriteRegister(0x6f, r->r_6f);
  spiWriteRegister(0x70, r->r_70);
  spiWriteRegister(0x71, r->r_71);
  spiWriteRegister(0x72, r->r_72);
}

void rfmSetCarrierFrequency(uint32_t f)
{
  uint16_t fb, fc, hbsel;
  if (f < 480000000) {
    hbsel = 0;
    fb = f / 10000000 - 24;
    fc = (f - (fb + 24) * 10000000) * 4 / 625;
  } else {
    hbsel = 1;
    fb = f / 20000000 - 24;
    fc = (f - (fb + 24) * 20000000) * 2 / 625;
  }
  spiWriteRegister(0x75, 0x40 + (hbsel ? 0x20 : 0) + (fb & 0x1f));
  spiWriteRegister(0x76, (fc >> 8));
  spiWriteRegister(0x77, (fc & 0xff));
}

void rfmSetPower(uint8_t p)
{
  spiWriteRegister(0x6d, p);
}

void init_rfm(uint8_t isbind)
{
#ifdef SDN_pin
  digitalWrite(SDN_pin, 1);
  delay(50);
  digitalWrite(SDN_pin, 0);
  delay(50);
#endif

  ItStatus1 = spiReadRegister(0x03);   // read status, clear interrupt
  ItStatus2 = spiReadRegister(0x04);
  spiWriteRegister(0x06, 0x00);    // disable interrupts
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY); // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
  spiWriteRegister(0x09, 0x7f);   // c = 12.5p
  spiWriteRegister(0x0a, 0x05);
#ifdef SWAP_GPIOS
  spiWriteRegister(0x0b, 0x15);    // gpio0 RX State
  spiWriteRegister(0x0c, 0x12);    // gpio1 TX State
#else
  spiWriteRegister(0x0b, 0x12);    // gpio0 TX State
  spiWriteRegister(0x0c, 0x15);    // gpio1 RX State
#endif
#ifdef ANTENNA_DIVERSITY
  spiWriteRegister(0x0d, (bind_data.flags & DIVERSITY_ENABLED)?0x17:0xfd); // gpio 2 ant. sw 1 if diversity on else VDD
#else
  spiWriteRegister(0x0d, 0xfd);    // gpio 2 VDD
#endif
  spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

  if (isbind) {
    setModemRegs(&bind_params);
  } else {
    setModemRegs(&modem_params[bind_data.modem_params]);
  }

  // Packet settings
  spiWriteRegister(0x30, 0x8c);    // enable packet handler, msb first, enable crc,
  spiWriteRegister(0x32, 0x0f);    // no broadcast, check header bytes 3,2,1,0
  spiWriteRegister(0x33, 0x42);    // 4 byte header, 2 byte synch, variable pkt size
  spiWriteRegister(0x34, (bind_data.flags & DIVERSITY_ENABLED)?0x14:0x0a);    // 40 bit preamble, 80 with diversity
  spiWriteRegister(0x35, 0x2a);    // preath = 5 (20bits), rssioff = 2
  spiWriteRegister(0x36, 0x2d);    // synchronize word 3
  spiWriteRegister(0x37, 0xd4);    // synchronize word 2
  spiWriteRegister(0x38, 0x00);    // synch word 1 (not used)
  spiWriteRegister(0x39, 0x00);    // synch word 0 (not used)

  uint32_t magic = isbind ? BIND_MAGIC : bind_data.rf_magic;
  for (uint8_t i = 0; i < 4; i++) {
    spiWriteRegister(0x3a + i, (magic >> 24) & 0xff);   // tx header
    spiWriteRegister(0x3f + i, (magic >> 24) & 0xff);   // rx header
    magic = magic << 8; // advance to next byte
  }

  spiWriteRegister(0x43, 0xff);    // all the bit to be checked
  spiWriteRegister(0x44, 0xff);    // all the bit to be checked
  spiWriteRegister(0x45, 0xff);    // all the bit to be checked
  spiWriteRegister(0x46, 0xff);    // all the bit to be checked

  if (isbind) {
    spiWriteRegister(0x6d, BINDING_POWER);
  } else {
    spiWriteRegister(0x6d, bind_data.rf_power);
  }

  spiWriteRegister(0x79, 0);

  spiWriteRegister(0x7a, bind_data.rf_channel_spacing);   // channel spacing

  spiWriteRegister(0x73, 0x00);
  spiWriteRegister(0x74, 0x00);    // no offset

  rfmSetCarrierFrequency(isbind ? BINDING_FREQUENCY : bind_data.rf_frequency);
  ENABLE_RFMINTERRUPT;
}

void to_rx_mode(void)
{
  ItStatus1 = spiReadRegister(0x03);
  ItStatus2 = spiReadRegister(0x04);
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  delay(10);
  rx_reset();
  NOP();
}

static inline void clearFIFO()
{
  //clear FIFO, disable multipacket, enable diversity if needed
#ifdef ANTENNA_DIVERSITY
  spiWriteRegister(0x08, (bind_data.flags & DIVERSITY_ENABLED)?0x83:0x03);
  spiWriteRegister(0x08, (bind_data.flags & DIVERSITY_ENABLED)?0x80:0x00);
#else
  spiWriteRegister(0x08, 0x03);
  spiWriteRegister(0x08, 0x00);
#endif
}

void rx_reset(void)
{
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  spiWriteRegister(0x7e, 36);    // threshold for rx almost full, interrupt when 1 byte received
  clearFIFO();
  spiWriteRegister(0x07, RF22B_PWRSTATE_RX);   // to rx mode
  spiWriteRegister(0x05, RF22B_Rx_packet_received_interrupt);
  ItStatus1 = spiReadRegister(0x03);   //read the Interrupt Status1 register
  ItStatus2 = spiReadRegister(0x04);
}

uint32_t tx_start = 0;

void tx_packet_async(uint8_t* pkt, uint8_t size)
{
  spiWriteRegister(0x3e, size);   // total tx size

  for (uint8_t i = 0; i < size; i++) {
    spiWriteRegister(0x7f, pkt[i]);
  }

  spiWriteRegister(0x05, RF22B_PACKET_SENT_INTERRUPT);
  ItStatus1 = spiReadRegister(0x03);      //read the Interrupt Status1 register
  ItStatus2 = spiReadRegister(0x04);
  tx_start = micros();
  spiWriteRegister(0x07, RF22B_PWRSTATE_TX);    // to tx mode

  RF_Mode = Transmit;
}

void tx_packet(uint8_t* pkt, uint8_t size)
{
  tx_packet_async(pkt, size);
  while ((RF_Mode == Transmit) && ((micros() - tx_start) < 100000));
  if (RF_Mode == Transmit) {
    printStrLn("TX timeout!");
  }

#ifdef TX_TIMING
  printStr("TX took:");
  printULLn(micros() - tx_start);
#endif
}

uint8_t tx_done()
{
  if (RF_Mode == Transmitted) {
#ifdef TX_TIMING
    printStr("TX took:");
    printULLn(micros() - tx_start);
#endif
    RF_Mode = Available;
    return 1; // success
  }
  if ((RF_Mode == Transmit) && ((micros() - tx_start) > 100000)) {
    spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
    RF_Mode = Available;
    return 2; // timeout
  }
  return 0;
}

// Print version, either x.y or x.y.z (if z != 0)
void printVersion(uint16_t v)
{
  printUL(v >> 8);
  printC('.');
  printUL((v >> 4) & 0x0f);
  if (version & 0x0f) {
    printC('.');
    printUL(v & 0x0f);
  }
}

// Halt and blink failure code
void fatalBlink(uint8_t blinks)
{
  while (1) {
    for (uint8_t i=0; i < blinks; i++) {
      Red_LED_ON;
      delay(100);
      Red_LED_OFF;
      delay(100);
    }
    delay(300);
  }
}
