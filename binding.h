// OpenLRSngDL binding

// Factory setting values, modify via the CLI

//####### RADIOLINK RF POWER (beacon is always 100/13/1.3mW) #######
// 7 == 100mW (or 1000mW with M3)
// 6 == 50mW (use this when using booster amp), (800mW with M3)
// 5 == 25mW
// 4 == 13mW
// 3 == 6mW
// 2 == 3mW
// 1 == 1.6mW
// 0 == 1.3mW
#define DEFAULT_RF_POWER 7

#define DEFAULT_CHANNEL_SPACING 5 // 50kHz
#define DEFAULT_HOPLIST 22,10,19,34,49,41
#define DEFAULT_RF_MAGIC 0xDEADFEED

//  0 -- 4800bps, best range
//  1 -- 9600bps, medium range
//  2 -- 19200bps, medium range
#define DEFAULT_DATARATE 2
#define DEFAULT_BAUDRATE 115200
#define MAX_PACKET_SIZE 49
#define MIN_PACKET_SIZE 9
#define DEFAULT_PACKET_SIZE 33

// CONFIG flag masks
#define WATCHDOG_USED       0x80 // read only flag, only sent to configurator
#define DIVERSITY_ENABLED   0x40
#define MUTE_TX             0x20
#define PACKET_MODE         0x01
#define STATUSPACKET_MODE   0x02
#define PACKETCRC_MODE      0x04
#define DEFAULT_FLAGS       0x00

#define BINDING_POWER     0x06 // not lowest since may result fail with RFM23BP

#define BIND_MAGIC (0xB16BEEF5 + (OPENLRSNGDL_VERSION & 0xfff0))
#define BINDING_VERSION ((OPENLRSNGDL_VERSION & 0x0ff0)>>4)

static uint8_t default_hop_list[] = {DEFAULT_HOPLIST};

// HW frequency limits
#if (RFMTYPE == 868)
#  define MIN_RFM_FREQUENCY 848000000
#  define MAX_RFM_FREQUENCY 888000000
#  define DEFAULT_CARRIER_FREQUENCY 868000000  // Hz  (ch 0)
#  define BINDING_FREQUENCY 868000000 // Hz
#elif (RFMTYPE == 915)
#  define MIN_RFM_FREQUENCY 895000000
#  define MAX_RFM_FREQUENCY 935000000
#  define DEFAULT_CARRIER_FREQUENCY 915000000  // Hz  (ch 0)
#  define BINDING_FREQUENCY 915000000 // Hz
#else
#  define MIN_RFM_FREQUENCY 413000000
#  define MAX_RFM_FREQUENCY 463000000
#  define DEFAULT_CARRIER_FREQUENCY 435000000  // Hz  (ch 0)
#  define BINDING_FREQUENCY 435000000 // Hz
#endif

#define MAXHOPS      24

struct bind_data {
  uint8_t  version;
  uint32_t serial_baudrate;
  uint32_t rf_frequency;
  uint32_t rf_magic;
  uint8_t  rf_power;
  uint8_t  rf_channel_spacing;
  uint8_t  hopchannel[MAXHOPS];
  uint8_t  modem_params;
  uint8_t  flags;
  uint16_t rfmType;
  uint8_t  packetSize;
  uint32_t maxFrequency;
} bind_data;

struct rfm22_modem_regs {
  uint32_t bps;
  uint8_t  r_1c, r_1d, r_1e, r_20, r_21, r_22, r_23, r_24, r_25, r_2a, r_6e, r_6f, r_70, r_71, r_72;
} modem_params[] = {
  { 4800, 0x1a, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x1b, 0x1e, 0x27, 0x52, 0x2c, 0x23, 0x30 }, // 50000 0x00
  { 9600, 0x05, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x20, 0x24, 0x4e, 0xa5, 0x2c, 0x23, 0x30 }, // 25000 0x00
  { 19200, 0x06, 0x40, 0x0a, 0xd0, 0x00, 0x9d, 0x49, 0x00, 0x7b, 0x28, 0x9d, 0x49, 0x2c, 0x23, 0x30 }, // 25000 0x01
  { 57600, 0x05, 0x40, 0x0a, 0x45, 0x01, 0xd7, 0xdc, 0x03, 0xb8, 0x1e, 0x0e, 0xbf, 0x00, 0x23, 0x2e },
  { 125000, 0x8a, 0x40, 0x0a, 0x60, 0x01, 0x55, 0x55, 0x02, 0xad, 0x1e, 0x20, 0x00, 0x00, 0x23, 0xc8 },
};

#define DATARATE_COUNT (sizeof(modem_params) / sizeof(modem_params[0]))

struct rfm22_modem_regs bind_params =
{ 9600, 0x05, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x20, 0x24, 0x4e, 0xa5, 0x2c, 0x23, 0x30 };

// prototype
void fatalBlink(uint8_t blinks);

#include <avr/eeprom.h>

// Save EEPROM by writing just changed data
void myEEPROMwrite(int16_t addr, uint8_t data)
{
  uint8_t retries = 5;
  while ((--retries) && (data != eeprom_read_byte((uint8_t *)addr))) {
    eeprom_write_byte((uint8_t *)addr, data);
  }
  if (!retries) {
    fatalBlink(2);
  }
}

