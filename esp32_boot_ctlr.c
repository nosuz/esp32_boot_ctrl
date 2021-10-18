#define F_CPU 2666666 /* 16 MHz / 6 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define RESET_BM PIN1_bm
#define RESETPIN_CTRL PORTA.PIN1CTRL

#define RTS_BM PIN2_bm
#define DTR_BM PIN3_bm

#define EN_BM PIN6_bm
#define BOOT_BM PIN7_bm

int reset_pushed();
void send_reset();

int main(void)
{
    bool boot_mode = false;

    /* pull up enable */
    RESETPIN_CTRL = PORT_PULLUPEN_bm;
    /* setup port */
    PORTA.OUT = EN_BM | BOOT_BM;
    /* set EN to H and start ESP32 */
    PORTA.DIR = EN_BM;

    while (1)
    {
        uint8_t in_buf = ~PORTA.IN;

        if (in_buf & (RTS_BM | DTR_BM))
        {
            uint8_t out_buf = EN_BM | BOOT_BM;

            if (in_buf & RTS_BM)
            {
                out_buf &= ~EN_BM;

                if (!boot_mode)
                {
                    boot_mode = true;
                    PORTA.OUT &= ~EN_BM;
                    _delay_ms(10);
                    PORTA.DIR |= BOOT_BM;
                }
            }

            if (in_buf & DTR_BM)
            {
                out_buf &= ~BOOT_BM;
            }

            if (boot_mode)
            {
                PORTA.OUT = out_buf;
            }
        }
        else
        {
            /* exit reset mode */
            boot_mode = false;
            PORTA.OUT = EN_BM | BOOT_BM;
            PORTA.DIR &= ~BOOT_BM;
        }

        if (!boot_mode)
        {
            if (reset_pushed())
            {
                send_reset();
            }
        }
    }
}

int reset_pushed()
{
    return ~PORTA.IN & RESET_BM;
}

void send_reset()
{
    PORTA.OUT &= ~EN_BM;
    _delay_ms(10);
    PORTA.DIR |= BOOT_BM;
    PORTA.OUT &= ~BOOT_BM;

    while (reset_pushed())
    {
        _delay_ms(100);
    }
    _delay_ms(100);

    PORTA.OUT |= BOOT_BM;
    PORTA.DIR &= ~BOOT_BM;
    _delay_ms(10);

    PORTA.OUT |= EN_BM;
}