/*
 * I2CDriver.c
 *
 *  Created on: 22/05/2023
 *  Author: malgeri
 */

#include "I2CDriver.h"

/*
 * Recordar que se debe configurar los pines para el I2C (SDA y SCL)
 * para lo cual se necesita el modulo GPIO y los pines configurados
 * en el modo Alternate Function
 *
 * Ademas estos pines deben ser configurados como salidas open-drain
 * y con la resistencias en modo pull-up
 */

void i2c_config(I2C_Handler_t *ptrHandlerI2C)
{
	// 1. Activamos la senal de reloj para el modulo I2C seleccionado
	if(ptrHandlerI2C->ptrI2Cx == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}

	else if (ptrHandlerI2C->ptrI2Cx == I2C2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}

	else if (ptrHandlerI2C->ptrI2Cx == I2C3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	else
	{
		__NOP();
	}

	// 2. Reiniciamos el periferico, de forma que inicia en un estado conocido
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_SWRST;

	// 3. Indicamos cual es la velocidad del reloh principal, que es la senal utilizada
	// por el periferico para generar la senal de reloj para el bus I2C
	ptrHandlerI2C->ptrI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos);
	ptrHandlerI2C->ptrI2Cx->CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C << I2C_CR2_FREQ_Pos);


	// 4. Configuramos el modo I2C en el que el sistemo funciona
	// En esta configuracion se incluye tambien la velociadda del reloj
	// y el tiempo maximo para el cambion de la senal (T-Rise)
	// Todo comienza con los dos registros en 0
	ptrHandlerI2C->ptrI2Cx->CCR=0;
	ptrHandlerI2C->ptrI2Cx->TRISE=0;

	if (ptrHandlerI2C->modeI2C == I2C_MODE_SM)
	{
		//Estamos en modo "standar" SM Mode
		//Seleccionamos el mode estandar
		ptrHandlerI2C->ptrI2Cx->CCR &= ~I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la senal del reloj
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-Rise maximo
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISA_TIME_SM;
	}

	else
	{
		//Estamos en modo "fast" FM Mode
		//Seleccionamos el modo Fast
		ptrHandlerI2C->ptrI2Cx->CCR |= I2C_CCR_FS;

		//Configuramos el registro que se encarga de generar la senal del reloj
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-Rise maximo
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISA_TIME_FM;
	}

	// 5. Activamos el modulo I2C
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_PE;

}


// 8. Generamos la condicion de stop
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C)
{
	// 7. Generamos la condicion de stop
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;
}

// 1. Verificamos que la linea no esta ocupada - bit busy en I2C_CR2
// 2. Generamos la senal "start"
// 2.a Esperamos a que la bandera del evento "start se levante
// Mientras esperamos, el valor de SB es 0, entonces la negacion es 1

void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C)
{
	// 1. Verificamos que la linea no esta ocupada - bit busy en I2C_CR2
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY)
	{
		__NOP();
	}

	// 2. Generamos la senal "start"
	ptrHandlerI2C->ptrI2Cx->CR1|= I2C_CR1_START;

	// 2.a Esperamos a que la bandera del evento "start" se levante
	// Mientras esperamos, el valor de SB es 0, entonces la negacion es 1
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB))
	{
		__NOP();
	}
}

/**/
void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C)
{
	// 2. Generamos la senal "start"
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	// 2.a Esperamos a que la bandera del evento "start" se levante
	// Mientras esperamos, el valor de SB es 0; entonces la negacion es 21
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB))
	{
		__NOP();
	}
}

// 7.a. Activamos la indicacion para no-ACK (indicacion para el slave de terminar)
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C)
{
	//Debemos escribir cero en la posicion ACK del registro de control 1
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

// 7.b. Activamos la indicacion para no-ACK (indicacion para el slave de terminar)
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C)
{
	//Debemos escribir cero en la posicion ACK de registro de control 1
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_ACK;
}

/**/
void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite)
{
	// 0. Definimos una variable auxiliar
	uint8_t auxByte = 0 ;
	(void) auxByte;

	// 3. Enviamos la direccion del Slave y el bit que indicamos que deseamos escribir (0)
	// en el siguiente paso se envia la direccion de memoria que se desea escribir

	ptrHandlerI2C->ptrI2Cx->DR = (slaveAddress << 1) | readOrWrite;

	// 3.1 Esperamos hasta que la bandera del evento "addr" se levante
	// esto nos indica que la direccio fue enviada satisfactoriamente
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR))
	{
		__NOP();
	}

	// 3.2 Debemos limpiar la bandeja de la recepcion ACK de la "addr" para lo cual
	// debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2
	auxByte = ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte = ptrHandlerI2C->ptrI2Cx->SR2;
}


/**/
void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr)
{
	// 4. Enviamos la direccion de memoria que deseamos leer
	ptrHandlerI2C->ptrI2Cx->DR = memAddr;

	//4.1. Esperamos hasta que el byte sea transmitido
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE))
	{
		__NOP();
	}
}


/**/
void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite)
{
	// 5. Cargamos el valor que deseamos escribir
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;

	//6. Esperamos hasta que el byte sea transmitido
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF))
	{
		__NOP();
	}
}

/**/
uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C)
{
	// 9. Esperamos hasta que el byte entrante sea recibido
	while (!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE))
	{
		__NOP();
	}

	ptrHandlerI2C->dataI2C=ptrHandlerI2C->ptrI2Cx->DR;
	return ptrHandlerI2C->dataI2C;
}

/**/
uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead)
{
	// O. Creamos una variable auxiliare para recibir el dato que leemos
	uint8_t auxRead = 0 ;

	// 1. Genreamos la condicion start
	i2c_startTransaction(ptrHandlerI2C);

	//2. Enviamos la direccion del esclavo y la indicacion de ESCRIBIR
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);


	// 3. Enviamos la direccion de memoria que deseamos leer
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	// 4. Creamos una condicion de re Start
	i2c_reStartTransaction(ptrHandlerI2C);

	// 5. Enviamos la direccion del esclavo y la indicacion de LEER
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	// 6. Leemos el dato que envia el esclavo
	auxRead = i2c_readDataByte(ptrHandlerI2C);

	// 7. Generamos la condicion de NoAck, para que el msater no responda y el slave solo envie 1 byte
	i2c_sendNoAck(ptrHandlerI2C);

	// 8. Generamos la condicion stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

	return auxRead;
}

/**/
void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue)
{

	// 1. Generamos la condicion start
	i2c_startTransaction(ptrHandlerI2C);


	// 2.Enviamos la direccion del esclavo y la indicacion de escribir
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	// 3. Enviamos la direccion de memoria que deseamos escribir
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	// 4. Enviamos el valor que deseamos escribir en el registro seleccionado
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	// 5. Generamos la condicion Stop, para que el slave se detenga despues de 1 byte
	i2c_stopTransaction(ptrHandlerI2C);

}
