/*!
 * @file        apm32f00x_int.c
 *
 * @brief       Main Interrupt Service Routines
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

#include "apm32f00x_int.h"
#include "main.h"
#include "apm32f00x_gpio.h"
#include "apm32f00x_eint.h"
#include "apm32f00x_misc.h"
#include "Hardware.h"
#include "parameter_define.h"
#include "apm32f00x_wwdt.h"

extern sys_parameter xSystem_para_now;
extern volatile uint32_t m_delay;

 /*!
  * @brief       This function handles NMI exception
  *
  * @param       None
  *
  * @retval      None
  *
  * @note
  */
 void NMI_Handler(void)
 {
 }

/*!
 * @brief       This function handles Hard Fault exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void HardFault_Handler(void)
{

}

/*!
 * @brief       This function handles SVCall exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SVC_Handler(void)
{
}

/*!
 * @brief       This function handles PendSV_Handler exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief       This function handles SysTick exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SysTick_Handler(void)
{
    xSystem_para_now.Tick.Tick_100ms++;
    xSystem_para_now.Tick.Tick_1000ms++;
    xSystem_para_now.Tick.Tick_200ms++;
    xSystem_para_now.Tick.Tick_10ms++;
    xSystem_para_now.Tick.Tick_1ms++;
    
    if (m_delay)
    {
        m_delay--;
    }
    if (xSystem_para_now.led.led_counter)
    {
        xSystem_para_now.led.led_counter--;
    }
    

}


/*!
 * @brief       This function handles ADC exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note       
 */
void ADCIsr(void);
void ADC_IRQHandler(void)
{
    ADCIsr();
}
