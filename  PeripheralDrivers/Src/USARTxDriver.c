/*
 * USARTxDriver.c
 *
 *  Created on: Apr 6, 2022
 *      Author: malgeri
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1)
	{
		// Escriba acá su código
		RCC->APB1ENR &= ~RCC_APB2ENR_USART1EN;
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}

    /* 1.2 Configuramos el USART2 */
    // Escriba acá su código
	else if (ptrUsartHandler->ptrUSARTx == USART2)
	{
		// Escriba acá su código
		RCC->APB2ENR &= ~RCC_APB1ENR_USART2EN;
		RCC->APB2ENR |= RCC_APB1ENR_USART2EN;
	}

	/* 1.3 Configuramos el USART6 */
	else if (ptrUsartHandler->ptrUSARTx == USART6)
	{
		// Escriba acá su código
		RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	}

	else
	{
		__NOP();
	}


	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		// Verificamos si se ha seleccionado ODD or EVEN

		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada
			// Escriba acá su código
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; //On dit qu'on veut un bit de parité
			ptrUsartHandler->ptrUSARTx->CR1 |= ~USART_CR1_PS; // Il faut mettre 0
			// Adapter la taille de datos


		}

		else
		{
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			// Escriba acá su código
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; //On dit qu'on veut un bit de parité
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS; // Il faut mettre 1
			// Adapter la taille de datos
		}

	}

	else
	{
		// Si llegamos aca, es porque no deseamos tener el parity-check
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PCE;
	}

	// 2.3 Configuramos el tamaño del dato
    // Escriba acá su código
	ptrUsartHandler->ptrUSARTx->CR1 &= ~ (USART_CR1_M);

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits)
	{

	case USART_STOPBIT_1:
	{
		// Debemos cargar el valor 0b00 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP); // Il faut mettre 00

		break;
	}

	case USART_STOPBIT_0_5:
	{
		// Debemoscargar el valor 0b01 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP); //Limpiamos
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_0;
		break;
	}

	case USART_STOPBIT_2:
	{
		// Debemoscargar el valor 0b10 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP); //Limpiamos
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_1;
		break;
	}

	case USART_STOPBIT_1_5:
	{
		// Debemoscargar el valor 0b11 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP); //Limpiamos
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP;
		break;
	}

	default:
	{
		// En el casopor defecto seleccionamos 1 bit de parada
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);
		break;
	}

	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;
	if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
		// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
		// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
		// Valor a cargar 0x0683
		// Configurando el Baudrate generator para una velocidad de 9600bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
	}

	else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
		// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
		// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
		// Valor a cargar Ox0341
		// Escriba acá su código y los comentarios que faltan
		ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
	}

	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
		// Escriba acá su código y los comentarios que faltan
		// El valor a cargar es 8.68056 -> Mantiza = 8,fraction = 0.68056
		// Mantiza = 8 = 0x8 , fraction = 16 * 0.68056 = 11 = OxB
		// Valor a cargar 0x0B
		ptrUsartHandler->ptrUSARTx->BRR = 0x0B;

	}

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TE | USART_CR1_RE);
		break;
	}

	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TE | USART_CR1_RE);
		break;
	}
	}

	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE)
	{
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE ;
	}
}

/* funcion para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

	// Escriba acá su código
	ptrUsartHandler->ptrUSARTx->DR = dataToSend;

	return dataToSend;
}
