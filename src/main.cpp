#include <avr/io.h>
#include <util/delay.h>
#include "serial.c"
#include <stdio.h>
#include <stdlib.h>
/* Pins
RAS     PD4
CAS     PD5
D       PD6 // chip data IN
Q       PB0 // chip data OUT
WRITE   PB1

A0-A5   PC0-PC5
A6-A7   PB2-PB3
*/

void set_addr(int addr)
{
    // Set all address lines to zero
    PORTC = 0x00;
    PORTB &= ~(1 << PB2) | ~(1 << PB3);

    // Apply a mask to each gpio
    PORTC |= (addr & (1 << 0));
    PORTC |= (addr & (1 << 1));
    PORTC |= (addr & (1 << 2));
    PORTC |= (addr & (1 << 3));
    PORTC |= (addr & (1 << 4));
    PORTC |= (addr & (1 << 5));
    PORTB |= (addr & (1 << 6)) >> 4;
    PORTB |= (addr & (1 << 7)) >> 4;
}

int main()
{
    serialSetup();

    // Address pins
    DDRC = 0xFF;                    // Set all pins on port C as output
    DDRB = (1 << PB2) | (1 << PB3); // Set PB2, PB3 to Output, all others as input

    // other pins
    DDRB |= (1 << PB1);                          // Set PB1 to Output
    DDRD = (1 << PD4) | (1 << PD5) | (1 << PD6); // Set PD4, PD5, PD6 to Output

    PORTB |= (1 << PB1);                    // Set write pin high (inactive)
    PORTD |= (1 << PORTD4) | (1 << PORTD5); // Set RAS and CAS high (inactive)

    // Initialize the chip as per datasheet (Samsung KM4164B)
    _delay_us(100);
    for (int row = 0; row < 8; ++row)
    {
        PORTD |= (1 << PORTD4) | (1 << PORTD5); // Set RAS and CAS high
        set_addr(row);
        PORTD &= ~(1 << PORTD4); // Set RAS low
    }

    int counter = 0;
    while (1)
    {
        counter++;
        char message[30];
        sprintf(message, "Loop: %d\n", counter);
        serialPrint(message);

        for (int row = 0; row < 255; ++row)
        {
            // Write all ones
            PORTD |= (1 << PORTD6);  // Set data bit to one
            PORTB &= ~(1 << PORTB1); // Set write low

            for (int col = 0; col < 255; ++col)
            {
                PORTD |= (1 << PORTD4) | (1 << PORTD5); // Set RAS and CAS high
                set_addr(row);
                PORTD &= ~(1 << PORTD4); // Set RAS low
                set_addr(col);
                PORTD &= ~(1 << PORTD5); // Set CAS low
            }

            // Read all ones
            PORTB |= (1 << PORTB1); // Set write high
            for (int col = 0; col < 255; ++col)
            {
                PORTD |= (1 << PORTD4) | (1 << PORTD5); // Set RAS and CAS high
                set_addr(row);
                PORTD &= ~(1 << PORTD4); // Set RAS low
                set_addr(col);
                PORTD &= ~(1 << PORTD5); // Set CAS low

                int value = (PINB & (1 << PINB0)) >> PINB0;
                if (value == 0)
                {
                    char message[80];
                    sprintf(message, "Error at row: %d, column: %d. Expected a 1, got a %d.\n", row, col, value);
                    serialPrint(message);
                    exit(0);
                }
            }

            // Write all zeroes
            PORTD &= ~(1 << PORTD6); // Set data bit to zero
            PORTB &= ~(1 << PORTB1); // Set write low

            for (int col = 0; col < 255; ++col)
            {
                PORTD |= (1 << PORTD4) | (1 << PORTD5); // Set RAS and CAS high
                set_addr(row);
                PORTD &= ~(1 << PORTD4); // Set RAS low
                set_addr(col);
                PORTD &= ~(1 << PORTD5); // Set CAS low
            }

            // Read all zeroes
            PORTB |= (1 << PORTB1); // Set write high

            for (int col = 0; col < 255; ++col)
            {
                PORTD |= (1 << PORTD4) | (1 << PORTD5); // Set RAS and CAS high
                set_addr(row);
                PORTD &= ~(1 << PORTD4); // Set RAS low
                set_addr(col);
                PORTD &= ~(1 << PORTD5); // Set CAS low

                int value = (PINB & (1 << PINB0)) >> PINB0;
                if (value == 1)
                {
                    char message[80];
                    sprintf(message, "Error at row: %d, column: %d. Expected a 0 got a %d.\n", row, col, value);
                    serialPrint(message);
                    exit(0);
                }
            }
        }
    }
}