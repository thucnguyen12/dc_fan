
#ifndef GPIO_H
#define GPIO_H

#include "apm32f00x.h"
#include "apm32f00x_gpio.h"
#include "apm32f00x_eint.h"
#include "apm32f00x_misc.h"
#include "Hardware.h"



void gpio_init (void);

void turn_off_led (GPIO_T* port, uint8_t pin);

void turn_on_led (GPIO_T* port, uint8_t pin);

void toggle_led (GPIO_T* port, uint8_t pin);

void turn_off_leds_speed (void);

#endif
