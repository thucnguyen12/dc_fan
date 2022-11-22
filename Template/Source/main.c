/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.1
 *
 * @date        2022-04-11
 *
 * @attention
 *
 *  Copyright (C) 2018-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#include "main.h"
//#include "Board.h"
#include "Hardware.h"
#include "gpio.h"
#include "parameter_define.h"
#include "adc_handler.h"
#include "motor_ctrl.h"
#include "light_control.h"
#include "IR_Handler.h"

/* system para */
sys_parameter xSystem_para_now;

void Tick_100ms_Handler (void);
void Tick_1000ms_check_power (void);


/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
int main(void)
{
    /* Setup SysTick Timer for 1 msec interrupts */
    SysTick_Config(RCM_GetMasterClockFreq() / 1000);
    
    gpio_init();
    ADCInit();
    DC_Moto_Init();
    Light_PWM_Init();
    xSystem_para_now.BT1.delay_scan =0;
    xSystem_para_now.BT2.delay_scan =0;
    xSystem_para_now.BT3.delay_scan =0;
    xSystem_para_now.BT1.thresh = 1;
    xSystem_para_now.BT2.thresh = 1;
    xSystem_para_now.BT3.thresh = 1;
    //turn_on_led (LED_LOW_SPD_GPIO,LED_LOW_SPD_PIN);
    while(1)
    {
        
        if ((xSystem_para_now.Vbat < 9600) && (xSystem_para_now.is_charging == 0)) //if Vbat low and not being charged go into sleep mode 
        {
            xSystem_para_now.in_sleep_mode = 1;
        }
        else if ((xSystem_para_now.Vbat <= 13500) && (xSystem_para_now.is_charging == 1)) // Vbat is not full and charging blink led
        {
            turn_on_led (LED_CHARG_GPIO, LED_CHARG_PIN);
        }
        
        
        if((xSystem_para_now.Tick.Tick_100ms >= 100))// && (xSystem_para_now.in_sleep_mode == 0)) // only check if not in sleep mode
        {
            xSystem_para_now.Tick.Tick_100ms = 0;
            Tick_100ms_Handler ();
        }
        
        if(xSystem_para_now.Tick.Tick_1000ms >= 1000){
            xSystem_para_now.Tick.Tick_1000ms = 0;
            //Check_Tick_1000ms();
            Tick_1000ms_check_power();
        }
    }
}


void scan_button_on_board (void)
{
    //scan button 1
    if((BT1_READ != 0) && (xSystem_para_now.BT1.delay_scan == 0))
    {
        if(xSystem_para_now.BT1.clk_cnt++ > xSystem_para_now.BT1.thresh)
        {
            
            xSystem_para_now.BT1.delay_scan = 2;
            xSystem_para_now.BT1.level = 1;
            xSystem_para_now.BT1.clk_cnt = 0;
        }
    }
    else
    {
        if (xSystem_para_now.BT1.delay_scan)
           xSystem_para_now.BT1.delay_scan--; 
    }
    
    //scan button 2
    if((BT2_READ != 0) && (xSystem_para_now.BT2.delay_scan == 0))
    {
        if(xSystem_para_now.BT2.clk_cnt++ > xSystem_para_now.BT2.thresh)
        {
            xSystem_para_now.BT2.delay_scan = 2;
            xSystem_para_now.BT2.level = 1;
            xSystem_para_now.BT2.clk_cnt = 0;
        }
    }
    else
    {
        if (xSystem_para_now.BT2.delay_scan)
           xSystem_para_now.BT2.delay_scan--; 
    }
    
    //scan button 3
    if((BT3_READ != 0) && (xSystem_para_now.BT3.delay_scan == 0))
    {
        if(xSystem_para_now.BT3.clk_cnt++ > xSystem_para_now.BT3.thresh)
        {
            
            xSystem_para_now.BT3.delay_scan = 2;
            xSystem_para_now.BT3.level = 1;
            xSystem_para_now.BT3.clk_cnt = 0;
        }
    }
    else
    {
        if (xSystem_para_now.BT3.delay_scan)
           xSystem_para_now.BT3.delay_scan--; 
    }
}

