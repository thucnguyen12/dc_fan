#include "light_control.h"

#define Light_MaxCount	200

void Light_PWM_Init(void)
{
	TMR2_OCConfig_T  ocConfigStruct;
	
//	LED_GPIO_Init();
	
	// Add PWM Channel 3 to TMR2
	ocConfigStruct.channel 			= LIGHT_PWM_CH;
	ocConfigStruct.mode 			= TMR2_OC_MODE_PWM1;
	ocConfigStruct.OCxOutputState	= TMR2_OC_OUTPUT_ENABLE;
	ocConfigStruct.OCxPolarity		= TMR2_OC_POLARITY_HIGH;
	ocConfigStruct.count 			= 0;
	
	TMR2_ConfigOutputCompare(&ocConfigStruct);
	TMR2_EnableCompareCapture(LIGHT_PWM_CH);
	TMR2_Enable();
}

void Light_SetDuty(uint8_t	Duty)
{
	uint32_t Value = (uint32_t)Light_MaxCount * (uint32_t)Duty / 100;
	TMR2_SetCompareCapture(LIGHT_PWM_CH,Value);
}
