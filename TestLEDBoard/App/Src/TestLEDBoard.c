/*
 * @file           : TestLEDBoard_Main.c
 * @author         : mmalgeri
 * @brief          : Programa que encende el LED2
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

// Definicion de los handlers necesarios
GPIO_Handler_t handlerPinLED        = {0};
BasicTimer_Handler_t handlerLEDTimer = {0};
GPIO_Handler_t handlerPinLED2        = {0};

//Definicion de las cabeceras de las funciones del main
void initSystem(void);


void initSystem(void)
{
	//Configuracion el pin para el LED2
	handlerPinLED2.pGPIOx                             = GPIOA;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinNumber      = PIN_5;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerPinLED2);

	//Configuracion el pin para el LED
	handlerPinLED.pGPIOx                             = GPIOC;
	handlerPinLED.GPIO_PinConfig.GPIO_PinNumber      = PIN_8;
	handlerPinLED.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerPinLED.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerPinLED.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinLED.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerPinLED);

	//Configurando el Timer2 para el LED
	handlerLEDTimer.ptrTIMx 								= TIM2;
	handlerLEDTimer.TIMx_Config.TIMx_mode				    = BTIMER_MODE_UP;
	handlerLEDTimer.TIMx_Config.TIMx_speed				    = BTIMER_SPEED_100us;
	handlerLEDTimer.TIMx_Config.TIMx_period				    = 2500;
	handlerLEDTimer.TIMx_Config.TIMx_interruptEnable        = BTIMER_INTERRUPT_ENABLE;
	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerLEDTimer);
}


int main (void)
{
	    initSystem();

	    // Activamos el pinA5
	    GPIO_WritePin (&handlerPinLED2,SET);

		// Activamos el pinC8
		GPIO_WritePin (&handlerPinLED,SET);

		GPIO_WritePin (&handlerPinLED2,RESET);
		GPIO_WritePin (&handlerPinLED,RESET);

	    while(1)
	    {
	    	__NOP();
	    }

	    return 0;
}



