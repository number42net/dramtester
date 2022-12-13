#include <avr/io.h>
#include "serial.h"
void serialSetup()
{
    /* Set Baudrate  */
    unsigned int ubrr = 8; // 115.2k @16mhz
    UBRR0H = (ubrr >> 8);
    UBRR0L = (ubrr);

    UCSR0C = 0x06;         /* Set frame format: 8data, 1stop bit  */
    UCSR0B = (1 << TXEN0); /* Enable  transmitter                 */
}

void serialPrint(const char data[])
{
    int i = 0;
    while (data[i] != 0) /* print the String  "Hello from ATmega328p" */
    {
        while (!(UCSR0A & (1 << UDRE0)))
            ;           /* Wait for empty transmit buffer*/
        UDR0 = data[i]; /* Put data into buffer, sends the data */
        i++;            /* increment counter           */
    }
}