#ifndef LIGHT_H
#define LIGHT_H

#include "apm32f00x.h"
#include "apm32f00x_usart.h"
#include "apm32f00x_gpio.h"
#include "apm32f00x_rcm.h"
#include "Hardware.h"
#include "apm32f00x_tmr2.h"

void Light_PWM_Init(void);
void Light_SetDuty(uint8_t	Duty);

#endif