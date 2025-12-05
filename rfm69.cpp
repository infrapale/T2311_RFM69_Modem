#include <Arduino.h>
#include <Wire.h>
#include <RH_RF69.h>
#include <SPI.h>
#include "rfm69.h"

RH_RF69 *rf69p;

rfm_receive_msg_st  receive_msg;
rfm_send_msg_st     send_msg;
rfm69_st rfm69 ={ .transparent = false};

void turn_led_on_ms( uint32_t duration_ms);

static led_on_function led_on_ms;

void rfm69_set_led_cb(led_on_function cb)
{
    led_on_ms = cb;
}

void turn_led_on_ms( uint32_t duration_ms)
{
    if (led_on_ms) {
        led_on_ms(duration_ms);   // pass through the parameter
    }
}

void rfm69_initialize(RH_RF69 *rf69_p, uint8_t pin_rst, uint8_t key[])
{
    rf69p = rf69_p;
    pinMode(pin_rst, OUTPUT);
    digitalWrite(pin_rst, LOW);
    digitalWrite(pin_rst, HIGH);    delay(100);
    digitalWrite(pin_rst, LOW);    delay(100);

    if (!rf69p->init()) {
       Serial.println(F("RFM69 radio init failed"));
       while (1){ delay(10000);};
    }
    #ifdef MODEM_DEBUG_PRINT
    Serial.println(F("RFM69 radio init OK!"));
    #endif
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    // No encryption
    if (!rf69p->setFrequency(RF69_FREQ)) {
        Serial.println(F("RF69 setFrequency failed!!"));
    }
    // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
    // ishighpowermodule flag set like this:
    rf69p->setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
 
    //uint8_t key[] = RFM69_KEY; //exactly the same 16 characters/bytes on all nodes!   
    rf69p->setEncryptionKey(key);
  
    #ifdef MODEM_DEBUG_PRINT
    Serial.print(F("RFM69 radio @"));  Serial.print((int)RF69_FREQ);  Serial.println(F(" MHz"));
    #endif   

    // Initialize Receive 
    receive_msg.avail = false;

}

void rfm69_set_transparent(bool is_transparent)
{
    rfm69.transparent = is_transparent;
}

rfm_receive_msg_st *rfm69_get_receive_data_ptr(void)
{
    return &receive_msg;
}

rfm_send_msg_st *rfm69_get_send_data_ptr(void)
{
    return &send_msg;
}
int16_t rfm69_get_last_rssi(void)
{
    return (receive_msg.rssi);
}

//*****************   Receive   *****************************************

void rfm69_receive_message(void)
{
    //rfm_receive_msg_st *tx_msg = &receive_msg;
    if (rf69p->available()) 
    {
        receive_msg.len = sizeof(receive_msg.radio_msg);
        if (rf69p->recv((uint8_t*)receive_msg.radio_msg, &receive_msg.len)) 
        {
            receive_msg.avail = true;
            if (receive_msg.len > 0)
            {   
                
                turn_led_on_ms(10000);
                if (receive_msg.len >= MAX_MESSAGE_LEN) receive_msg.len = MAX_MESSAGE_LEN -1;
                receive_msg.radio_msg[receive_msg.len] = 0;
                if (rfm69.transparent) Serial.println((char*)receive_msg.radio_msg);  
                #ifdef MODEM_DEBUG_PRINT
                Serial.print(F("Received [)"));Serial.print(receive_msg.len);Serial.print("]: ");
                Serial.println((char*)receive_msg.radio_msg);               
                Serial.print(F("len: "));
                Serial.print(receive_msg.len, DEC);
                Serial.print(F("  RSSI: "));
                Serial.println(rf69p->lastRssi(), DEC);
                #endif
                receive_msg.rssi = rf69p->lastRssi();

            }
        }
    }
}

bool rfm69_receive_message_is_avail(void)
{
    bool  is_avail = receive_msg.avail;
    // receive_msg.avail = false;
    return  is_avail;
}

void rfm69_clr_receive_message_flag(void)
{
    receive_msg.avail = false;
}

//*****************   Send   *****************************************
void rfm69_radiate_msg( char *radio_msg )
{
    //Serial.print("rfm69_radiate_msg: "); Serial.println(radio_msg); 
    if (radio_msg[0] != 0)
    {
        #ifdef DEBUG_PRINT
        Serial.println(radio_msg);
        #endif
        rf69p->waitPacketSent();
        rf69p->send((uint8_t *)radio_msg, strlen(radio_msg));      
    }
}



