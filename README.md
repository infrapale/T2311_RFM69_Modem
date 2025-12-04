# T2311_RFM69_Modem

*******************************************************************************

*******************************************************************************
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio
https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide/all
https://github.com/infrapale/T2310_RFM69_TxRx

*******************************************************************************
UART Command Syntax

    rfm_send_msg_st *rx_msg = &send_msg; 
    Frame: <R1F1O2:L>\r\n
            \\\\\\\\______ value, message (optional)
             \\\\\\\______ action set: '=' get: '?' reply: ':'
              \\\\\\______ index
               \\\\\______ function
                \\\\______ sender addr
                 \\\______ sender tag
                  \\______ module_addr target
                   \______ module_tag target

*******************************************************************************

    module_tag  and sender tag identifies the type of module
      R Radio
      Y Relay
      K Keypad
      # All modules
    module_addr and sender addr
      0-9
      # All modules
    Radio Functions
      A Is messages available
      R Read Raw Message
      D Read Decoded Message
      T Transmit Raw Message
      J Send Desoced Message as json
      S Set parameter
    Index
      0-9
      0 is also used when N/A
    Action
      set: '=' 
      get: '?' 
      reply: ':'
    Value
      Total max 61 characters for RFM69

*******************************************************************************

    Examples:
      Check if we have available messages:  
          <R1X1A0?->  
          <X1R1A0:1>    1 message is available   
      Read raw messages:      
          <R1X1R0?->   
          <X1R1R0:{"Z":"MH1","S":"RKOK1","V":"T","R":"-"}>
      Read decoded messages:  
          <R1X1D0?->
          <X1R1D0:OD_1;Temp;23.1;->
          <X1R1D0:->    No message available
      Transmit Raw Message
          <R1X1T0={"Z":"MH1","S":"RKOK1","V":"T","R":"-"}>
          +-+-+-  {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
      Send Decoded Message as JSON
          <R1X1J0=RMH1;RKOK1;T;->
          +-+-+-  {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
      Set Parameter
          <R1X1S0=1>  Set transparent mode 
          <R1X1S0=0>  Set polling mode mode 


*******************************************************************************

******** UART ***************** Transmit Raw ********* Radio ********************
                                  --------
 <#X1T:Hello World>\n             |      |
--------------------------------->|      | Hello World
                                  |      |-------------------------------------->
                                  |      |
<---------------------------------|      |
                                  |      |<-------------------------------------
                                  --------

******** UART ***************** Transmit Node ********* Radio ********************
                                  --------
 <#X1N:RMH1;RKOK1;T;->\n          |      |
--------------------------------->|      | {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
                                  |      |-------------------------------------->
                                  |      |
<---------------------------------|      |
                                  |      |<-------------------------------------
                                  --------

******** UART *************** Check Radio Data ********* Radio ********************
                                  --------
<#X1A:>\n                         |      |
--------------------------------->|      | 
<#X1a:0>\n                        |      |
<---------------------------------|      | {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                                  |      |<-------------------------------------
<#X1A:>\n                         |      |
--------------------------------->|      | 
<#X1a:1>\n                        |      |
<---------------------------------|      | 
                                  |      |
                                  --------

******** UART ************ Read Radio Raw Data ********* Radio ********************
                                  --------
                                  |      | {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                                  |      |<-------------------------------------
<#X1R:>\n                         |      |
--------------------------------->|      | 
<#X1r:{"Z":"OD_1","S":"Temp",     |      |
"V":23.1,"R":"-"}>                |      |
<---------------------------------|      | 
                                  |      |
                                  --------

******** UART ************ Read Radio Node Data ********* Radio ********************
                                  --------
                                  |      | {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                                  |      |<-------------------------------------
<#X1O:>\n                         |      |
--------------------------------->|      | 
<#X1o:OD_1;Temp;23.1;->\n         |      |
<---------------------------------|      | 
                                  |      |
                                  --------

UART Commands
  UART_CMD_TRANSMIT_RAW   = 'T',
  UART_CMD_TRANSMIT_NODE  = 'N',
  UART_CMD_GET_AVAIL      = 'A',
  UART_CMD_READ_RAW       = 'R',
  UART_CMD_READ_NODE      = 'O' 

UART Replies
  UART_REPLY_AVAILABLE    = 'a',
  UART_REPLY_READ_RAW     = 'r',
  UART_REPLY_READ_NODE    = 'o' 

*******************************************************************************
Sensor Radio Message:   {"Z":"OD_1","S":"Temp","V":23.1,"R":"-"}
                        {"Z":"Dock","S":"T_dht22","V":"8.7","R":"-"}
Relay Radio Message     {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
Sensor Node Rx Mesage:  <#X1N:OD1;Temp;25.0;->
Relay Node Rx Mesage:   <#X1N:RMH1;RKOK1;T;->

Relay Mesage      <#R12=x>   x:  0=off, 1=on, T=toggle


*******************************************************************************
UART Commands
-  UART_CMD_TRANSMIT_RAW   = 'T',
-  UART_CMD_TRANSMIT_NODE  = 'N',
-  UART_CMD_GET_AVAIL      = 'A',
-  UART_CMD_READ_RAW       = 'R',
-  UART_CMD_READ_NODE      = 'O' 

UART Replies
-  UART_REPLY_AVAILABLE    = 'a',
-  UART_REPLY_READ_RAW     = 'r',
-  UART_REPLY_READ_NODE    = 'o' 


<#X1T:Hello World>
<#X1T:<*RLH_1=T>>
<*RLH_1=T>
<*RLH_2=T>
