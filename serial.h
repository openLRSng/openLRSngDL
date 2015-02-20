#include<stdint.h>
bool serialWrite(uint8_t);
void serialWriteSync(uint8_t);

bool serialAvailable();
uint8_t serialRead();

void serialFlush();

void serialInit(uint32_t baudrate);