/*
    Handler process button every 100ms
*/

void Tick_100ms_Handler (void)
{
    static uint8_t delay_IR = 0;
    scan_button_on_board ();
#warning "need handle ir code"
    if (xSystem_para_now.IrCode != 0 && (delay_IR == 0))
    {
        xSystem_para_now.IrCode = 0;
        delay_IR = 2;
        
        if (xSystem_para_now.IrCode == IR_SWING)
        {
            xSystem_para_now.BT1.level = 1;
        }
        else if (xSystem_para_now.IrCode == IR_SPEED)
        {
            xSystem_para_now.BT2.level = 1;
        }
        else if (xSystem_para_now.IrCode == IR_LIGHT)
        {
            xSystem_para_now.BT3.level = 1;
        }
    }
    
    if (delay_IR)
    {
        xSystem_para_now.IrCode = 0;
        delay_IR--;
    }
    //PROCESS BUTTON PRESS
    if (xSystem_para_now.BT1.level == 1)
    {
        xSystem_para_now.BT1.level = 0; //clear state BT
        
        if (xSystem_para_now.speed > speed_off)
        GPIO_Toggle (SWING_GPIO, SWING_PIN); //changed swing state
        toggle_led (LED_SWING_GPIO, LED_SWING_PIN);
    }
    
    if (xSystem_para_now.BT2.level == 1)
    {
        xSystem_para_now.BT2.level = 0; //clear
        turn_off_leds_speed();
        
        if (xSystem_para_now.speed == speed_off)
        {
            xSystem_para_now.speed = low_spd;
            turn_on_led (LED_LOW_SPD_GPIO, LED_LOW_SPD_PIN);
            DC_SetDuty (LOW_SPEED_DUTY);
        }
        else if (xSystem_para_now.speed == low_spd)
        {
            xSystem_para_now.speed = high_spd;
            turn_on_led (LED_HIGH_SPD_GPIO, LED_HIGH_SPD_PIN);
            DC_SetDuty (HIGH_SPEED_DUTY);
        }
        else if (xSystem_para_now.speed == high_spd)
        {
            xSystem_para_now.speed = nature_spd;
            turn_on_led (LED_NATURAL_GPIO, LED_NATURAL_PIN);
        }
        else if (xSystem_para_now.speed == nature_spd)
        {
            xSystem_para_now.speed = speed_off;
            DC_SetDuty (0);
        }
    }
    
    if (xSystem_para_now.BT3.level == 1)
    {
        xSystem_para_now.BT3.level = 0; //clear
        
        if (xSystem_para_now.lightLevel == 0)
        {
            xSystem_para_now.lightLevel = 1;
            Light_SetDuty (LOW_LIGHT_DUTY);
        }
        else if (xSystem_para_now.lightLevel == 1)
        {
            xSystem_para_now.lightLevel = 2;
            Light_SetDuty (HIGH_LIGHT_DUTY);
        }
        else if (xSystem_para_now.lightLevel == 2)
        {
            xSystem_para_now.lightLevel = 0;
            Light_SetDuty (LIGHT_OFF_DUTY);
        }
    }
    //PROCESS NATURE MODE
    if (xSystem_para_now.speed == nature_spd)
    {
        static uint16_t nature_duty = NATURE_LOW_DUTY;
        static uint8_t light_wind = 0;
        if ((nature_duty < NATURE_HIGH_DUTY) && (!light_wind))
        {
            nature_duty++;
        }
        else
        {
            light_wind = 1;
        }
        
        if ((nature_duty > NATURE_LOW_DUTY) && (light_wind))
        {
            nature_duty--;
        }
        else
        {
            light_wind = 0;
        }
    }
}


void Tick_1000ms_check_power (void)
{
    static uint8_t last_power_in = 0;
    if ((PWRIN_READ !=0) && (last_power_in == 0))
    {
        xSystem_para_now.is_charging = 1;
        last_power_in = 1;
    }
    else if ((PWRIN_READ == 0) && last_power_in)
    {
        xSystem_para_now.is_charging = 0;
        last_power_in = 0;
    }
}

