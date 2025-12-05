#include "uart.h"
#include "json.h"
#include "rfm69.h"
#include "io.h"

uart_msg_st         uart;
extern module_data_st  module;

void uart_rx_task(void);


uart_msg_st *uart_get_data_ptr(void)
{
    return &uart;
}

void uart_initialize(void)
{
    uart.rx.avail = false;
}

void uart_read_uart(void)
{
    if (SerialX.available())
    {
        String Str;
        io_led_flash(LED_INDX_BLUE,20);
        Str  = SerialX.readStringUntil('\n');
        #ifdef MODEM_DEBUG_PRINT
        Serial.println(Str);
        #endif  
        if (Str.length()> 0)
        {
            Str.trim();
            uart.rx.len = Str.length();
            Str.toCharArray(uart.rx.msg, MAX_MESSAGE_LEN);
            uart.rx.avail = true;
            //uart.rx.str.remove(uart.rx.str.length()-1);
        }
        #ifdef MODEM_DEBUG_PRINT
        Serial.print("rx is available: "); Serial.println(uart.rx.msg);
        #endif        
    } 

}

void uart_parse_rx_frame(void)
{
    //rfm_send_msg_st *rx_msg = &send_msg; 
    bool do_continue = true;
    uint8_t len;

    if (uart.rx.len < UART_FRAME_POS_END)do_continue = false;
    if (do_continue){
        uart.rx.frame.tag       = uart.rx.msg[UART_FRAME_POS_TO_TAG];
        uart.rx.frame.addr      = uart.rx.msg[UART_FRAME_POS_TO_ADDR];
        uart.rx.frame.from_tag  = uart.rx.msg[UART_FRAME_POS_FROM_TAG];
        uart.rx.frame.from_addr = uart.rx.msg[UART_FRAME_POS_FROM_ADDR];
        uart.rx.frame.function  = uart.rx.msg[UART_FRAME_POS_FUNC];
        uart.rx.frame.index     = uart.rx.msg[UART_FRAME_POS_INDEX];
        uart.rx.frame.action    = uart.rx.msg[UART_FRAME_POS_ACTION];
    }

    if ((uart.rx.msg[0] != '<') || 
        (uart.rx.frame.tag != module.tag) || 
        (uart.rx.frame.addr  != module.addr) || 
        (uart.rx.msg[uart.rx.len-1] != '>'))  do_continue = false;

    if (do_continue)
    {   
        #ifdef MODEM_DEBUG_PRINT
        Serial.print("Buffer frame is OK\n");
        #endif

        uart.rx.status = STATUS_CORRECT_FRAME;
        if (uart.rx.frame.function == UART_CMD_TRANSMIT_RAW)  
            uart.rx.format = MSG_FORMAT_SENSOR_JSON;
        else uart.rx.format = MSG_FORMAT_RAW;       
    }
    else uart.rx.status = STATUS_INCORRECT_FRAME;
}

void uart_prepare_reply(void)
{
    uart.tx.msg[UART_FRAME_POS_START] = UART_FRAME_START;
    uart.tx.msg[UART_FRAME_POS_TO_TAG] = uart.rx.frame.from_tag; 
    uart.tx.msg[UART_FRAME_POS_TO_ADDR] = uart.rx.frame.from_addr;
    uart.tx.msg[UART_FRAME_POS_FROM_TAG] = uart.rx.frame.tag; 
    uart.tx.msg[UART_FRAME_POS_FROM_ADDR] = uart.rx.frame.addr;
    uart.tx.msg[UART_FRAME_POS_FUNC] = uart.rx.frame.function;
    uart.tx.msg[UART_FRAME_POS_INDEX] = uart.rx.frame.index;
    uart.tx.msg[UART_FRAME_POS_ACTION] = UART_ACTION_REPLY;
    uart.tx.msg[UART_FRAME_POS_DATA] = UART_FRAME_DUMMY;
    uart.tx.msg[UART_FRAME_POS_END] = UART_FRAME_END;
    uart.tx.msg[UART_FRAME_POS_END+1] = 0x00;
}

void uart_build_node_from_rx_str(void)
{
    uint8_t indx1;
    uint8_t indx2;
    String Str =uart.rx.msg;
    Str = Str.substring(UART_FRAME_POS_DATA, uart.rx.len-1);
    //Serial.print("uart_build_node_from_rx_str: ");Serial.print(uart.rx.len); Serial.println(Str);
    indx1 = 0;  //uart.rx.str.indexOf(':')
    indx2 = Str.indexOf(';');
    uart.node.zone = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    uart.node.name = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    uart.node.value = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    uart.node.remark = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    
}


void uart_build_node_tx_str(void)
{
    uart_prepare_reply();
    if(rfm69_receive_message_is_avail()){
        rfm_receive_msg_st *receive_p = rfm69_get_receive_data_ptr();
        String StrRadio = (char*) receive_p->radio_msg;  
        String StrTx ="";
        json_pick_data_from_rx(&uart);
        
        StrTx += json_parse_tag(StrRadio, "{\"Z");
        StrTx += ';';
        StrTx += json_parse_tag(StrRadio, ",\"S");
        StrTx += ';';
        StrTx += json_parse_tag(StrRadio, ",\"V");
        StrTx += ';';
        StrTx += json_parse_tag(StrRadio, ",\"R");
        StrTx += '>';

        StrTx.toCharArray(&uart.tx.msg[UART_FRAME_POS_DATA], UART_MAX_REPLY_LEN - UART_FRAME_POS_END -3);
        uint8_t len = strlen(uart.tx.msg);
        uart.tx.msg[len-1] = '>';
        uart.tx.msg[len] = 0x00;
    }
    else {
        // return an empty frame  starting with '-'
    }
}

