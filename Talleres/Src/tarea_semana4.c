/*
 * tarea_semana4.c
 *
 *  Created on: Mar 7, 2023
 *      Author: ingfisica
 */


#include <stdint.h>



int main(void)
{
	uint16_t faceSet1 = 0b0000011011001110 ;
	uint16_t faceSet2 = 0b1101100001000111 ;

	// Solucion para los hombres
	uint16_t faceSet1A = ~faceSet1;

	// Solucion para las mujeres con lente

	// définition directe
	//uint16_t mascaraB = 0b0000010000000000 ;

	// définition bit a bit
	uint16_t mascaraB = 0b0;
	mascaraB ++;
	mascaraB = mascaraB << 10;


	uint16_t faceSet1B = FaceSet1 && mascaraB ;




    /* Loop forever */
	while(1){
	return 0;
	}
}
