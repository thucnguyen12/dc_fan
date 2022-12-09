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

#include "apm32f00x_iwdt.h"
#include "apm32f00x_rcm.h"
#include "apm32f00x_wupt.h"
#include "apm32f00x_tmr1.h"
#include "Hardware.h"
#include "gpio.h"
#include "parameter_define.h"
#include "adc_handler.h"
#include "motor_ctrl.h"
#include "light_control.h"
#include "IR_Handler.h"
#include "uart.h"
#include "apm32f00x_wwdt.h"

/* system para */
sys_parameter xSystem_para_now;
volatile uint32_t m_delay = 0;
uint8_t ADC_arr [15];
/** WWDT counter */
#define WWDT_COUNTER_VALUE          (122)
/** WWDT window value */
#define WWDT_WINDOW_VALUE           (100)


void Tick_100ms_Handler (void);
void Tick_1000ms_check_power (void);
void delay_time_1ms(uint32_t count);
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
    turn_off_leds_speed();
    turn_off_led (LED_FULLBAT_GPIO, LED_FULLBAT_PIN);
}

/*!
 * @brief       Measure the LIRC frequency
 *
 * @param       None
 *
 * @retval      LIRC frequency in HZ
 *
 * @note
 */
uint32_t LIRCMeasurment(void)
{
    uint16_t icValue[2];
    uint32_t freqLIRC;
    uint32_t freqMasterClk;
    TMR1_ICConfig_T icConfig;

    freqMasterClk = RCM_GetMasterClockFreq();

    WUPT_EnableLIRCMeasurement();

    icConfig.channel = TMR1_CHANNEL_1;
    icConfig.div = TMR1_IC_DIV_8;
    icConfig.filter = 0;
    icConfig.polarity = TMR1_IC_POLARITY_FALLING;
    icConfig.selection = TMR1_IC_SELECT_DIRECTTI;
    TMR1_ConfigInputCapture(TMR1, &icConfig);
    TMR1_Enable(TMR1);

    while(TMR1_ReadStatusFlag(TMR1, TMR1_FLAG_CH1CC) == RESET);
    icValue[0] = TMR1_ReadCompareCapture(TMR1, TMR1_CHANNEL_1);
    TMR1_ClearStatusFlag(TMR1, TMR1_FLAG_CH1CC);

    while(TMR1_ReadStatusFlag(TMR1, TMR1_FLAG_CH1CC) == RESET);
    icValue[1] = TMR1_ReadCompareCapture(TMR1, TMR1_CHANNEL_1);
    TMR1_ClearStatusFlag(TMR1, TMR1_FLAG_CH1CC);

    TMR1_DisableCompareCapture(TMR1, TMR1_CHANNEL_1);
    TMR1_Disable(TMR1);

    freqLIRC = (8 * freqMasterClk) / (icValue[1] - icValue[0]);

    WUPT_DisableLIRCMeasurement();

    return freqLIRC;
}


/*!
 * @brief       IWDT init.Set IWDT timeout = 0.5s
 *
 * @param       freqHIRC:    LIRC frequency in HZ
 *
 * @retval      None
 *
 * @note
 */
