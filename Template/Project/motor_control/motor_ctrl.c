#include "motor_ctrl.h"


//void DC_Moto_GPIO_Init(void)
//{
//    GPIO_Config_T gpioConfig;

//    gpioConfig.mode 	= GPIO_MODE_OUT_PP;
//    gpioConfig.speed 	= GPIO_SPEED_10MHz;
//    gpioConfig.pin 		= FAN_CTRL_PIN;

//    GPIO_Config(FAN_CTRL_GPIO, &gpioConfig);
//}

void DC_Moto_Init(void)
{
	TMR2_OCConfig_T  ocConfigStruct;
	
//	DC_Moto_GPIO_Init();
	// 24Mhz / 3 / 100 = 
	
	TMR2_ConfigTimerBase(2, DC_MaxCount);	// 1Mhz clock, 1000Hz
	//3.3kHz
    
	// PWM for DC-moto
	ocConfigStruct.channel 			= FAN_PWM_CH;
	ocConfigStruct.mode 			= TMR2_OC_MODE_PWM1;
	ocConfigStruct.OCxOutputState	= TMR2_OC_OUTPUT_ENABLE;
	ocConfigStruct.OCxPolarity		= TMR2_OC_POLARITY_HIGH;
	ocConfigStruct.count 			= 0;
	
	TMR2_ConfigOutputCompare(&ocConfigStruct);
	TMR2_EnableCompareCapture(FAN_PWM_CH);
	
	TMR2_Enable();
}

void DC_SetDuty(uint8_t	Duty){
	uint32_t Value = (uint32_t)DC_MaxCount * (uint32_t)Duty / 100;
//	if(Value==0 || xSystem.IsLowBat==1 ) TMR2_DisableCompareCapture(FAN_PWM_CH);
	if (Value)
    {
        TMR2_SetCompareCapture(FAN_PWM_CH,Value);
        TMR2_EnableCompareCapture(FAN_PWM_CH);
    }
    else
    {
        TMR2_DisableCompareCapture(FAN_PWM_CH);
    }
//	if(Value>0 && xSystem.IsLowBat==0 )  TMR2_EnableCompareCapture(FAN_PWM_CH);
}

