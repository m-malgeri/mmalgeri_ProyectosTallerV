/**
 ******************************************************************************
 * @file           : main.c
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
 *
 * PRIMER PUNTO
1. En el desarrollo mostrado en el video del Driver para el puerto GPIOx,
la función GPIO_ReadPin() tiene un error, el cuál no nos permite obtener
el valor real del PinX que estamos leyendo. Explique sus respuestas en un
comentario de bloque dentro del archivo main del proyecto.
¿Cúal es este error?
¿Cómo lo puede solucionar?
Corrija el código y pruebe que efectivamente su propuesta de solución funciona (verificar en modo debugger).

SOLUCION

Eso era el codigo de la video :

uint32_t GPIO_ReadPin (GPIO_Handler_t *pPinHandler)
{
	// Creamos una variable auxiliar la cual luego retornaremos
	uint32_t pinValue = 0;

	// Cargamos el valor del registro IDR, desplezado a derecha tantas veces como la ubicacion
	// del pin especifico
	pinValue = (pPinHandler -> pGPIOx -> IDR >> pPinHandler -> GPIO_PinConfig.GPIO_PinNumber);

	return pinValue;
}

--> Primero, vamos a explicar que hace esta funcion.
Creamos una variable de 32 bits que llamamos pinValue.
Asignamos a esta variable los 32bits del registro IDR del GPIO con el que trabajamos pero que hemos
movido hacia la derecha tanta veces como el numero del Pin que deseamos leer.
Al final en la variable pinValue, tenemos el valor del Pin que queremos leer en la posicion 1, pero todavia
quedan otros valores en las posiciones mas altas. Esto no es optimal para trabajar con el microcontrolador.

Para solucionar esto, debemos usar una mascara.

uint32_t GPIO_ReadPin (GPIO_Handler_t *pPinHandler)
{
	// Creamos una variable auxiliar la cual luego retornaremos
	uint32_t pinValue = 0;

	// Cargamos el valor del registro IDR, desplezado a derecha tantas veces como la ubicacion
	// del pin especifico
	pinValue = (pPinHandler -> pGPIOx -> IDR >> pPinHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Limpiamos pinValue para que solo nos queda el valor del PIN que deseamos leer en la
	//posicion 1
	pinValue &= (SET << 0);

	// Retornamos pinValue
	return pinValue;
}


* SEGUNDO PUNTO
Cree una nueva función llamada GPIOxTooglePin, la cual reciba como parámetro solamente un elemento del tipo
GPIO_Handler_t y su función sea cambiar el estado de un PinX (seleccionado en el handler y debidamente configurado).
Cambiar el estado significa: sí está encendido que pase a apagado, si está apagado que pase a encendido.
Hint: La operación XOR puede ser útil.
NOTA: Esta función debe ser creada dentro del archivo adecuado. Si no es ubicada correctamente, la calificación
de este punto será penalizada.

uint32_t GPIOxTooglePin (GPIO_Handler_t *pPinHandler)
{
	uint32_t mascara = ( SET << pPinHandler -> GPIO_PinConfig.GPIO_PinNumber)


*/




#include <stdint.h>
#include "stm32f411xx_hal.h"

#include "GPIOxDriver.h"

// Funcion principal del programa
// Es aca donde se ejecuta todo

int main(void)
{
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
