/*****************************************************************************
T2512_RFM69_Modem 
*******************************************************************************

HW: Adafruit M0 RFM69 Feather or Arduino Pro Mini + RFM69

Send and receive data via UART

*******************************************************************************
https://github.com/infrapale/T2310_RFM69_TxRx
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
*******************************************************************************


*******************************************************************************
Sensor Radio Message:   {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                        {"Z":"Dock","S":"T_dht22","V":"8.7","R":"-"}
Relay Radio Message     {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
Sensor Node Rx Mesage:  <#X1N:OD1;Temp;25.0;->
Relay Node Rx Mesage:   <#X1N:RMH1;RKOK1;T;->

Relay Mesage      <#R12=x>   x:  0=off, 1=on, T=toggle

*******************************************************************************
**/

#include <Arduino.h>
#include "main.h"
#ifdef ADAFRUIT_FEATHER_M0
#include <wdt_samd21.h>
#endif
#ifdef PRO_MINI_RFM69
#include "avr_watchdog.h"
#endif
#include "secrets.h"
#include <RH_RF69.h>
#include <VillaAstridCommon.h>
#include "atask.h"
#include "json.h"
#include "rfm69.h"
#include "uart.h"
#include "rfm_receive.h"
#include "rfm_send.h"
#include "io.h"

#define ZONE  "OD_1"
//*********************************************************************************************
#define SERIAL_BAUD   9600
#define ENCRYPTKEY    RFM69_KEY   // defined in secret.h


RH_RF69         rf69(RFM69_CS, RFM69_INT);
RH_RF69         *rf69p;
module_data_st  module = {MY_MODULE_TAG, MY_MODULE_ADDR};
time_type       MyTime = {2023, 11,01,1,01,55}; 

#define NBR_TEST_MSG  4
#define LEN_TEST_MSG  32
const char test_msg[NBR_TEST_MSG][LEN_TEST_MSG] =
{  //12345678901234567890123456789012
    "<#X1N:Dock;T_bmp1;9.1;->",
    "<#X1N:Dock;T_dht22;8.7;->",
    "<#X1N:Dock;T_Water;5.3;->",
    "<#X1N:Dock;ldr1;0.33;->",
};

void debug_print_task(void);
void run_100ms(void);
void send_test_data_task(void);
void rfm_receive_task(void); 


atask_st debug_print_handle        = {"Debug Print    ", 5000,0, 0, 255, 0, 1, debug_print_task};
atask_st clock_handle              = {"Tick Task      ", 100,0, 0, 255, 0, 1, run_100ms};
atask_st rfm_receive_handle        = {"Receive <- RFM ", 500,0, 0, 255, 0, 1, rfm_receive_task};

#ifdef SEND_TEST_MSG
atask_st send_test_data_handle     = {"Send Test Task ", 10000,0, 0, 255, 0, 1, send_test_data_task};
#endif

#ifdef PRO_MINI_RFM69
//AVR_Watchdog watchdog(4);
#endif

rfm_receive_msg_st  *receive_p;
rfm_send_msg_st     *send_p;
uart_msg_st         *uart_p;



void initialize_tasks(void)
{

  atask_add_new(&debug_print_handle);
  atask_add_new(&clock_handle);
  atask_add_new(&rfm_receive_handle);

  #ifdef SEND_TEST_MSG
  atask_add_new(&send_test_data_handle);
  #endif
  Serial.print("Tasks initialized "); Serial.println(TASK_NBR_OF);
}


void setup() 
{
    //while (!Serial); // wait until serial console is open, remove if not tethered to computer
    delay(2000);
    Serial.begin(9600);
    Serial.print("T2512_RFM69_Modem"); Serial.print(" Compiled: ");
    Serial.print(__DATE__); Serial.print(" ");
    Serial.print(__TIME__); Serial.println();

    SerialX.begin(9600);
    atask_initialize();
    
    uart_initialize();
    uart_p = uart_get_data_ptr();
    send_p = rfm_send_get_data_ptr();

    rf69p = &rf69;
    rfm69_initialize(&rf69);
    rfm_receive_initialize();
    rfm_send_radiate_msg("T2512_RFM69_Modem");

    io_initialize();
    // Hard Reset the RFM module
    
    initialize_tasks();


    #ifdef ADAFRUIT_FEATHER_M0
    // Initialze WDT with a 2 sec. timeout
    wdt_init ( WDT_CONFIG_PER_16K );
    #endif
    #ifdef PRO_MINI_RFM69
    //watchdog.set_timeout(4);
    #endif


}



void loop() 
{
    atask_run();  
}


void rfm_receive_task(void) 
{
    rfm_receive_message();
    #ifdef ADAFRUIT_FEATHER_M0
    wdt_reset();
    #endif
    #ifdef PRO_MINI_RFM69
    // watchdog.clear();
    #endif
}


void run_100ms(void)
{
    static uint8_t ms100 = 0;
    if (++ms100 >= 10 )
    {
        ms100 = 0;
        if (++MyTime.second > 59 )
        {
          MyTime.second = 0;
          if (++MyTime.minute > 59 )
          {    
            MyTime.minute = 0;
            if (++MyTime.hour > 23)
            {
                MyTime.hour = 0;
            }
          }   
      }
    }
    io_run_100ms();
}

void debug_print_task(void)
{
  atask_print_status(true);
}

#ifdef SEND_TEST_MSG
void send_test_data_task(void)
{
    if  (send_test_data_handle.state >= NBR_TEST_MSG ) send_test_data_handle.state = 0;

    uart_p->rx.str  = test_msg[send_test_data_handle.state];
    uart_p->rx.avail = true;
    send_test_data_handle.state++;
}
#endif

