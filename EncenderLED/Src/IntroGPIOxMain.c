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
	//Configuracion PC13
	//Configuracion initial del MCU
	//RCC->AHB1ENR &= ~(1<<0);    //Borrar la posicion cual sea del AHB1ENR
	//RCC->AHB1ENR |= 1<<0 ;      //Activar el ciclo del reloj en AHB1ENR



	// Definimos el handler para el PIN que deseamos configurar
	GPIO_Handler_t handlerUserLedPin = {0};

    // Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber        =PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode          =GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType        =GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl   =GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed         =GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode    =AF0;  //Ninguna funcion

	// Cargamos la configuracion del PIN especifico
	GPIO_Config(&handlerUserLedPin);

	// Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);

	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1)
	{
		NOP();
	}

}
