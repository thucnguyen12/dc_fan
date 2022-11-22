
#include "IR_Handler.h"


/*
Cach 1: dung cho SPI Led keo dai ko chung.
Cach 2: Dung cho Led 4 so hien thi chung.

*/


#define		MAXIRRANGE		100
#define		MAXIRDATA		32
#define		MAXIRCOMMAND	20

uint8_t		IRCommand[MAXIRCOMMAND];
uint8_t		IRComandIndex = 0;

uint8_t		IRCode[MAXIRDATA];
uint8_t		IRHigh[MAXIRDATA], IR_ClkHigh;
uint8_t		IRLow[MAXIRDATA] , IR_ClkLow;

uint8_t		IRCodeIndex = 0;
uint8_t		HasBegin = 0;

uint32_t	HighSide = 0;
uint32_t	LowSide = 0;
uint8_t		OldIRState = 0;
uint8_t		NewIRData = 0;
uint8_t		MaxIR = 0;

uint32_t	IRTimeOut = 0;
uint32_t	WaitNextCode = 0;

/*******************************************************************************************************/

void Ir_GPIO_Init(void)
{
	GPIO_Config_T gpioConfig;
	
	
    gpioConfig.mode = GPIO_MODE_IN_PU;
    gpioConfig.speed = GPIO_SPEED_10MHz;
    gpioConfig.pin = IR_PIN;
	gpioConfig.intEn = GPIO_EINT_ENABLE;
    GPIO_Config(IR_GPIO, &gpioConfig);
	
	EINT_Config(EINT_PORT_A, EINT_TRIGGER_FALLING_RISING);
	
	NVIC_EnableIRQRequest(EINTA_IRQn,2);
}

void EINTA_IRQHandler(void){
	// Start a bit with Low level
	if(IR_REMOTE_READ == 0)
    {
		if(IRTimeOut > 0)
        {
			IRHigh [IR_ClkHigh++]= HighSide;
		}else {
			// Bit Start Frame
			IR_ClkLow = IR_ClkHigh = 0;
		}
		LowSide = 0;
		IRTimeOut = 50;
	}
	else 
    {
		IRLow[IR_ClkLow++]= LowSide;
		HighSide = 0; 
	}
	
	EINT_ClearIntFlag(EINT_PORT_A);
}

/* Ham nay goi chu ky 0.1 ms */
void	ProcessSignalIrRemote(void)
{
	uint8_t	TinHieuIR= 0;
	
	/* Gia tri cac gia tri muc thap, cao */
	TinHieuIR = IR_REMOTE_READ ;  /* Mach cu */

	if(TinHieuIR)	{HighSide++; } else { LowSide++;}

        /* Chuyen tu trang thai cao xuong thap thi xu ly tin hieu */
	if( (TinHieuIR==0) && OldIRState )
	{
		//UPrintf(USART1,"\rH:%d;L:%d.",HighSide,LowSide);
		XuLyChuKyIR();	
		HighSide = 0;
		LowSide = 0;	
        //        SerialPutChar('+');
    }
	OldIRState = TinHieuIR;	

	/* Reset chu ky moi */
	if(LowSide == 1)	HighSide = 0;	 

	if(HighSide > MAXIRRANGE)
	{
		HasBegin = 0;
		HighSide = 0;
		LowSide = 0;
		NewIRData = 0;
 	}
	
}

/*******************************************************************************************************/
void	XuLyChuKyIR() // dc goi boi 0.1 ms
{
	/* Tin hieu vuot qua khoang cho phep */
	if(HighSide > MAXIRRANGE || LowSide > MAXIRRANGE) return;
	/* Tin hieu bat dau truyen */
	if(LowSide > 80 && LowSide < 100)
		if(HighSide > 37 && HighSide < 52)
		{
		 	IRCodeIndex = 0;
			HasBegin = 1;
			NewIRData = 0;
            return;	
        }	

	if(HasBegin == 0) return;

	/* Xu ly logic 0*/
	if(LowSide  >= 3 && LowSide  <= 7)
	if(HighSide >= 3 && HighSide <= 7)
	{
		IRCode[IRCodeIndex++] = 0;	
    }

	/* Xu ly logic 1*/
	if(LowSide  >=  3 && LowSide  <= 7)
	if(HighSide >= 13 && HighSide <= 25)
	{
	      IRCode[IRCodeIndex++] = 1;	
    }

	/* Xu ly du lieu */
	if(IRCodeIndex >= MAXIRDATA)
	{
		ProcessIRCode();
		HasBegin = 0;
		IRCodeIndex = 0;
 	}
}

/*******************************************************************************************************/
uint16_t		GetIRCommand()
{
	uint8_t	i = 0;
	uint16_t Code = 0;
	//UPrintf(USART1,"\rClk: L:%d-H:%d",IR_ClkLow,IR_ClkHigh);
	for(i = 0; i < 12; i++){
		//UPrintf(USART1,"\r%x L:%d;H:%d",Code,IRLow[i],IRHigh[i]);
		if(   (IRLow[i] >=11  && IRLow[i]<=16)
			//&&(IRHigh[i]>=2   && IRHigh[i]<=6)
		  )
		Code|=(1<<i);
	}

	return Code;
}	

/*******************************************************************************************************/
extern void	ProcessIR(uint8_t KeyCode);
void ProcessIRCode()
{
	uint16_t	Code = 0;
	/* Lay ma phim duoc bam */
	Code = GetIRCommand();
	//UPrintf(USART1,"\rIR:%d.",Code);
	xSystem_para_now.IrCode = Code;
}



void Ir_Timer_Init(void)
{	
	Ir_GPIO_Init();
	// 24Mhz / 3 / 100 = 
	RCM_EnableAPBPeriphClock(RCM_PERIPH_TMR1);
	TMR1_TimeBaseConfig_T timeBaseConfigStruct;
	
	timeBaseConfigStruct.cntMode = TMR1_CNT_MODE_UP;
	timeBaseConfigStruct.count   = 198;
	timeBaseConfigStruct.divider = 5;
	timeBaseConfigStruct.repetitionCount = 1;
	TMR1_ConfigTimerBase(TMR1, &timeBaseConfigStruct);
	
	TMR1_EnableInterrupt(TMR1, TMR1_INT_UPDATE);
	
	NVIC_EnableIRQRequest(TMR1_UT_IRQn, 2);
	
	TMR1_Enable(TMR1);
}


void TMR1_UT_IRQHandler(void){
	LowSide++; HighSide++;
	TMR1_ClearStatusFlag(TMR1,TMR1_FLAG_UPDATE);
	if(IRTimeOut) if(--IRTimeOut==0) ProcessIRCode();
}




