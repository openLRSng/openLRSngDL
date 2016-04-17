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
#define DEFAULT_SMODE 0
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
  uint8_t  serial_mode;
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
  bind_data.serial_mode = DEFAULT_SMODE;
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