void IWDTInit(uint32_t freqLIRC)
{
    IWDT_Enable();

    IWDT_EnableWriteAccess();
    /**
        IWDT timeout = 0.5S = (Counter Reload Value * DIV) / freqLIRC;

        Counter Reload Value = (freqLIRC * 0.5) / DIV
                             = freqLIRC / (2 * DIV)
    */
    IWDT_SetDivider(IWDT_DIV_256);
    IWDT_SetReloadCounter(freqLIRC / 512);

    IWDT_ReloadCounter();
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
    delay_time_1ms (8000);
    /**
    WWDT Clock = 48MHZ;
    Watchdog Window     = (WWDT_COUNTER_VALUE - 63) * 1 step
                        = (122 - 63) * (12288 / 48)
                        =~ 15ms

    Non Allowed Window  = (WWDT_COUNTER_VALUE - WWDT_WINDOW_VALUE) * 1 step
                        = (122 - 90) * (12288 / 48)
                        =~ 5.6ms
    */
    WWDT_Config(WWDT_COUNTER_VALUE, WWDT_WINDOW_VALUE);
    WWDT_SetCounter(WWDT_COUNTER_VALUE);
    uint32_t freqLIRC;
    freqLIRC = LIRCMeasurment();
    IWDTInit(freqLIRC);
    
    
    gpio_init();
//    InitUART(USART1,115200);
//    UPrintf(USART1,"=====================\r\n");
//	UPrintf(USART1,"AMP32F003 Started....\r\n");
//	UPrintf(USART1,"=====================\r\n");
    ADCInit();
    DC_Moto_Init();
    Light_PWM_Init();
    Ir_Timer_Init();
    init_system_default ();
    xSystem_para_now.BT1.delay_scan =0;
    xSystem_para_now.BT2.delay_scan =0;
    xSystem_para_now.BT3.delay_scan =0;
    xSystem_para_now.BT1.thresh = 1;
    xSystem_para_now.BT2.thresh = 1;
    xSystem_para_now.BT3.thresh = 1;
    
    WWDT_SetCounter(WWDT_COUNTER_VALUE);
    xSystem_para_now.Tick.Tick_1ms = 0;
    while(1)
    {
        if (xSystem_para_now.Tick.Tick_1ms > 10) // (15.1 - 5.6)
        {
            xSystem_para_now.Tick.Tick_1ms = 0;
            WWDT_SetCounter(WWDT_COUNTER_VALUE);
        } 
//        static uint8_t i = 0;
        IWDT_ReloadCounter();
           
        
        if (xSystem_para_now.Tick.Tick_10ms >= 500)
        {
            xSystem_para_now.Tick.Tick_10ms = 0;
            static uint8_t i = 0;
            ADC_arr[i++] = xSystem_para_now.Vbat;
            if (i == 15) i = 0;
        }
        if((xSystem_para_now.Tick.Tick_100ms >= 100) && (xSystem_para_now.in_sleep_mode == 0)) // only check if not in sleep mode
        {
            //for wdt test
//            i++;
//            if (i == 20)
//            {
//                delay_time_1ms (2048);
//            }
            
            xSystem_para_now.Tick.Tick_100ms = 0;
            Tick_100ms_Handler ();
        }
        
        if(xSystem_para_now.Tick.Tick_1000ms >= 1000){
            xSystem_para_now.Tick.Tick_1000ms = 0;
            Tick_1000ms_check_power();
//            UPrintf(USART1,"Still run On!");
        }
        

        if (xSystem_para_now.Tick.Tick_200ms >= 10)
        {
            xSystem_para_now.Tick.Tick_200ms = 0;
            if (xSystem_para_now.in_sleep_mode && (xSystem_para_now.is_charging == 0))
            {
                //scan but only process button2
                scan_button_on_board ();
                scan_IR_Code ();
                
                gpio_deinit_in_sleep_mode ();
                if (xSystem_para_now.BT1.level || xSystem_para_now.BT2.level || xSystem_para_now.BT3.level)
                {
                    xSystem_para_now.BT1.level = 0;
                    xSystem_para_now.BT2.level = 0;
                    xSystem_para_now.BT3.level = 0;
                    
                    // blink led low baterry 3 time
                    turn_off_led (LED_FULLBAT_GPIO, LED_FULLBAT_PIN);
                    turn_on_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
                    delay_time_1ms (1000);
                    turn_off_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
                }
            }
            
        }
    }
}


void scan_button_on_board (void)
{
    //scan button 1
    if((BT1_READ != 0) && (xSystem_para_now.BT1.delay_scan == 0) && (xSystem_para_now.BT1.last_state == 0))
    {
        if(xSystem_para_now.BT1.clk_cnt++ > xSystem_para_now.BT1.thresh)
        {
            
            xSystem_para_now.BT1.delay_scan = 1;
            xSystem_para_now.BT1.level = 1;
            xSystem_para_now.BT1.clk_cnt = 0;
            xSystem_para_now.BT1.last_state = BT1_READ;
        }
    }
    else
    {
        if (xSystem_para_now.BT1.delay_scan)
           xSystem_para_now.BT1.delay_scan--;
        if (BT1_READ != xSystem_para_now.BT1.last_state)
           xSystem_para_now.BT1.last_state = BT1_READ;
    }
    
    //scan button 2
    if((BT2_READ != 0) && (xSystem_para_now.BT2.delay_scan == 0) && (xSystem_para_now.BT2.last_state == 0))
    {
        if(xSystem_para_now.BT2.clk_cnt++ > xSystem_para_now.BT2.thresh)
        {
            xSystem_para_now.BT2.delay_scan = 1;
            xSystem_para_now.BT2.level = 1;
            xSystem_para_now.BT2.clk_cnt = 0;
            xSystem_para_now.BT2.last_state = BT2_READ;
        }
    }
    else
    {
        if (xSystem_para_now.BT2.delay_scan)
           xSystem_para_now.BT2.delay_scan--;
        if (BT2_READ != xSystem_para_now.BT2.last_state)
           xSystem_para_now.BT2.last_state = BT2_READ;
    }
    
    //scan button 3
    if((BT3_READ != 0) && (xSystem_para_now.BT3.delay_scan == 0) && (xSystem_para_now.BT3.last_state == 0))
    {
        if(xSystem_para_now.BT3.clk_cnt++ > xSystem_para_now.BT3.thresh)
        {
            
            xSystem_para_now.BT3.delay_scan = 1;
            xSystem_para_now.BT3.level = 1;
            xSystem_para_now.BT3.clk_cnt = 0;
            xSystem_para_now.BT3.last_state = BT3_READ;
        }
    }
    else
    {
        if (xSystem_para_now.BT3.delay_scan)
           xSystem_para_now.BT3.delay_scan--;
        if (BT3_READ != xSystem_para_now.BT3.last_state)
           xSystem_para_now.BT3.last_state = BT3_READ;
    }
}

