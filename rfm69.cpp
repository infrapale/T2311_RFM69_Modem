#include "main.h"
#include <Arduino.h>
#include <Wire.h>
#include <RH_RF69.h>
#include <SPI.h>
#include "rfm69.h"

extern RH_RF69 *rf69p;

// char radio_packet[MAX_MESSAGE_LEN];
// RH_RF69 *rf69_p;


void rfm69_initialize(RH_RF69 *rf69_ptr)
{

    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);
    
    Serial.println("RFM69 TX Test!");
    digitalWrite(RFM69_RST, HIGH);    delay(100);
    digitalWrite(RFM69_RST, LOW);    delay(100);

    if (!rf69p->init()) {
       Serial.println("RFM69 radio init failed");
       while (1);
    }
    Serial.println("RFM69 radio init OK!");
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    // No encryption
    if (!rf69p->setFrequency(RF69_FREQ)) {
       Serial.println("setFrequency failed");
    }
    // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
    // ishighpowermodule flag set like this:
    rf69p->setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
 
    uint8_t key[] ="VillaAstrid_2003"; //exactly the same 16 characters/bytes on all nodes!   
    rf69p->setEncryptionKey(key);
  
    
    Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

}
