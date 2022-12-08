#ifndef PARA_H
#define PARA_H

#include "apm32f00x.h"

typedef enum{
	SYST_OFF=0,
	SYST_ON=1
}SYST_ET;

typedef enum{
    speed_off = 0,
	low_spd = 1,
	high_spd = 2,
    nature_spd = 3
} speed_type;

typedef struct{
	uint32_t	Tick_1ms;
	uint32_t	Tick_10ms;
	uint32_t	Tick_100ms;
	uint32_t	Tick_200ms;
	uint32_t	Tick_1000ms;
}Tick_T;

typedef struct 
{
    uint8_t level;
    uint8_t delay_scan;
    uint8_t thresh;
    uint8_t clk_cnt;
    uint8_t last_state;
} button_t;

typedef struct
{
    SYST_ET 	SystemStatus;
    uint16_t    Vbat;
    uint8_t     in_sleep_mode;
    uint8_t     is_charging;
    speed_type  speed;
    uint8_t     lightLevel;
    Tick_T		Tick;
    button_t    BT1;
    button_t    BT2;
    button_t    BT3;
    uint16_t	IrCode;
} sys_parameter;

extern sys_parameter xSystem_para_now;

#endif

