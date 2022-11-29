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
#include "uart.h"

/* system para */
sys_parameter xSystem_para_now;
uint8_t ADC_arr [20];
uint8_t voltage_arr [10];

void Tick_100ms_Handler (void);
void Tick_1000ms_check_power (void);
void delay_time(uint32_t count);
void check_wake_up_event (void);
void scan_button_on_board (void);
void scan_IR_Code (void);

void init_system_default (void)
{
    xSystem_para_now.BT1.level = 0;
    xSystem_para_now.BT2.level = 0;
    xSystem_para_now.BT3.level = 0;
    xSystem_para_now.BT1.thresh = 1;
    xSystem_para_now.BT2.thresh = 1;
    xSystem_para_now.BT3.thresh = 1;
    xSystem_para_now.in_sleep_mode = 0;
    xSystem_para_now.IrCode = 0;
    xSystem_para_now.is_charging = 0;
    xSystem_para_now.speed = speed_off;
    xSystem_para_now.SystemStatus = SYST_OFF;
}

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
    delay_time (409600);
    
    
    gpio_init();
//    InitUART(USART1,115200);
//    UPrintf(USART1,"=====================\r\n");
//	UPrintf(USART1,"AMP32F003 Started....\r\n");
//	UPrintf(USART1,"=====================\r\n");
    ADCInit();
    DC_Moto_Init();
    Light_PWM_Init();
    init_system_default ();
    xSystem_para_now.BT1.delay_scan =0;
    xSystem_para_now.BT2.delay_scan =0;
    xSystem_para_now.BT3.delay_scan =0;
    xSystem_para_now.BT1.thresh = 1;
    xSystem_para_now.BT2.thresh = 1;
    xSystem_para_now.BT3.thresh = 1;
    //turn_on_led (LED_LOW_SPD_GPIO,LED_LOW_SPD_PIN);
    while(1)
    {
        if (xSystem_para_now.Tick.Tick_10ms >= 50)
        {
            xSystem_para_now.Tick.Tick_10ms = 0;
            static uint8_t i = 0;
            ADC_arr[i++] = xSystem_para_now.Vbat;
            if (i == 20) i = 0;
        }
        
        if((xSystem_para_now.Tick.Tick_100ms >= 100) && (xSystem_para_now.in_sleep_mode == 0)) // only check if not in sleep mode
        {
            xSystem_para_now.Tick.Tick_100ms = 0;
            Tick_100ms_Handler ();
        }
        
        if(xSystem_para_now.Tick.Tick_1000ms >= 1000){
            xSystem_para_now.Tick.Tick_1000ms = 0;
            //Check_Tick_1000ms();
            Tick_1000ms_check_power();
//            UPrintf(USART1,"Still run On!");
        }
        
//        if (xSystem_para_now.in_sleep_mode)
//        {
//            void gpio_deinit_in_sleep_mode ();
//            scan_button_on_board ();
//            scan_IR_Code();
//            DC_SetDuty (0);
//            if (xSystem_para_now.lightLevel > 1)
//            { 
//                xSystem_para_now.lightLevel = 1;
//                Light_SetDuty (LOW_LIGHT_DUTY);
//            }
//            
//            if (xSystem_para_now.BT2.level == 1)
//            { 
//                
//            }
//            check_wake_up_event ();
//        }
        if (xSystem_para_now.Tick.Tick_200ms >= 200)
        {
            xSystem_para_now.Tick.Tick_200ms = 0;
            if (xSystem_para_now.in_sleep_mode && (xSystem_para_now.is_charging == 0))
            {
                //scan but only process button2
                scan_button_on_board ();
                scan_IR_Code ();
                
                gpio_deinit_in_sleep_mode ();
                if (xSystem_para_now.BT1.level || xSystem_para_now.BT3.level)
                {
                    xSystem_para_now.BT1.level = 0;
                    // blink led low baterry 3 time
                    turn_on_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
                    delay_time (1000);
                    toggle_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
                    delay_time (1000);
                    toggle_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
                    delay_time (1000);
                    toggle_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
                    
                }
            }
            
            if (xSystem_para_now.is_charging)
            {
                toggle_led (LED_CHARG_GPIO, LED_CHARG_PIN); //blink led to indicate charging event
            }
        }
    }
}


//void check_wake_up_event (void)
//{
//    if ((xSystem_para_now.BT1.level == 1) || (xSystem_para_now.IrCode == IR_SPEED))
//    {
//        if (xSystem_para_now.Vbat > 9600)
//        {
//            xSystem_para_now.in_sleep_mode = 0;
//            init_system_default ();
//            turn_on_led (LED_SWING_GPIO, LED_SWING_PIN);
//            //init gpio again
//        }
//        else
//        {
//            toggle_led (LED_CHARG_GPIO,  LED_CHARG_PIN);
//           
//        }
//    }
//    
//}


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

void scan_IR_Code (void)
{
    static uint8_t delay_IR = 0;
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
}

/*
    Handler process button every 100ms
*/

