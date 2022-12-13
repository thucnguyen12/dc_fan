#include "adc_handler.h"
extern uint8_t first_time_ADC;
uint8_t this_is_first_time = 1;

void ADCCalibration(void)
{
    uint8_t i;
    int8_t offset = 0;
    uint16_t adcData = 0;
    
    ADC_SetOffset(0);
    ADC_EnableCompensation();
    ADC_ClearStatusFlag(ADC_FLAG_CC);
    ADC_Enable();
    ADC_StartConversion();
    
    for(i = 0; i < 10; i++)
    {
        while(ADC_ReadStatusFlag(ADC_FLAG_CC) == RESET);
        ADC_ClearStatusFlag(ADC_FLAG_CC);
    }
    ADC_Disable();
    
    adcData = ADC_ReadData();    
    offset = (int8_t)(0x800 - adcData);
    ADC_SetOffset(offset);
    
    ADC_DisableCompensation();
    ADC_ClearStatusFlag(ADC_FLAG_CC);
}



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
    adcConfig.dataAlign = ADC_DATA_ALIGN_RIGHT;
    ADC_Config(&adcConfig);

    /** ADC Calibration */
    ADCCalibration();

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
        xSystem_para_now.Vbat = ADC_ReadData(); // Vbat by mV
    }
    if (this_is_first_time == 1)
    {
        this_is_first_time = 0;
        first_time_ADC = 1;
    }
}


