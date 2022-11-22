#include "adc_handler.h"


/*!
 * @brief       ADC Init   
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void ADCInit(void)
{
    ADC_Config_T adcConfig;
    GPIO_Config_T gpioConfig;
    
    /** ADC GPIO configuration */
    gpioConfig.intEn = GPIO_EINT_DISABLE;
    gpioConfig.mode = GPIO_MODE_IN_FLOATING;
    gpioConfig.pin = BATTERY_ADC_PIN;
    GPIO_Config(BATTERY_ADC_PORT, &gpioConfig);
    
    ADC_SetMode(ADC_MODE_SINGLE_END);
    
    /** ADC configuration */
    ADC_ConfigStructInit(&adcConfig);
    adcConfig.channel = BATTERY_ADC_CHANNEL;
    adcConfig.convMode = ADC_CONV_MODE_CONTINUOUS;
    adcConfig.interrupt = ADC_INT_CC;
	adcConfig.div = ADC_DIV_12;
    ADC_Config(&adcConfig);

    /** ADC Calibration 
    ADCCalibration();*/

//	adcConfig.convMode = ADC_CONV_MODE_SINGLE;
//    ADC_Config(&adcConfig);

    NVIC_EnableIRQRequest(ADC_IRQn, 0X05);
    ADC_Enable();
    ADC_StartConversion();
}

/*!
 * @brief       ADC interrupt service routine   
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void ADCIsr(void){
    if(ADC_ReadIntFlag(ADC_INT_FLAG_CC) == SET){
        ADC_ClearIntFlag(ADC_INT_FLAG_CC);
        xSystem_para_now.Vbat = ADC_ReadData() * 5000 / 4095; // Vbat by mV
    }
}


