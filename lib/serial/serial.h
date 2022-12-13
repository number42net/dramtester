#ifndef PRIMARY_HEADER_H
#define SERIAL_HEADER_H

#include <avr/io.h>


void serialSetup(void);

void serialPrint(const char data[]);

#endif