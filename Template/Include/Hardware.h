/*!
 * @file        Hardware.h
 *
 * @brief       Header for main.c module 
 *
 * @version     V1.0.0
 *
 * @date        2019-11-4
 *
 */
#ifndef HARDWARE_H
#define HARDWARE_H
#include "apm32f00x.h"
#include "apm32f00x_usart.h"
#include "apm32f00x_gpio.h"
#include "apm32f00x_rcm.h"



#define 	IR_PIN			    GPIO_PIN_1
#define		IR_GPIO			    GPIOA
#define		IR_REMOTE_READ 	    GPIO_ReadInputBit(IR_GPIO,IR_PIN)

#define 	FAN_CTRL_PIN	    GPIO_PIN_3		// TIM2_CH3		// Fan's Motor control 
#define		FAN_CTRL_GPIO	    GPIOA
#define 	FAN_PWM_CH		    TMR2_CHANNEL_3

#define 	LIGHT_PIN		    GPIO_PIN_4		// TIM2_CH1		// Moto FAN
#define		LIGHT_GPIO		    GPIOD
#define 	LIGHT_PWM_CH	    TMR2_CHANNEL_1

#define 	SWING_PIN		    GPIO_PIN_5		// Swing Control moto   : 6 to test, 5 to 
#define		SWING_GPIO		    GPIOD

#define 	LED_SWING_PIN	    GPIO_PIN_5		// Led Swing
#define		LED_SWING_GPIO	    GPIOC

#define 	LED_LOW_SPD_PIN	    GPIO_PIN_3		// Led Low Speed
#define		LED_LOW_SPD_GPIO    GPIOD

#define 	LED_HIGH_SPD_PIN	GPIO_PIN_1		// Led High Speed
#define		LED_HIGH_SPD_GPIO	GPIOD

#define 	LED_NATURAL_PIN		GPIO_PIN_7		// Led Neuture Speed
#define		LED_NATURAL_GPIO	GPIOC		

#define 	LED_CHARG_PIN		GPIO_PIN_2		// Led Charge
#define		LED_CHARG_GPIO		GPIOA

#define 	LED_LOWBAT_PIN		GPIO_PIN_6		// Led Battery Low
#define		LED_LOWBAT_GPIO		GPIOD

#define 	BT1_PIN			    GPIO_PIN_3		// BT 1 => FAN Speed
#define		BT1_GPIO		    GPIOC
#define		BT1_READ 		    GPIO_ReadInputBit(BT1_GPIO,BT1_PIN)

#define 	BT2_PIN			    GPIO_PIN_5		// BT 2 => Led Night light
#define		BT2_GPIO		    GPIOB
#define		BT2_READ 		    GPIO_ReadInputBit(BT2_GPIO,BT2_PIN)

#define 	BT3_PIN			    GPIO_PIN_4		// BT 3 => Swing Toggle
#define		BT3_GPIO		    GPIOC
#define		BT3_READ 		    GPIO_ReadInputBit(BT3_GPIO,BT3_PIN)

#define 	PWRIN_PIN		    GPIO_PIN_4		// Power In Detect
#define		PWRIN_GPIO		    GPIOB
#define		PWRIN_READ 		    GPIO_ReadInputBit(PWRIN_GPIO,PWRIN_PIN)




/* ADC Channel definition */
#define BATTERY_ADC_CHANNEL     ADC_CHANNEL_3		// Battery Measure

/* ADC GPIO port and pin */
#define BATTERY_ADC_PORT       	GPIOD
#define BATTERY_ADC_PIN        	GPIO_PIN_2


/* ADC Channel definition */
#define ADC_POWERIN         	ADC_CHANNEL_4	// Power In Measure

/* ADC GPIO port and pin */
#define POWERIN_ADC_PORT   GPIOC
#define POWERIN_ADC_PIN    GPIO_PIN_4


#define LOW_SPEED_DUTY      30
#define HIGH_SPEED_DUTY     80
#define NATURE_LOW_DUTY     40
#define NATURE_HIGH_DUTY    85

#define LOW_LIGHT_DUTY      10
#define HIGH_LIGHT_DUTY     99
#define LIGHT_OFF_DUTY      0


#endif
