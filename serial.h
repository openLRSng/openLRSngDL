#include<stdint.h>
bool serialWrite(uint8_t);
void serialWriteSync(uint8_t);

bool serialAvailable();
uint8_t serialRead();

void serialFlush();

extern const char* serial_bits_str[];

#define SERIAL_8N1 0x00
#define SERIAL_8N2 0x01
#define SERIAL_8E1 0x02
#define SERIAL_8E2 0x03
#define SERIAL_8O1 0x04
#define SERIAL_8O2 0x05

void serialInit(uint32_t baudrate, uint8_t bits);

