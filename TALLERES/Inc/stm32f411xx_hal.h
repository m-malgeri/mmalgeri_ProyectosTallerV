/*
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 16, 2023
 *      Author: ingfisica
 */

#ifndef STM32F411XX_HAL_H_
#define STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK SPEED     16000000
#define HSE_CLOCK SPEED     4000000

#define NOP()  asm("NOP")
#define __weak  __attribute__((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * Remember, 1Kbytes = 1024 bytes
 */

#define FLASH_BASE_ADDR    0x08000000U  // esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR     0x20000000U  // esta es la memoria RAM, 128KB


/* Nota
 * Ahora agregamos la direccion de memoria base para cada uno de los perifericos que posee el micro
 * En el datasheet del micro, figura 14 (memory map), encontramos el mapa de los buses :
 * APB1 (Advance Peripherical Bus)
 * APB2
 * AHBA (Advance High-performance Bus)
 * AHB2
 */

/*
 * AHBx and APBx Bus peripherals base addresses
 */
#define APB1_BASE_ADDR   0x40000000U
#define APB2_BASE_ADDR   0x40010000U
#define AHB1_BASE_ADDR   0x40020000U
#define AHB2_BASE_ADDR   0x50000000U

#define USB_OTG_FS_BASE_ADDR   (AHB2_BASE_ADDR + 0x0000U)

/*
 * Y ahora debemos hacer lo mismo pero cada una de las posiciones de memoria de cada unode los
 * perofericos descritos en la Tabla 1 del manuel de referencia del micro.
 * Observe que en dicha tabla esta a su vez dividida en cuatro segmentos, cada uno correspondiente
 * a APB1 APB2 AHB1 AHB2
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla
 * Inicia USB_OTG_FS (AHB2)
 *
 * NOTA
 * Solo lo vamos a hacer con los elementos que necesitamos en este ejercicio : RCC y GBIOx
 */

/* Posiccciones de memoria para perifericos del AHB1
 * Observa que no esta completa */
#define RCC_BASE_ADDR      (AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR    (AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR    (AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR    (AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR    (AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR    (AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR    (AHB1_BASE_ADDR + 0x0000U)

/* Macros genericos */
#define ENABLE           1
#define DISABLE          0
#define SET              ENABLE
#define CLEAR            DISABLE
#define RESET            DISABLE
#define GPIO_PIN_SET     SET
#define GPIO_PIN_RESET   RESET
#define FLAG_SET         SET
#define FLAG_RESET       RESET
#define I2C_WRITE        0
#define I2C_READ         1


/* Inicio de la descripcion de los elementos que componen el periferico*/

