/*
 * @file           : ProyectoFinal_Main.c
 * @author         : mmalgeri
 * @brief          : Solar Tracker
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <stm32f4xx.h>

#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "PwmDriver.h"
#include "USARTxDriver.h"

#include "SPA.h"


//Definicion de los elementos para el calcul SPA ------------------------------------------------------------

spa_data spa;  //declare the SPA structure
int result;
float min, sec;

// Definicion de los handlers necesarios --------------------------------------------------------------------


//Elementos para el Blinky

GPIO_Handler_t handlerBlinkyPin        = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

GPIO_Handler_t handlerBlinkyBoardPin        = {0};
BasicTimer_Handler_t handlerBlinkyBoardTimer = {0};


GPIO_Handler_t handlerUserButton       = {0};
EXTI_Config_t handlerUserButtonExti    = {0};




//Elementos para hacer la comunicacion serial ---------------------------------------------------------------

GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0 ;



//Elementos para el PWM -------------------------------------------------------------------------------------

//Servo1
GPIO_Handler_t handlerPin1PwmChannel  = {0};
PWM_Handler_t handlerSignal1PWM      = {0};
uint16_t duttyValue1=1600;

//Servo2
GPIO_Handler_t handlerPin2PwmChannel  = {0};
PWM_Handler_t handlerSignal2PWM      = {0};
uint16_t duttyValue2=500;



char bufferMsg[64] = {0};


//Definicion de las cabeceras de las funciones del main
void initSystem(void);
int16_t convertToDutty1 (double angle);


int main (void)
{
	//Inicialisamos todos los elementos del sistema
	initSystem();




	//Loop forever
	while (1)
	{
		//Verificando el PWM

				if (usart2DataReceived != '\0')
				{
					if (usart2DataReceived == 'M')
					{
						//enter required input values into SPA structure
						spa.year          = 2003;
						spa.month         = 10;
						spa.day           = 17;
						spa.hour          = 12;
						spa.minute        = 30;
						spa.second        = 30;
						spa.timezone      = -7.0;
						spa.delta_ut1     = 0;
						spa.delta_t       = 67;
						spa.longitude     = -105.1786;
						spa.latitude      = 39.742476;
						spa.elevation     = 1830.14;
						spa.pressure      = 820;
						spa.temperature   = 11;
						spa.slope         = 30;
						spa.azm_rotation  = -10;
						spa.atmos_refract = 0.5667;
						spa.function      = SPA_ALL;

						//call the SPA calculate function and pass the SPA structure
						result = spa_calculate(&spa);

						if (result == 0)  //check for SPA errors
							{
								//display the results inside the SPA structure (zenith and azimuth)
								sprintf(bufferMsg, "Zenith:       %02d \n", (int)(spa.zenith));
								writeMsg(&usart2Comm, bufferMsg);

								if (convertToDutty1(spa.zenith)>450)
								{
									duttyValue1= convertToDutty1(spa.zenith);
								}
								else
								{
									duttyValue1= 450;
								}

								updateDuttyCycle(&handlerSignal1PWM, duttyValue1);

								sprintf(bufferMsg, "Dutty asociado al Zenith:       %02d \n", (int)(duttyValue1));
								writeMsg(&usart2Comm, bufferMsg);



								sprintf(bufferMsg, "Azimuth:       %02d \n", (int)(spa.azimuth));
								writeMsg(&usart2Comm, bufferMsg);
							}

						else
							{
								sprintf(bufferMsg, "SPA Error Code:%d\n", result);
								writeMsg(&usart2Comm, bufferMsg);
							}

						//Down
						duttyValue1 -= 10 ;
						updateDuttyCycle(&handlerSignal1PWM, duttyValue1);
					}

					//Cambiamos el estado del elemento que controla la entrada
					usart2DataReceived = '\0';

				}

	}



	return 0;

}

void initSystem(void)
{

	//Configuracion el pin para el LED_Blinky
	handlerBlinkyPin.pGPIOx                             = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerBlinkyPin);


	//Configuracion el pin para el LEDBlinky on la board
	handlerBlinkyBoardPin.pGPIOx                             = GPIOC;
	handlerBlinkyBoardPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_8;
	handlerBlinkyBoardPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBlinkyBoardPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyBoardPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinkyBoardPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerBlinkyBoardPin);


	//Configurando el Timer2 para que funcione el blinky
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;
	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerBlinkyTimer);

	//EL pin UserButton es una entrada simple que entragara la interrupcion EXTI-13
	//Observar que el pin seleccionado es el PIN13, por lo tanto el callback
	//que se debe configurar es el callback_extInt13()
	handlerUserButton.pGPIOx                             = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber      = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	// Cargamos la configuración del pinC13
	GPIO_Config(&handlerUserButton);

	handlerUserButtonExti.pGPIOHandler		=&handlerUserButton; //Puntero a la esctructura
	handlerUserButtonExti.edgeType          =EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);

	// Configuracion de la comunicacion serial
	handlerPinTX.pGPIOx 							= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx 							= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx                         = USART2;
	usart2Comm.USART_Config.USART_baudrate 		= USART_BAUDRATE_19200;
	usart2Comm.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity 		= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_mode 			= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
//usart2Comm.USART_Config.USART_enableIntRX
//usart2Comm.USART_Config.USART_enableIntTX
	//Cargando la configuracion del usart2
	USART_Config(&usart2Comm);


	// PWM --------------------------------------------------------------------------------------------------

	// SERVO 1 ----------------------------------------------------------------------------------------------
	// Configuracion del pin para el servo1
	handlerPin1PwmChannel.pGPIOx 								= GPIOC;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinNumber 		= PIN_7;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	GPIO_Config(&handlerPin1PwmChannel);

	// Configuracion del timer para el servo1
	handlerSignal1PWM.ptrTIMx 									= TIM3;
	handlerSignal1PWM.config.channel							= PWM_CHANNEL_2;
	handlerSignal1PWM.config.duttyCicle							= duttyValue1;
	handlerSignal1PWM.config.periodo							= 20000; // 20ms 5OHz
	handlerSignal1PWM.config.prescaler					    	= 16;
	// Cargamos la configuración del timer
	pwm_Config(&handlerSignal1PWM);

	//Activamos la senal
	enableOutput(&handlerSignal1PWM);
	startPwmSignal(&handlerSignal1PWM);

	/*// SERVO 2 ----------------------------------------------------------------------------------------------
	// Configuracion del pin para el servo2
	handlerPin2PwmChannel.pGPIOx 								= GPIOA;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinNumber 		= PIN_8;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	GPIO_Config(&handlerPin2PwmChannel);

	// Configuracion del timer para el servo2
	handlerSignal1PWM.ptrTIMx 									= TIM3;
	handlerSignal1PWM.config.channel							= PWM_CHANNEL_1;
	handlerSignal1PWM.config.duttyCicle							= duttyValue2;
	handlerSignal1PWM.config.periodo							= 20000; // 20ms 5OHz
	handlerSignal1PWM.config.prescaler					    	= 16;
	// Cargamos la configuración del timer
	pwm_Config(&handlerSignal2PWM);

	//Activamos la senal
	enableOutput(&handlerSignal2PWM);
	startPwmSignal(&handlerSignal2PWM);
	*/

}


int16_t convertToDutty1 (double angle)
{
	return 1600-(840*angle/90);
}

//Callback del Timer2 - Hacemos un blinky
void BasicTimer2_Callback(void)
{
	GPIOxTooglePin(&handlerBlinkyPin);
	GPIOxTooglePin(&handlerBlinkyBoardPin);
	sendMsg++;
}

//Callback del userButton --> hacer algo
void callback_extInt13(void)
{
		__NOP();
}

// Esta funcion se ejecuta cada vez que un caractera es recibido
// por el puerto USART2
void usart2Rx_Callback (void)
{
	usart2DataReceived =getRxData();
}


