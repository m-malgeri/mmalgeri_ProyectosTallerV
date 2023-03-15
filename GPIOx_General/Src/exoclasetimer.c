/**
 ******************************************************************************
 * @file           : IntroGPIOxMain.c
 * @author         : mmalgeri
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/*
 * Con esta programa se desea mostrar el uso basico de los registros que controlan
 * al micro (SFR) y la forma adecuada para utilizar los operadores &, |, ~y =, para
 * cambiar la configuracion de algun registro
 * Tambien es importante para entender la utilidad de los numeros BIN y HEX
 *
 * Es necesario tener a la mano tanto el manual de referencia del micro cpmo la hoja de
 * datos del micro (ambos son documentos diferentes y complementarios el uno del otro)
 *
 * HAL --> Hardware Abstraction Layer
 *
 * Este programa introduce el periferico mas simple que tiene el micro, que es el
 * encargado de manejar los pines de cada pines de cada puerto del micro
 *
 * Cada PINx de cada puerto GPIO puede desarollar funciones basicas de tipo entrada
 * y salida de datos digitales (o sea 1's y 0's), ademas se les puede asignar funciones
 * especiales que generalmente estan ligadas a otro periferico adicional (se vera mas
 * adelante)
 *
 * De igual forma, varias caracteristicas pueden ser confoguradas para cade PINX como :
 * - la velocidad a la que puede responder
 * - el tipo de entrada (pull up, pull down, open drain)
 * - el tipo de salida (push-pull, open drain...)
 *
 *  Debemos definir entonces todos los registros que manejan el periferico GPIOx y luego
 *  crear algunas funciones para utilizar adecuadamente el equipo
 *
 */


#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

// Funcion principal del programa
// Es aca donde se ejecuta todo

int main(void)
{
	// Definimos el handler para el PIN que deseamos configurar
	GPIO_Handler_t handlerOnBoardLed = {0};

    // Deseamos trabajar con el puerto GPIOA
	handlerOnBoardLed.pGPIOx = GPIOA;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinNumber        =PIN_5;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinMode          =GPIO_MODE_OUT;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinOPType        =GPIO_OTYPE_PUSHPULL;
	//handlerOnBoardLed.GPIO_PinConfig.GPIO_PinPuPdControl   =GPIO_PUPDR_NOTHING;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinSpeed         =GPIO_OSPEED_FAST;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinAltFunMode    =AF0;  //Ninguna funcion

	// Cargamos la configuracion del PIN especifico
	GPIO_Config(&handlerOnBoardLed);

	// Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerOnBoardLed, SET);

	//Configuracion TIMER
	RCC->APB1ENR|=(1<<0);  // On met un 1 à la position O car c'est à cette position
			               // mettre un 1 pour activer le timer2
	//RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //fais la même chose

	//Il manque la librairie mcu_headers pour utiliser tout ça
	TIM2->CR1 &= ~(TIM_CR1_DIR);
	TIM2->PSC =1600;
	TIM2->CNT=0;
	TIM2->ARR =250;
	TIM2-> CR1 |= TIM_CR1_CEN;




	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1)
	{
		if (TIM2->SR & TIM_SR_UIF)
			{
				GPIO_TooglePin(&handlerOnBoardLed);
				TIM2->SR &= ~TIM_SR_UIF;
			}
	}

}