void uart_build_raw_tx_str(void)
{
    rfm_receive_msg_st *receive_p = rfm69_get_receive_data_ptr();
    // uart.tx.str = "<#X1r:";
    uart_prepare_reply();
    memcpy(&uart.tx.msg[UART_FRAME_POS_DATA],(char*) receive_p->radio_msg, MAX_MESSAGE_LEN);
    uint8_t len  = strlen(uart.tx.msg);
    uart.tx.msg[len] = '>';  uart.tx.msg[len+1] = 0x00;
}

void uart_rx_send_rfm_from_raw(void)
{
    uart.rx.msg[uart.rx.len-1] = 0x00;
    // Serial.print("uart_rx_send_rfm_from_raw: "); Serial.println(uart.rx.msg);
    // Serial.print("...len: "); Serial.println(uart.rx.len);
    rfm69_radiate_msg(&uart.rx.msg[UART_FRAME_POS_DATA]);
}

void uart_rx_send_rfm_from_node(void)
{
    //uart.rx.str = uart.rx.str.substring(6,uart.rx.len - 1);
    uart_build_node_from_rx_str();
    rfm_send_msg_st *send_p = rfm69_get_send_data_ptr();
    json_convert_uart_node_to_json(send_p->radio_msg, &uart);
    rfm69_radiate_msg(send_p->radio_msg);
}


void uart_exec_cmnd(uart_cmd_et ucmd)
{
    switch(ucmd)
    {
        case UART_CMD_TRANSMIT_RAW:
            io_led_flash(LED_INDX_RED, 10);
            uart_rx_send_rfm_from_raw();
            break;
        case UART_CMD_TRANSMIT_NODE:
            io_led_flash(LED_INDX_RED, 20);
            uart_rx_send_rfm_from_node();
            break;
        case UART_CMD_GET_AVAIL:
            uart_prepare_reply(); 
            if(rfm69_receive_message_is_avail()) uart.tx.msg[UART_FRAME_POS_DATA] = '1';
            else uart.tx.msg[UART_FRAME_POS_DATA] = '0';
            SerialX.println(uart.tx.msg);
            break;
        case UART_CMD_GET_RSSI:
            uart_prepare_reply(); 
            if(rfm69_receive_message_is_avail()){
                String Str = String(rfm69_get_last_rssi());
                Str.toCharArray(&uart.tx.msg[UART_FRAME_POS_DATA], UART_MAX_REPLY_LEN - UART_FRAME_POS_DATA -3);
                //Serial.println(uart.tx.msg);
                uint8_t len = strlen(uart.tx.msg);
                uart.tx.msg[len] = UART_FRAME_END;
                uart.tx.msg[len+1] = 0x00;
            }
            else uart.tx.msg[UART_FRAME_POS_DATA] = UART_FRAME_DUMMY;
            SerialX.println(uart.tx.msg);
            break;
        case UART_CMD_READ_RAW:
            uart_build_raw_tx_str();
            rfm69_clr_receive_message_flag();
            SerialX.println(uart.tx.msg);          
            break;
        case UART_CMD_READ_NODE:
            uart_build_node_tx_str();
            rfm69_clr_receive_message_flag();
            SerialX.println(uart.tx.msg);          
            break;
        case UART_CMD_SET_PARAM:
            rfm69_set_transparent(uart.rx.msg[UART_FRAME_POS_DATA] == '1');
            break;

    }
}


void uart_print_rx_metadata(void)
{
    #ifdef MODEM_DEBUG_PRINT
    Serial.print(F("Length      ")); Serial.println(uart.rx.len);
    Serial.print(F("Avail       ")); Serial.println(uart.rx.avail);
    Serial.print(F("Status      ")); Serial.println(uart.rx.status);
    Serial.print(F("Tag         ")); Serial.println(uart.rx.frame.tag);
    Serial.print(F("Address     ")); Serial.println(uart.rx.frame.addr);
    Serial.print(F("Function    ")); Serial.println(uart.rx.frame.function);
    Serial.print(F("Index       ")); Serial.println(uart.rx.frame.index);
    Serial.print(F("Action      ")); Serial.println(uart.rx.frame.action);
    Serial.print(F("Format      ")); Serial.println(uart.rx.format);
    #endif
}    


void uart_rx_task(void)
{
    static uint8_t rx_state = 0;
    switch(rx_state)
    {
        case 0:
            rx_state = 10;
            break;
        case 10:
            uart_read_uart();    // if available -> uart->prx.str uart->rx.avail
            if(uart.rx.avail) rx_state = 20;
            break;
        case 20:
                uart_parse_rx_frame();
                #ifdef MODEM_DEBUG_PRINT
                Serial.println(uart.rx.msg);
                uart_print_rx_metadata();
                #endif
                uart.rx.avail = false;
                //Serial.print("uart.rx.status="); Serial.println(uart.rx.status);
                if (uart.rx.status == STATUS_CORRECT_FRAME) rx_state = 30;
                else rx_state = 10;
            break;
        case 30:
            uart_exec_cmnd(uart.rx.frame.function);
            rx_state = 10;
            break;
        case 40:
            rx_state = 10;
            break;
    }
}