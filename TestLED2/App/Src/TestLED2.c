/*
 * @file           : TestLED2_Main.c
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
GPIO_Handler_t handlerBlinkyPin        = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
//Definicion de las cabeceras de las funciones del main
void initSystem(void);


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

}


//Callback del Timer2 - Hacemos un blinky
void BasicTimer2_Callback(void)
{
	GPIOxTooglePin(&handlerBlinkyPin);
}


int main (void)
{
	    initSystem();

	    // Activamos el pinA5 //Eso es para que quede encendido
	    //GPIO_WritePin (&handlerPinLED2,SET);

	    while(1)
	    {
	    	__NOP();
	    }

	    GPIO_WritePin (&handlerBlinkyPin,RESET);

	    return 0;
}



