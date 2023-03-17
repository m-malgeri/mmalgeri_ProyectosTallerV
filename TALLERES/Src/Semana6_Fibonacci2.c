/**
 ******************************************************************************
 * @file           : Semana.c
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

#include <stdint.h>

#define SUMA '+'
#define RESTA '-'
#define MULTIPLICACION '*'
#define DIVISION '/'
#define FACTORIAL '!'
#define FIBONACCI 'F'


//Funcion calcular operacion
uint16_t Operaciones (uint8_t TipoOperacion, uint16_t numero1, uint16_t numero2);
uint16_t Operaciones2 (uint8_t TipoOperacion, uint16_t numero1);



uint16_t Operaciones (uint8_t TipoOperacion, uint16_t numero1, uint16_t numero2)
{

switch(TipoOperacion)
{

case SUMA :
{
	return numero1+numero2;
	break;
}

case RESTA :
{
	return numero1-numero2;
	break;
}

case MULTIPLICACION :
{
	return numero1*numero2;
	break;
}

case DIVISION :
{
	return numero1/numero2;
	break;
}

default :
{
	return 0;
	break;
}

}
}


uint16_t Operaciones2 (uint8_t TipoOperacion, uint16_t numero1)
{

switch(TipoOperacion)
{

	case FACTORIAL :
	{
		uint32_t factorial=1;
		uint16_t contador=1;
		while (contador <= numero1)
		{
			factorial = factorial * contador;
			contador ++;
		}

		return factorial;
		break;
	}

	case FIBONACCI :
	{
		uint32_t n_menos1=0;
		uint32_t n_menos2=0;
		uint32_t n = 1;
		uint16_t compteur=1;

		while (compteur < numero1)
		{
			n = n + n_menos1;
			n_menos2=n_menos1;
			n_menos1 = n;
			compteur++;
		}
	return n;
	break;
	}

}
}

int main(void)
{
	uint16_t resultado= Operaciones(SUMA,10,20);
	resultado= Operaciones(RESTA,10,20);
	resultado= Operaciones('*',10,20);

	resultado=Operaciones2(FIBONACCI, 5);
	resultado=Operaciones2(FACTORIAL, 5);

	while (1)
	{
	}
}







