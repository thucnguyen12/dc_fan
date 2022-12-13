#ifndef LED_H
#define LED_H
#include "main.h"
#include "parameter_define.h"
#include "gpio.h"

void led_on_time(uint32_t led_tick_count, uint8_t toggle_time);
void led_status_control (void);


#endif