/* Definicion de la estructura de datos que representa a cada uno de los registros que componen
 * el periferico RCC
 *
 * Debido a los temas que se van a manejar en el curso, solo se deben definir los
 * bits de los registros 6.3.1 (RCC_CR) hasta el 6.3.12 (RCC_APB2ENR), 6.3.17 (RCC_BDCR)
 * y 6.3.1 (RCC_CR)
 *
 * NOTA : La posicion de memoria (offset) debe encajar perfectamentte con la posicion de memoria indicada
 * en la hoja de datos del equipo. Observe que los elementos reservedx tambien estan presentes alli
* */
typedef struct
{
	volatile uint32_t CR;          // Clock Control Register             ADDR_OFFSET : 0x00
	volatile uint32_t PLLCFGR;     // PLL Configuration Register         ADDR_OFFSET : 0x04
	volatile uint32_t CFGR;        // Clock Configuration Register       ADDR_OFFSET : 0x08
	volatile uint32_t CIR;         // Clock Interrupt Register           ADDR_OFFSET : 0x0C
	volatile uint32_t AHB1RSTR;    // AHB1 Peripheral Reset Register     ADDR_OFFSET : 0x10
	volatile uint32_t AHB2RSTR;    // AHB2 Peripheral Reset Register     ADDR_OFFSET : 0x14
	volatile uint32_t reserved0;   // Reserved                           ADDR_OFFSET : 0x18
	volatile uint32_t reserved1;   // Reserved                           ADDR_OFFSET : 0x1C
	volatile uint32_t APB1RSTR;    // APB1 Peripheral Reset Register     ADDR_OFFSET : 0x20
	volatile uint32_t APB2RSTR;    // APB2 Peripheral Reset Register     ADDR_OFFSET : 0x24
	volatile uint32_t reserved2;   // Reserved                           ADDR_OFFSET : 0x28
	volatile uint32_t reserved3;   // Reserved                           ADDR_OFFSET : 0x2C
	volatile uint32_t AHB1ENR;     // AHB1 Peripheral Clock Enable Register     ADDR_OFFSET : 0x30
	volatile uint32_t AHB2ENR;     // AHB2 Peripheral Clock Enable Register     ADDR_OFFSET : 0x34
	volatile uint32_t reserved4;   // Reserved                                  ADDR_OFFSET : 0x38
	volatile uint32_t reserved5;   // Reserved                                  ADDR_OFFSET : 0x3C
	volatile uint32_t APB1ENR;     // APB1 Peripheral Clock Enable Register     ADDR_OFFSET : 0x40
	volatile uint32_t APB2ENR;     // APB2 Peripheral Clock Enable Register     ADDR_OFFSET : 0x44
	volatile uint32_t reserved6;   // Reserved                                  ADDR_OFFSET : 0x48
	volatile uint32_t reserved7;   // Reserved                                  ADDR_OFFSET : 0x4C
	volatile uint32_t AHB1LPENR;   // AHB1 Clock Enable Low Power Register      ADDR_OFFSET : 0x50
	volatile uint32_t AHB2LPENR;   // AHB2 Clock Enable Low Power Register      ADDR_OFFSET : 0x54
	volatile uint32_t reserved8;   // Reserved                                  ADDR_OFFSET : 0x58
	volatile uint32_t reserved9;   // Reserved                                  ADDR_OFFSET : 0x5
	volatile uint32_t APB1LPENR;    // APB1 Clock Enable Low Power Register      ADDR_OFFSET : 0x60
	volatile uint32_t APB2LPENR;    // APB2 Clock Enable Low Power Register      ADDR_OFFSET : 0x64
	volatile uint32_t reserved10;   // Reserved                                  ADDR_OFFSET : 0x68
	volatile uint32_t reserved11;   // Reserved                                  ADDR_OFFSET : 0x6C
	volatile uint32_t BDCR;         // Backup Domain Control Register            ADDR_OFFSET : 0x70
	volatile uint32_t CSR;          // Clock Control & Status Register           ADDR_OFFSET : 0x74
	volatile uint32_t reserved12;   // Reserved                                  ADDR_OFFSET : 0x78
	volatile uint32_t reserved13;   // Reserved                                  ADDR_OFFSET : 0x7C
	volatile uint32_t SSCGR;        // Spread Spectrum Clock Generation Reg      ADDR_OFFSET : 0x80
	volatile uint32_t PLLI2SCFGR;   // PLLI2S Configuration Register             ADDR_OFFSET : 0x84
	volatile uint32_t reserved14;   // Reserved                                  ADDR_OFFSET : 0x88
	volatile uint32_t DCKCFGR;      // Dedicated Clocks Configuration Reg        ADDR_OFFSET : 0x8C
} RCC_RegDef_t;

#define RCC    ((RCC_RegDef_t *) RCC_BASE_ADDR)

/* 6.3.9 RCC_AHB1ENR*/
#define RCC_AHB1ENR_GPIOA_EN         0
#define RCC_AHB1ENR_GPIOB_EN         1
#define RCC_AHB1ENR_GPIOC_EN         2
#define RCC_AHB1ENR_GPIOD_EN         3
#define RCC_AHB1ENR_GPIOE_EN         4
#define RCC_AHB1ENR_GPIOH_EN         7
#define RCC_AHB1ENR_CRCEN            12
#define RCC_AHB1ENR_DMA1_EN          21
#define RCC_AHB1ENR_DMA2_EN          22



/* Inicio de description de los elementos que componen el periferico GPIOx */