void scan_IR_Code (void)
{
    static uint8_t delay_IR = 0;
    if (xSystem_para_now.IrCode != 0 && (delay_IR == 0))
    {
//        UPrintf(USART1,"Scan Ir code now : %d\r\n", xSystem_para_now.IrCode);
        
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
        
        xSystem_para_now.IrCode = 0;
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
    uint8_t divider = 0;
    static uint16_t Vbat_avr;
    for (uint8_t i = 0; i < 15; i++)
    {
        Vbat_avr += ADC_arr[i];
        if (ADC_arr[i] == 0)
        {
            divider = i;
            break;
        }
        divider = i;
    }
    Vbat_avr = Vbat_avr / divider;
    mVolOfBat = 500 * Vbat_avr /4095;
    mVolOfBat = mVolOfBat * 431 / 100;

//    UPrintf(USART1,"VBat: %d~%d.%dV, -> mVbat: %d\r\n",xSystem_para_now.Vbat, mVolOfBat/100,mVolOfBat/10%10, mVolOfBat);
    
    if ((mVolOfBat <= 956) && (xSystem_para_now.is_charging == 0) && (xSystem_para_now.in_sleep_mode == 0)) //9v6 and not charging
    {
        if (++ClkLowBat > 7)
        {
            if(ClkLowBat > 9) ClkLowBat = 9;
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
            
            turn_off_led (LED_FULLBAT_GPIO, LED_FULLBAT_PIN);
            turn_on_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
            delay_time_1ms (1000);
            toggle_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
            delay_time_1ms (1000);
            toggle_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
            delay_time_1ms (1000);
            toggle_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
            delay_time_1ms (1000);
            toggle_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
            delay_time_1ms (1000);
            toggle_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
            
        }
    }
    else
    {
        if (mVolOfBat >= 1050) // if (Vbat > 10,5V) or (Vbat > 9.6V and wakeup button pressed) then get out sleep mode
        {
            if (++ClkOutLowBatState > 5)
            {
                //reset state low bat and finish sleep mode
                ClkOutLowBatState = 0;
                ClkLowBat = 0;
                xSystem_para_now.in_sleep_mode = 0;
            }
            
        }
        
        if (mVolOfBat > 1420 && (xSystem_para_now.is_charging == 1)) // if v >14.4 and plug in
        {
            if (PWRIN_READ != 0)
            {
                if(++ClkFullBat > 20) {
                    if(ClkFullBat > 25) 
                        ClkFullBat = 25;
//                    UPrintf (USART1, " BATERRY IS FULL NOW\r\n");
                    xSystem_para_now.is_charging = 0; // BATERRY IS FULL
                    turn_off_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
                    turn_on_led (LED_FULLBAT_GPIO, LED_FULLBAT_PIN); //LED GREEN INDICATE FULL BAT ALWAYS ON
                }
            }
        }
        else if (mVolOfBat < 1420)
        {
            if (ClkFullBat > 0)
            {
                if (--ClkFullBat)
                {
                    if (PWRIN_READ != 0)
                    {
//                        UPrintf(USART1,"PLUG IN AND BATERRY IS NOT FULL!\r\n");
                        xSystem_para_now.is_charging = 1; // PLUG IN AND BATERRY IS NOT FULL
                        turn_off_led (LED_FULLBAT_GPIO, LED_FULLBAT_PIN);
                        turn_on_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
                    }
                }                
            }
        }
    }
    
    // LED indicate plug in and plug out
    if ((PWRIN_READ !=0) && (last_power_in == 0))
    {
        turn_off_led (LED_FULLBAT_GPIO, LED_FULLBAT_PIN);
        turn_on_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
        xSystem_para_now.is_charging = 1;
        last_power_in = 1;
//        UPrintf(USART1,"PWRIN On! plug in\r\n");
    }
    else if ((PWRIN_READ == 0) && (last_power_in == 1))
    {
        turn_off_led (LED_FULLBAT_GPIO, LED_FULLBAT_PIN);
        turn_off_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
        xSystem_para_now.is_charging = 0;
        last_power_in = 0;
//        UPrintf(USART1,"PWRIN Off! plug out\r\n");
    }
}

/*
    delay_1ms
*/
void delay_time_1ms(uint32_t count)
{
    m_delay = count;
    while (m_delay)
    {
        if (WWDT_ReadCounter() >= WWDT_WINDOW_VALUE)
        {
           WWDT_SetCounter(WWDT_COUNTER_VALUE);
        }
       
        IWDT_ReloadCounter();
    }
    
}
