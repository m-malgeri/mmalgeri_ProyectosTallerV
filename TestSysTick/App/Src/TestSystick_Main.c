/*
 * @file           : TestPWM_Main.c
 * @author         : mmalgeri
 * @brief          : Pruebas a un PWM simple
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <stm32f4xx.h>

#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"

#define HSI_CLOCK_CONFIGURED     0   //16MHz
#define HSE_CLOCK_CONFIGURED     1
#define PLL_CLOCK_CONFIGURED     2

// Definicion de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin        = {0};

GPIO_Handler_t handlerUserButton       = {0};
EXTI_Config_t handlerUserButtonExti    = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};


//Elementos para hacer la comunicacion serial
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint8_t sendMsg = 0;

//Definicion de las cabeceras de las funciones del main
void initSystem(void);

int main (void)
{
	//Inicialisamos todos los elementos del sistema
	initSystem();

	//Se configura el SysTick a 16MHz
	config_SysTick_ms(HSI_CLOCK_CONFIGURED);

	//writeMsg(&usart2Comm, "Hola Mundo!!");
	//Loop forever
	while (1)
	{
		if(sendMsg>4)
		{
			writeMsg(&usart2Comm, "Hola Mundo !!");

			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);

			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);

			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(185);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(185);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(185);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(185);

			sendMsg=0;

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
	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerUserButton);

	handlerUserButtonExti.pGPIOHandler		=&handlerUserButton; //Puntero a la esctructura
	handlerUserButtonExti.edgeType          =EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);

	// Configuracion de la comunicacion serial*/
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
	USART_Config(&usart2Comm);
}





//Callback del Timer2 - Hacemos un blinky
void BasicTimer2_Callback(void)
{
	sendMsg++;
}

//Callback del userButton --> hacer algo
void callback_extInt13(void)
{
		__NOP();
}


