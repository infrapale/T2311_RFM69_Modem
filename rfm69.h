#ifndef __RFM69_H__
#define __RFM69_H__
#include <RH_RF69.h>

//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NETWORKID         100  //the same on all nodes that talk to each other
#define NODEID            10  
#define BROADCAST         255
#define MAX_MESSAGE_LEN   68
#define RECEIVER          BROADCAST    // The recipient of packets
//Match frequency to the hardware version of the radio on your Feather
//#define FREQUENCY       RF69_433MHZ
//#define FREQUENCY       RF69_868MHZ
//#define FREQUENCY       RF69_915MHZ
//#define ENCRYPTKEY        "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW       true // set to 'true' if you are using an RFM69HCW module

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ     434.0  //915.0


#ifdef  ADA_M0_RFM69
#define RFM69_CS      8
#define RFM69_INT     3
// #define RFM69_IRQN    0  // Pin 2 is IRQ 0!
#define RFM69_RST     4
#endif

#ifdef PRO_MINI_RFM69
#define RFM69_CS      10
#define RFM69_INT     2
#define RFM69_IRQN    0  // Pin 2 is IRQ 0!
#define RFM69_RST     9
#endif



/// @brief  Initialize menu
/// @note   Set opinter to the ohjecct
/// @param  object pointer
/// @return
void rfm69_initialize(RH_RF69 *rf69_ptr);
#endif