/* defincion de la estructura de datos que representa a cada uno de los registros que componen el
 * periferico GPIOx
 * Debido a que el periferico GPIOX es muy simple, no es muy necesario crear en la descripcion bit a bit
 * de cada uno de los registros que componen el dicho periferico, pero si es necesario comprendre que
 * hace cada registro, para poder cargar correctamente la configuracion
 */
typedef struct
{
	volatile uint32_t MODER;          // Port Mode Register            ADDR_OFFSET : 0x00
	volatile uint32_t OTYPER;          // Port Output Type Register            ADDR_OFFSET : 0x04
	volatile uint32_t OSPEEDR;          // Port Output Speed Register            ADDR_OFFSET : 0x08
	volatile uint32_t PUPDR;          // Port Pull-up/Pull-down Register         ADDR_OFFSET : 0x0C
	volatile uint32_t IDR;          // Port Input Data Register           ADDR_OFFSET : 0x10
	volatile uint32_t ODR;          // Port Output Data Register           ADDR_OFFSET : 0x14
	volatile uint32_t BSRR;          // Port Bit Set/Reset Register           ADDR_OFFSET : 0x18
	volatile uint32_t LCKR;          // Port Configuration Lock Register           ADDR_OFFSET : 0x1C
	volatile uint32_t AFRL;          // Alternate Function Low Register          ADDR_OFFSET : 0x20
	volatile uint32_t AFRH;          // Alternate Function High Register            ADDR_OFFSET : 0x24
} GPIOx_RegDef_t;

#define GPIOA      ((GPIOx_RegDef_t *) GPIOA_BASE_ADDR)
#define GPIOB      ((GPIOx_RegDef_t *) GPIOB_BASE_ADDR)
#define GPIOC      ((GPIOx_RegDef_t *) GPIOC_BASE_ADDR)
#define GPIOD      ((GPIOx_RegDef_t *) GPIOD_BASE_ADDR)
#define GPIOE      ((GPIOx_RegDef_t *) GPIOE_BASE_ADDR)
#define GPIOH      ((GPIOx_RegDef_t *) GPIOH_BASE_ADDR)

#define GPIO_MODE_IN      O
#define GPIO_MODE_OUT     1
#define GPIO_MODE_ALTFN   2
#define GPIO_MODE_ANALOG  3

/* 8.4.2 GPIOx_OTYPER (un bit por PIN) */
#define GPIO_OTYPE_PUSHPULL    0
#define GPIO_OTYPE_OPENDRAIN   1

/* 8.4.3 GPIOx_OSPEEDR (dos bits po cada PIN) */
#define GPIO_OSPEED_LOW        O
#define GPIO_OSPEED_MEDIUM     1
#define GPIO_OSPEED_FAST       2
#define GPIO_OSPEED_HIGH       3

/* 8.4.3 GPIOx_PUPDR (dos bits po cada PIN) */
#define GPIO_PUPDR_NOTHING     0
#define GPIO_PUPDR_PULLUP      1
#define GPIO_PUPDR_PULLDOWN    2
#define GPIO_PUPDR_RESERVED    0

/* Definicion de los nombres de los pines */
#define PIN_O        0
#define PIN_1        1
#define PIN_2        2
#define PIN_3        3
#define PIN_4        4
#define PIN_5        5
#define PIN_6        6
#define PIN_7        7
#define PIN_8        8
#define PIN_9        9
#define PIN_1O       10
#define PIN_11       11
#define PIN_12       12
#define PIN_13       13
#define PIN_14       14
#define PIN_15       15

/* Definiciones de las funciones alternativas */
#define AF0    0b0000
#define AF1    0b0001
#define AF2    0b0010
#define AF3    0b0011
#define AF4    0b0100
#define AF5    0b0101
#define AF6    0b0110
#define AF7    0b0111
#define AF8    0b1000
#define AF9    0b1001
#define AF10   0b1010
#define AF11   0b1011
#define AF12   0b1100
#define AF13   0b1101
#define AF14   0b1110
#define AF15   0b1111


#endif /* STM32F411XX_HAL_H_ */
