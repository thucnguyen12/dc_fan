#include "gpio.h"
#include "Hardware.h"


void gpio_init (void)
{
    GPIO_Config_T gpioConfig;
	
	// config gpio IR pin (for remote control)
    gpioConfig.mode = GPIO_MODE_IN_PU;
    gpioConfig.speed = GPIO_SPEED_10MHz;
    gpioConfig.pin = IR_PIN;
	gpioConfig.intEn = GPIO_EINT_ENABLE;
    GPIO_Config(IR_GPIO, &gpioConfig);
	
	EINT_Config(EINT_PORT_A, EINT_TRIGGER_FALLING_RISING);
	NVIC_EnableIRQRequest(EINTA_IRQn,2);
    
    //fan ctrl speed
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= FAN_CTRL_PIN;

    GPIO_Config(FAN_CTRL_GPIO, &gpioConfig);
    
    //light ctrl
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LIGHT_PIN;

    GPIO_Config(LIGHT_GPIO, &gpioConfig);
    
    //swing ctrl
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= SWING_PIN;

    GPIO_Config(SWING_GPIO, &gpioConfig);
    GPIO_ClearBit(SWING_GPIO, SWING_PIN);
    
    //swing led ctrl
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LED_SWING_PIN;

    GPIO_Config(LED_SWING_GPIO, &gpioConfig);
    
    //led low speed ctrl
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LED_LOW_SPD_PIN;

    GPIO_Config(LED_LOW_SPD_GPIO, &gpioConfig);
    
    //led low speed ctrl
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LED_LOW_SPD_PIN;

    GPIO_Config(LED_LOW_SPD_GPIO, &gpioConfig);
    
    //led high speed ctrl
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LED_HIGH_SPD_PIN;

    GPIO_Config(LED_HIGH_SPD_GPIO, &gpioConfig);
    
    //led nature speed ctrl
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LED_NATURAL_PIN;

    GPIO_Config(LED_NATURAL_GPIO, &gpioConfig);
    
    //led charging
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LED_CHARG_PIN;

    GPIO_Config(LED_CHARG_GPIO, &gpioConfig);
    
    //led low battery
    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
    gpioConfig.speed 	= GPIO_SPEED_10MHz;
    gpioConfig.pin 		= LED_LOWBAT_PIN;

    GPIO_Config(LED_LOWBAT_GPIO, &gpioConfig);
    
    // button 1
    gpioConfig.mode     = GPIO_MODE_IN_FLOATING;
    gpioConfig.pin 	    = BT1_PIN;
    gpioConfig.intEn    = GPIO_EINT_DISABLE;
    
    GPIO_Config(BT1_GPIO, &gpioConfig);
    
    // button 2
    gpioConfig.mode     = GPIO_MODE_IN_FLOATING;
    gpioConfig.pin  	= BT2_PIN;
    gpioConfig.intEn    = GPIO_EINT_DISABLE;
    
    GPIO_Config(BT2_GPIO, &gpioConfig);

    // button 3
    gpioConfig.mode     = GPIO_MODE_IN_FLOATING;
    gpioConfig.pin 	    = BT3_PIN;
    gpioConfig.intEn    = GPIO_EINT_DISABLE;
    
    GPIO_Config(BT3_GPIO, &gpioConfig);
    
    // power in detect 
    gpioConfig.mode     = GPIO_MODE_IN_FLOATING;
    gpioConfig.pin 	    = PWRIN_PIN;
    gpioConfig.intEn    = GPIO_EINT_DISABLE;
    
    GPIO_Config(PWRIN_GPIO, &gpioConfig);
    
}

void gpio_deinit_in_sleep_mode (void)
{
}

void turn_off_led (GPIO_T* port, uint8_t pin)
{
    GPIO_ClearBit(port, pin);
}

void turn_on_led (GPIO_T* port, uint8_t pin)
{
     GPIO_SetBit(port, pin);
}

void toggle_led (GPIO_T* port, uint8_t pin)
{
    GPIO_Toggle (port, pin);
}

void turn_off_leds_speed (void)
{
    turn_off_led (LED_LOW_SPD_GPIO, LED_LOW_SPD_PIN);
    turn_off_led (LED_HIGH_SPD_GPIO, LED_HIGH_SPD_PIN);
    turn_off_led (LED_NATURAL_GPIO, LED_NATURAL_PIN);
}
