#include "WString.h"
#ifndef __MAIN_H__
#define __MAIN_H__


#define ADA_M0_RFM69 
//#define PRO_MINI_RFM69
#include <Arduino.h>
#include "rfm69.h"

typedef struct
{
    char            module;
    char            addr;         
} module_data_st;


typedef enum
{
    MSG_FORMAT_RAW = 0,
    MSG_FORMAT_SENSOR_JSON,
    MSG_FORMAT_RELAY_JSON
}  msg_format_et;

typedef enum
{
    STATUS_UNDEFINED = 0,
    STATUS_OK_FOR_ME,
    STATUS_NOT_FOR_ME,
    STATUS_UNKNOWN_COMMAND,
    STATUS_CORRECT_FRAME,
    STATUS_INCORRECT_FRAME,
} msg_status_et;


extern module_data_st     me;


#endif