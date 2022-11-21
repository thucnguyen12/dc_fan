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


/* system para*/
sys_parameter xSystem_para_now;

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
    gpio_init();
    
    while(1)
    {
        if (xSystem_para_now.Vbat > 9600) //only check if vbat > 9.6V
        {
            if(xSystem_para_now.Tick.Tick_100ms >= 100){
                xSystem_para_now.Tick.Tick_100ms = 0;
                //Check_Tick_100ms();
            }
            
            if(xSystem_para_now.Tick.Tick_1000ms >= 1000){
                xSystem_para_now.Tick.Tick_1000ms = 0;
                //Check_Tick_1000ms();
            }
        }
    }
}