void Tick_100ms_Handler (void)
{
//    static uint8_t delay_IR = 0;

    scan_button_on_board ();
#warning "need handle ir code"
//    if (xSystem_para_now.IrCode != 0 && (delay_IR == 0))
//    {
//        xSystem_para_now.IrCode = 0;
//        delay_IR = 2;
//        
//        if (xSystem_para_now.IrCode == IR_SWING)
//        {
//            xSystem_para_now.BT1.level = 1;
//        }
//        else if (xSystem_para_now.IrCode == IR_SPEED)
//        {
//            xSystem_para_now.BT2.level = 1;
//        }
//        else if (xSystem_para_now.IrCode == IR_LIGHT)
//        {
//            xSystem_para_now.BT3.level = 1;
//        }
//    }
//    
//    if (delay_IR)
//    {
//        xSystem_para_now.IrCode = 0;
//        delay_IR--;
//    }

    scan_IR_Code ();
    
    //PROCESS BUTTON PRESS
    if (xSystem_para_now.BT1.level == 1)
    {
        xSystem_para_now.BT1.level = 0; //clear state BT
        
        if (xSystem_para_now.speed > speed_off)
        {
            GPIO_Toggle (SWING_GPIO, SWING_PIN); //changed swing state
            toggle_led (LED_SWING_GPIO, LED_SWING_PIN);
        }
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
            GPIO_ClearBit (SWING_GPIO, SWING_PIN);
            turn_off_led (LED_SWING_GPIO, LED_SWING_PIN);
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
        DC_SetDuty (nature_duty);
    }
}


void Tick_1000ms_check_power (void)
{
    static uint8_t last_power_in = 0;
    static uint8_t ClkLowBat = 0, ClkFullBat = 0, ClkOutLowBatState = 0;
    uint32_t mVolOfBat;
    static uint16_t Vbat_avr;
    for (uint8_t i = 0; i < 20; i++)
    {
        Vbat_avr += ADC_arr[i];
    }
    Vbat_avr = Vbat_avr / 20;
    
    if (Vbat_avr <= 20)
    {
       mVolOfBat = (Vbat_avr * 10000) / 2010; //check low bat
    }
    else if (Vbat_avr > 20 && (Vbat_avr <= 25))
    {
       mVolOfBat = (Vbat_avr * 10000) / 1609; // check full bat
    }
    else if(Vbat_avr > 30)
    {
        mVolOfBat = (Vbat_avr * 10000) / 2609; // not all
    }
    else
    {
        mVolOfBat = (Vbat_avr * 10000) / 2097; // not all
    }
       
//    UPrintf(USART1,"VBat: %d~%d.%dV, -> mVbat: %d\r\n",Vbat_avr, mVolOfBat/10,mVolOfBat%10, mVolOfBat);
    
    if ((mVolOfBat <= 96) && (xSystem_para_now.is_charging == 0)) //9v6 and not charging
    {
        if (++ClkLowBat > 20)
        {
            if(ClkLowBat>25) ClkLowBat=25;
            xSystem_para_now.in_sleep_mode = 1;
            xSystem_para_now.BT1.level = 0;
            xSystem_para_now.BT2.level = 0;
            xSystem_para_now.BT3.level = 0;
            xSystem_para_now.lightLevel = 0;
            
            DC_SetDuty(0);
            xSystem_para_now.speed = speed_off;
            Light_SetDuty (0);
            turn_off_leds_speed ();
            GPIO_ClearBit (SWING_GPIO, SWING_PIN);
            turn_off_led (LED_SWING_GPIO, LED_SWING_PIN);
            turn_on_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
            delay_time (1000);
            toggle_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
            delay_time (1000);
            toggle_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
            delay_time (1000);
            toggle_led (LED_LOWBAT_GPIO, LED_LOWBAT_PIN);
            
        }
    }
    else
    {
        if (mVolOfBat >= 105) // if (Vbat > 10,5V) or (Vbat > 9.6V and wakeup button pressed) then get out sleep mode
        {
            if (++ClkOutLowBatState > 15)
            {
                //reset state low bat and finish sleep mode
                ClkOutLowBatState = 0;
                ClkLowBat = 0;
                xSystem_para_now.in_sleep_mode = 0;
            }
            
        }
        
        if (mVolOfBat > 140)
        {
            if (PWRIN_READ != 0)
            {
                if(++ClkFullBat > 80) {
                    if(ClkFullBat > 85) 
                        ClkFullBat = 85;
                    xSystem_para_now.is_charging = 0; // BATERRY IS FULL
                    turn_on_led (LED_CHARG_GPIO, LED_CHARG_PIN); //LED ALWAYS ON
                }
            }
        }
        else
        {
            if (ClkFullBat > 0)
            {
                if (--ClkFullBat)
                {
                    if (PWRIN_READ != 0)
                    {
                        xSystem_para_now.is_charging = 1; // PLUG IN AND BATERRY IS NOT FULL
                    }
                }                
            }
        }
    }
    
    // LED indicate plug in and plug out
    if ((PWRIN_READ !=0) && (last_power_in == 0))
    {
        turn_on_led (LED_CHARG_GPIO, LED_CHARG_PIN);
        xSystem_para_now.is_charging = 1;
        last_power_in = 1;
//        UPrintf(USART1,"\rPWRIN On!");
    }
    else if ((PWRIN_READ == 0) && last_power_in)
    {
        turn_off_led (LED_CHARG_GPIO, LED_CHARG_PIN);
        xSystem_para_now.is_charging = 0;
        last_power_in = 0;
//        UPrintf(USART1,"\rPWRIN Off!");
    }
}

void delay_time(uint32_t count)
{
    while (count--);
}