#if 0
void CRC16_add(uint16_t *crcp, uint8_t c) // CCITT polynome
{
  uint8_t i;
  uint16_t crc = *crcp;
  crc ^= (uint16_t)c << 8;
  for (i = 0; i < 8; i++) {
    if (crc & 0x8000) {
      crc = (crc << 1) ^ 0x1021;
    } else {
      crc <<= 1;
    }
  }
  *crcp = crc;
}
#else
PROGMEM const uint16_t crc16t[256] = {
  0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
  0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
  0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
  0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
  0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
  0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
  0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
  0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
  0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
  0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
  0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
  0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
  0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
  0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
  0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
  0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
  0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
  0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
  0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
  0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
  0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
  0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
  0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
  0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
  0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
  0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
  0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
  0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
  0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
  0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
  0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
  0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040};

void CRC16_add(uint16_t *crcp, uint8_t c) // CCITT polynome
{
  uint16_t crc = *crcp;
  uint8_t index = (crc >> 8) ^ c;
  *crcp = (crc << 8) ^ pgm_read_byte_near((uint16_t)crc16t + index);
}
#endif

#define EEPROM_SIZE 1024 // EEPROM is 1k on 328p and 32u4
#define ROUNDUP(x) (((x)+15)&0xfff0)
#define MIN256(x)  (((x)<256)?256:(x))
#define EEPROM_DATASIZE MIN256(ROUNDUP(sizeof(bind_data) + 6))


bool accessEEPROM(bool write)
{
  void *dataAddress = NULL;
  uint16_t dataSize = 0;

  uint16_t addressNeedle = 0;
  uint16_t addressBase = 0;
  uint16_t CRC = 0;
  uint16_t CRCv = 0;
start:
  do {
    dataAddress = &bind_data;
    dataSize = sizeof(bind_data);
    addressNeedle = 0;
    addressNeedle += addressBase;
    CRCv = 0;

    for (uint8_t i = 0; i < dataSize; i++, addressNeedle++) {
      if (!write) {
        *((uint8_t*)dataAddress + i) = eeprom_read_byte((uint8_t *)(addressNeedle));
      } else {
        myEEPROMwrite(addressNeedle, *((uint8_t*)dataAddress + i));
      }

      CRC16_add(&CRCv, *((uint8_t*)dataAddress + i));
    }

    if (!write) {
      CRC = eeprom_read_byte((uint8_t *)addressNeedle) << 8 | eeprom_read_byte((uint8_t *)(addressNeedle + 1));

      if (CRCv == CRC) {
        // recover corrupted data
        // write operation is performed after every successful read operation, this will keep all cells valid
        write = true;
        addressBase = 0;
        goto start;
      } else {
        // try next block
      }
    } else {
      myEEPROMwrite(addressNeedle++, CRCv >> 8);
      myEEPROMwrite(addressNeedle, CRCv & 0x00FF);
    }
    addressBase += EEPROM_DATASIZE;
  } while (addressBase <= (EEPROM_SIZE - EEPROM_DATASIZE));
  return (write); // success on write, failure on read
}

void bindWriteEeprom()
{
  accessEEPROM(true);
}

void bindInitDefaults(void)
{
  bind_data.version = BINDING_VERSION;
  bind_data.serial_baudrate = DEFAULT_BAUDRATE;
  bind_data.rf_power = DEFAULT_RF_POWER;
  bind_data.rf_frequency = DEFAULT_CARRIER_FREQUENCY;
  bind_data.rf_channel_spacing = DEFAULT_CHANNEL_SPACING;

  bind_data.rf_magic = DEFAULT_RF_MAGIC;

  for (uint8_t c = 0; c < MAXHOPS; c++) {
    bind_data.hopchannel[c] = (c < sizeof(default_hop_list)) ? default_hop_list[c] : 0;
  }

  bind_data.modem_params = DEFAULT_DATARATE;
  bind_data.flags = DEFAULT_FLAGS;

  bind_data.maxFrequency = MAX_RFM_FREQUENCY;
  bind_data.packetSize = DEFAULT_PACKET_SIZE;
  bind_data.rfmType = RFMTYPE;
}

bool bindReadEeprom()
{
  if (accessEEPROM(false) && (bind_data.version == BINDING_VERSION)) {
    return true;
  }
  return false;
}

void bindRandomize(bool randomChannels)
{
  uint8_t emergency_counter = 0;
  uint8_t c;
  uint32_t t = 0;
  while (t == 0) {
    t = micros();
  }
  srandom(t);

  bind_data.rf_magic = 0;
  for (c = 0; c < 4; c++) {
    bind_data.rf_magic = (bind_data.rf_magic << 8) + (random() % 255);
  }

  if (randomChannels) {
    // TODO: verify if this works properly
    for (c = 0; (c < MAXHOPS) && (bind_data.hopchannel[c] != 0); c++) {
again:
      if (emergency_counter++ == 255) {
        bindInitDefaults();
        return;
      }

      uint8_t ch = (random() % 253) + 1;

      // don't allow same channel twice
      for (uint8_t i = 0; i < c; i++) {
        if (bind_data.hopchannel[i] == ch) {
          goto again;
        }
      }

      // don't allow frequencies higher then tx_config.maxFrequency
      uint32_t real_frequency = bind_data.rf_frequency + (uint32_t)ch * (uint32_t)bind_data.rf_channel_spacing * 10000UL;
      if (real_frequency > bind_data.maxFrequency) {
        goto again;
      }

      bind_data.hopchannel[c] = ch;
    }
  }
}

