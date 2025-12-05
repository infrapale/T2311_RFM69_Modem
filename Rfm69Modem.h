#ifndef __RFM69MODEM_H__
#define __RFM69MODEM_H__

#undef      MODEM_DEBUG_PRINT
#include <RH_RF69.h>

// #define RFM69_CS      10
// #define RFM69_INT     2
// #define RFM69_IRQN    0  // Pin 2 is IRQ 0!
// #define RFM69_RST     9

//typedef void (*led_on_callback(uint32_t duration_ms));

typedef struct
{
    uint8_t pin_rfm69_rst;
    uint8_t pin_led;
    uint32_t led_timeout;
} rf_modem_st;


class Rfm69Modem
{

    private:
        rf_modem_st modem;

    public:
       RH_RF69   *_rf69p;

        Rfm69Modem(RH_RF69 *rf69p, uint8_t pin_rfm69_rst, uint8_t pin_led);

        static void led_on(uint32_t duration_ms);

        void initialize(uint8_t key[]);

        void modem_task(void);

        void radiate(char *buff);

        void receive(char *buff);

};

#endif