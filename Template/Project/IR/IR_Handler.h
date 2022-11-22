
#ifndef __IR_REMOTE_H__
#define __IR_REMOTE_H__


#define		IR_LIGHT	1057
#define		IR_SWING	545	
#define		IR_SPEED 	2081	




#include "apm32f00x.h"
#include "apm32f00x_usart.h"
#include "apm32f00x_gpio.h"
#include "apm32f00x_rcm.h"
#include "Hardware.h"

#include "apm32f00x_tmr1.h"
#include "apm32f00x_misc.h"
#include "apm32f00x_eint.h"
#include "parameter_define.h"



void 	IrRemote_Init(void);
void	ProcessSignalIrRemote(void);
void 	ProcessIRCode(void);
void	IR_Tick(void);
void 	Beeps(unsigned char n);
void	XuLyChuKyIR(void);
void 	Ir_Timer_Init(void);


#endif
