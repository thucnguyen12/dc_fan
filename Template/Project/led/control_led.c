#include "control_led.h"

static uint32_t count_now;

void led_on_time(uint32_t led_tick_count, uint8_t toggle_time)
{
    xSystem_para_now.led.led_counter = led_tick_count;
    xSystem_para_now.led.toggle_time = toggle_time;
    count_now = led_tick_count;
}

void led_status_control (void)
{
    if (xSystem_para_now.led.toggle_time)
    {
        if (xSystem_para_now.led.led_counter == 0)
        {
            xSystem_para_now.led.toggle_time--;
            if ((xSystem_para_now.led.toggle_time % 2) == 0)
            {
                turn_on_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
                xSystem_para_now.led.led_counter = count_now;
            }
            else if ((xSystem_para_now.led.toggle_time % 2) == 1)
            {
                turn_off_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
                xSystem_para_now.led.led_counter = count_now;
            }
        }
    }
    else
    {
        if (xSystem_para_now.led.led_counter == 0)
        {
            turn_off_led (LED_CHARG_AND_LOWBAT_GPIO, LED_CHARG_AND_LOWBAT_PIN);
        }
    }
}
