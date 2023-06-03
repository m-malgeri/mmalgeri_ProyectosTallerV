/*
 * USARTxDriver.c
 *
 *  Created on: 20/05/2023
 *  Author: malgeri
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"



uint8_t data;
uint8_t auxRxData;



/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler)
{
	/* 0. Desactivamos las interrupciones globales */
		__disable_irq();

	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1)
	{
		// Escriba acá su código
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}

    /* 1.2 Configuramos el USART2 */
    // Escriba acá su código
	else if (ptrUsartHandler->ptrUSARTx == USART2)
	{
		// Escriba acá su código
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	}

	/* 1.3 Configuramos el USART6 */
	else if (ptrUsartHandler->ptrUSARTx == USART6)
	{
		// Escriba acá su código
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
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
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; // Enable parity
			ptrUsartHandler->ptrUSARTx->CR1 |= ~USART_CR1_PS;  // Choosing even parity

			// Adaptamos el tamano de los datos
			ptrUsartHandler -> USART_Config.USART_datasize = USART_DATASIZE_9BIT;
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_M);

		}

		else
		{
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			// Escriba acá su código
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; // Enable parity
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS; // Choosing odd parity
			// Adaptamos el tamano de los datos
			ptrUsartHandler -> USART_Config.USART_datasize = USART_DATASIZE_9BIT;
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_M);
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
	if (ptrUsartHandler -> USART_Config.USART_datasize == USART_DATASIZE_8BIT)
	{
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~ (USART_CR1_M);
	}
	else //9 bits
	{
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_M);
	}

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits)
	{

	case USART_STOPBIT_1:
	{
		// Debemos cargar el valor 0b00 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);
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
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RE);
		break;
	}

	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		// Escriba acá su código
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RE);
		break;
	}
	}

	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE)
	{
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE ;
	}

// 3. Activamos la interrupción para el USART

	switch(ptrUsartHandler -> USART_Config.USART_interrupt)

	{
	case USART_INTERRUPT_RX_ENABLE:
	{
		// Permitimos que se levante la bandera RX
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_RXNEIE);
		// NO permitimos que se levante la bandera TX
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
		break;
		}
	case USART_INTERRUPT_TX_ENABLE:
		{
		// NO permitimos que se levante la bandera TX
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
		// NO Permitimos que se levante la bandera RX
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RXNEIE);
		break;

		}
	case USART_INTERRUPT_RXTX_ENABLE:
		{
		// Activamos las dos interrupciones tanto del RX como del TX
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TXEIE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
		break;
		}
	case USART_INTERRUPT_RXTX_DISABLE:
		{
		// Activamos las dos interrupciones tanto del RX como del TX
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
		break;
		}

	default:
		{
		// NO Permitimos que se levante la bandera RX
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RXNEIE);
		// NO permitimos que se levante la bandera TX
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
		break;
		}
	}

// 4. Activamos la señal de la interrupción en el NVIC

if((ptrUsartHandler -> USART_Config.USART_interrupt == USART_INTERRUPT_RX_ENABLE) | (ptrUsartHandler -> USART_Config.USART_interrupt == USART_INTERRUPT_TX_ENABLE))
{
	if(ptrUsartHandler -> ptrUSARTx == USART1)
	{
			// Activamos la IRQ del USART1
			__NVIC_EnableIRQ(USART1_IRQn);
	}
	else if(ptrUsartHandler -> ptrUSARTx == USART2)
	{
		// Activamos la IRQ del USART2
		__NVIC_EnableIRQ(USART2_IRQn);
	}
	else if(ptrUsartHandler -> ptrUSARTx == USART6)
	{
		// Activamos la IRQ del USART6
		__NVIC_EnableIRQ(USART6_IRQn);
	}
}

// 5. Activamos de nuevo todas las interrupciones

		__enable_irq();
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

/* funcion para leer un solo char */
char readChar(USART_Handler_t *ptrUsartHandler){

	//Se lee el valor recivido el data register
	char dataToRead;
	dataToRead = ((char)(ptrUsartHandler -> ptrUSARTx -> DR));
	return dataToRead;

	}


uint8_t returnData(void){
	return data;				// Devolvemos el valor
}

/* funcion para escribir un mensaje */
void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend){

	while(*msgToSend != '\0')
	{
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}
}

// Callbacks

__attribute__((weak)) void usart1Rx_Callback(void){
//esta funcion no puede ser modificada,cuando se necesite el callback debe
//implemtarse el BasicTimer_CallBack en el archivo main
	__NOP();
}

__attribute__((weak)) void usart2Rx_Callback(void){
//esta funcion no puede ser modificada,cuando se necesite el callback debe
//implemtarse el BasicTimer_CallBack en el archivo main
	__NOP();
}

__attribute__((weak)) void usart6Rx_Callback(void){
//esta funcion no puede ser modificada,cuando se necesite el callback debe
//implemtarse el BasicTimer_CallBack en el archivo main
	__NOP();
}


//Lectura del caracter que llega
uint8_t getRxData(void){
	return auxRxData;
}

void USART2_IRQHandler(void){
	//evaluamos is la interrupcion se dio por rx
	if (USART2-> SR & USART_SR_RXNE){
		auxRxData = (uint8_t)USART2 -> DR;
		usart2Rx_Callback();
	}
}

void USART1_IRQHandler(void){
	//evaluamos is la interrupcion se dio por rx
	if (USART1-> SR & USART_SR_RXNE){
		auxRxData = (uint8_t)USART1 -> DR;
		usart1Rx_Callback();
	}
}

void USART6_IRQHandler(void){
	//evaluamos is la interrupcion se dio por rx
	if (USART6-> SR & USART_SR_RXNE){
		auxRxData = (uint8_t)USART6 -> DR;
		usart6Rx_Callback();
	}
}




