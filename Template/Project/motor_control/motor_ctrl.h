#ifndef MOTOR_H
#define MOTOR_H

#include "apm32f00x.h"
#include "apm32f00x_gpio.h"
#include "apm32f00x_eint.h"
#include "apm32f00x_misc.h"
#include "Hardware.h"
#include "apm32f00x_tmr2.h"

#define	DC_MaxCount	150

void DC_Moto_Init(void);
void DC_SetDuty(uint8_t	Duty);

#endif
