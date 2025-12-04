#include "main.h"
#include "io.h"
#include <Arduino.h>
#include <Wire.h>
#include <RH_RF69.h>
#include <SPI.h>
#include "rfm69.h"
#include "secrets.h"

extern RH_RF69 *rf69p;

// char radio_packet[MAX_MESSAGE_LEN];
// RH_RF69 *rf69_p;

rfm_receive_msg_st  receive_msg;
rfm_send_msg_st     send_msg;

//char radio_packet[RH_RF69_MAX_MESSAGE_LEN];




void rfm69_initialize(RH_RF69 *rf69_ptr)
{

    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);
    digitalWrite(RFM69_RST, HIGH);    delay(100);
    digitalWrite(RFM69_RST, LOW);    delay(100);

    if (!rf69p->init()) {
       Serial.println(F("RFM69 radio init failed"));
       while (1);
    }
    #ifdef DEBUG_PRINT
    Serial.println(F("RFM69 radio init OK!"));
    #endif
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    // No encryption
    if (!rf69p->setFrequency(RF69_FREQ)) {
        #ifdef DEBUG_PRINT
        Serial.println(F("setFrequency failed"));
        #endif
    }
    // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
    // ishighpowermodule flag set like this:
    rf69p->setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
 
    uint8_t key[] = RFM69_KEY; //exactly the same 16 characters/bytes on all nodes!   
    rf69p->setEncryptionKey(key);
  
    #ifdef DEBUG_PRINT
    Serial.print(F("RFM69 radio @"));  Serial.print((int)RF69_FREQ);  Serial.println(F(" MHz"));
    #endif   

    // Initialize Receive 
    receive_msg.avail = false;

}


rfm_receive_msg_st *rfm_receive_get_data_ptr(void)
{
    return &receive_msg;
}

rfm_send_msg_st *rfm_send_get_data_ptr(void)
{
    return &send_msg;
}
int16_t rfm69_get_last_rssi(void)
{
    return (receive_msg.rssi);
}

//*****************   Receive   *****************************************

void rfm_receive_message(void)
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
                io_led_flash(LED_INDX_GREEN,20); 
                if (receive_msg.len >= MAX_MESSAGE_LEN) receive_msg.len = MAX_MESSAGE_LEN -1;
                receive_msg.radio_msg[receive_msg.len] = 0;
                #ifdef DEBUG_PRINT
                Serial.print("Received [");Serial.print(receive_msg.len);Serial.print("]: ");
                Serial.println((char*)receive_msg.radio_msg);               
                Serial.print("len: ");
                Serial.print(receive_msg.len, DEC);
                Serial.print("  RSSI: ");
                Serial.println(rf69p->lastRssi(), DEC);
                #endif
                receive_msg.rssi = rf69p->lastRssi();

            }
        }
    }
}

bool rfm_receive_message_is_avail(void)
{
    bool  is_avail = receive_msg.avail;
    // receive_msg.avail = false;
    return  is_avail;
}

void rfm_receive_clr_message_flag(void)
{
    receive_msg.avail = false;
}

//*****************   Send   *****************************************
void rfm_send_radiate_msg( char *radio_msg )
{
    //Serial.print("rfm_send_radiate_msg: "); Serial.println(radio_msg); 
    if (radio_msg[0] != 0)
    {
        #ifdef DEBUG_PRINT
        Serial.println(radio_msg);
        #endif
        rf69p->waitPacketSent();
        rf69p->send((uint8_t *)radio_msg, strlen(radio_msg));      
    }
}



