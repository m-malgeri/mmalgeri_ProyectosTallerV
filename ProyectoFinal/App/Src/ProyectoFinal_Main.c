/*
 * @file           : ProyectoFinal_Main.c
 * @author         : mmalgeri
 * @brief          : Solar Tracker
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <stm32f4xx.h>

#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "PwmDriver.h"
#include "USARTxDriver.h"

#include "SPA.h"


//Definicion de los elementos para el calcul SPA ------------------------------------------------------------

spa_data spa;  //declare the SPA structure
int result;

// Definicion de los handlers necesarios --------------------------------------------------------------------


//Elementos para el Blinky

GPIO_Handler_t handlerPinLED2        = {0};
GPIO_Handler_t handlerBoardPin        = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

//UserButton
GPIO_Handler_t handlerUserButton       = {0};
EXTI_Config_t handlerUserButtonExti    = {0};


//Elementos para acer la comunicacion serial ---------------------------------------------------------------

GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};

//GPIO_Handler_t handlerPinTXGPS = {0}; //No necesitamos Transmitir al GPS
GPIO_Handler_t handlerPinRXGPS = {0};
USART_Handler_t usart6Comm = {0};

uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0 ;
uint8_t usart6DataReceived = 0 ;

// Elementos para sacar la informacion del GPS

int GuardarMensaje = 0;
int MensajeCorrecto=0;
int MensajeCompleto=0;

int i=0;
int n=0;

char CodigoMsg[6]={0};

int Zenit=0;
int Azimut=0;

char infoSPA[20]="hh,mm,ss,ll,d,LLL,D";
int hour=00;
int min=00;
int sec=00;
int Lat=00;
char dirLat ={0};
int Longi=00;
char dirLong={0};





//Elementos para el PWM -------------------------------------------------------------------------------------

//Servo1
GPIO_Handler_t handlerPin1PwmChannel  = {0};
PWM_Handler_t handlerSignal1PWM      = {0};
uint16_t duttyValue1=800;

//Servo2
GPIO_Handler_t handlerPin2PwmChannel  = {0};
PWM_Handler_t handlerSignal2PWM      = {0};
uint16_t duttyValue2=1600;

uint16_t newDutty=0;

char bufferGPS[128] = {0};
char bufferMsg[256] = {0};


//Definicion de las cabeceras de las funciones del main
void initSystem(void);
void printMsgGPS(void);
int16_t convertToDutty1 (double angle);
int16_t convertToDutty2 (double angle);
void movimientoServo();


int main (void)
{

	//Inicialisamos todos los elementos del sistema
	initSystem();


	//Loop forever
	while (1)
	{

		// Colocamos el mensaje enviado por el GPS
		printMsgGPS();

		if (usart2DataReceived != '\0')
			{
				if (usart2DataReceived == 'M')
				{
						// SACAMOS LA INFORMACION DEL GPS ---------------------------------------------------
						while (MensajeCorrecto!=1)
						{
							while (MensajeCompleto!=1)
							{

								if (usart6DataReceived != '\0')
								{
									if (usart6DataReceived == '$')
									{
									GuardarMensaje=1;
									//usart6DataReceived ='\0';
									}
								}


								if (usart6DataReceived != '\0')
									{
									if( (GuardarMensaje==1) & (usart6DataReceived != '\n'))
									{
										bufferGPS[i]=usart6DataReceived;
										i++;
										usart6DataReceived ='\0';
									}
								}


								if (usart6DataReceived != '\0')
								{
									if((GuardarMensaje==1) & (usart6DataReceived == '\n'))
									{
											bufferGPS[i]='\0';

											MensajeCompleto=1;
											GuardarMensaje=0;
											i=0;
											usart6DataReceived = '\0';
									}
								}

							}

							writeChar(&usart2Comm,'\n');

							for (n = 0 ; n < 5; n++)
							{
								CodigoMsg[n]=bufferGPS[n+1];
								sprintf(bufferMsg, "Char:%c \n", CodigoMsg[n]);
								writeMsg(&usart2Comm, bufferMsg);
							}
							CodigoMsg[5]='\0';


							sprintf(bufferMsg, "Codigo Mensaje: %s\n", CodigoMsg);
							writeMsg(&usart2Comm, bufferMsg);

							if (strcmp(CodigoMsg,"GPRMC")==0)
							//if ( (CodigoMsg[1]=='G') & (CodigoMsg[2]=='P') &(CodigoMsg[3]=='R') & (CodigoMsg[4]=='M') & (CodigoMsg[5]=='C') )
							{
								MensajeCorrecto=1;

								sprintf(bufferMsg, "Mensaje correcto !!\n");
								writeMsg(&usart2Comm, bufferMsg);

								sprintf(bufferMsg, "Buffer GPS: %s\n", bufferGPS);
								writeMsg(&usart2Comm, bufferMsg);



								//Cargamos la informacion del GPS en el arreglo infoSPA
								// infoSPA=hh,mm,ss,latlat,N,longlonglong,E
								//hora
								infoSPA[0]=bufferGPS[7];
								infoSPA[1]=bufferGPS[8];
								infoSPA[2]=':';
								//minutos
								infoSPA[3]=bufferGPS[9];
								infoSPA[4]=bufferGPS[10];
								infoSPA[5]=':';
								//segundo
								infoSPA[6]=bufferGPS[11];
								infoSPA[7]=bufferGPS[12];
								infoSPA[8]=':';
								//latitude
								infoSPA[9]=bufferGPS[19];
								infoSPA[10]=bufferGPS[20];
								infoSPA[11]=':';
								//Norte o Sur
								infoSPA[12]=bufferGPS[30];
								infoSPA[13]=':';
								//longitude
								infoSPA[14]=bufferGPS[32];
								infoSPA[15]=bufferGPS[33];
								infoSPA[16]=bufferGPS[34];
								infoSPA[17]=':';
								//Oeste o Este
								infoSPA[18]=bufferGPS[44];
								infoSPA[19]='\0';

								writeChar(&usart2Comm,'\n');


								sprintf(bufferMsg, "Info SPA :  %s\n", infoSPA);
								writeMsg(&usart2Comm, bufferMsg);


								//infoSPA[20]="03:41:56:06:N:075:W\0"
								sscanf(infoSPA, "%i:%i:%i:%i:%c:%i:%c", &hour,&min,&sec,&Lat,&dirLat,&Longi,&dirLong);

								//Calculo de SPA
							    //enter required input values into SPA structure
							    spa.year          = 2023;
							    spa.month         = 06;
							    spa.day           = 02;
							    spa.hour          = hour;
							    spa.minute        = min;
							    spa.second        = sec;
							    spa.timezone      = -5.0;
							    spa.delta_ut1     = 0;
							    spa.delta_t       = 67;

							    if (dirLong=='E')
							    {
							    	spa.longitude     = Longi; //
							    }
							    else
							    {
							    	spa.longitude     = -1*Longi; //
							    }

							    if (dirLat=='N')
							    {
							    	spa.latitude     = Lat;
							    }
							    else
							    {
							    	spa.latitude    = -Lat;
							    }

							    spa.elevation     = 1830.14;
							    spa.pressure      = 820;
							    spa.temperature   = 11;
							    spa.slope         = 30;
							    spa.azm_rotation  = -10;
							    spa.atmos_refract = 0.5667;
							    spa.function      = SPA_ALL;

								sprintf(bufferMsg, "Hora:%02d \n", (int)spa.hour);
								writeMsg(&usart2Comm, bufferMsg);
								sprintf(bufferMsg, "Minutos:%02d \n", (int)spa.minute);
								writeMsg(&usart2Comm, bufferMsg);
								sprintf(bufferMsg, "Segundos:%02d \n", (int)spa.second);
								writeMsg(&usart2Comm, bufferMsg);
								sprintf(bufferMsg, "Latitude:%02d \n", (int)spa.latitude);
								writeMsg(&usart2Comm, bufferMsg);
								sprintf(bufferMsg, "Dir Latitude:%c \n", dirLat);
								writeMsg(&usart2Comm, bufferMsg);

								sprintf(bufferMsg, "Longitude:%02d \n", (int)spa.longitude);
								writeMsg(&usart2Comm, bufferMsg);
								sprintf(bufferMsg, "Dir Longitude:%c \n", dirLong);
								writeMsg(&usart2Comm, bufferMsg);


							    //call the SPA calculate function and pass the SPA structure
							    result = spa_calculate(&spa);



								sprintf(bufferMsg, "Zenit indicado por el GPS:%02d \n", (int)spa.zenith);
								writeMsg(&usart2Comm, bufferMsg);
								sprintf(bufferMsg, "Azimut indicado por el GPS:%02d \n", (int)spa.azimuth);
								writeMsg(&usart2Comm, bufferMsg);

								//Movemos los servos
								movimientoServo((int)spa.zenith,(int)spa.azimuth);
							}

							else
							{
								sprintf(bufferMsg, "Mensaje incorrecto !!\n");
								writeMsg(&usart2Comm, bufferMsg);
								MensajeCompleto=0;

								//LimpiarBufferGPS
							}
						}
						MensajeCorrecto=0;


				}
			}
	}




	return 0;

}

void initSystem(void)
{

	//Configuracion del pin para encender el LED2
	handlerPinLED2.pGPIOx                             = GPIOA;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinNumber      = PIN_5;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinLED2.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerPinLED2);

	//Configurando el Timer2 para el blinky
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 5000;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;
	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerBlinkyTimer);


	//Configuracion del pin para encender el LED on la board cuando comunicamos con el GPS
	handlerBoardPin.pGPIOx                             = GPIOC;
	handlerBoardPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_8;
	handlerBoardPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	handlerBoardPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBoardPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBoardPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerBoardPin);






	//EL pin UserButton es una entrada simple que entragara la interrupcion EXTI-13
	//Observar que el pin seleccionado es el PIN13, por lo tanto el callback
	//que se debe configurar es el callback_extInt13()
	handlerUserButton.pGPIOx                             = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber      = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	// Cargamos la configuración del pinC13
	GPIO_Config(&handlerUserButton);

	handlerUserButtonExti.pGPIOHandler		=&handlerUserButton; //Puntero a la esctructura
	handlerUserButtonExti.edgeType          =EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);



	// Configuracion de la comunicacion serial CoolTerm
	handlerPinTX.pGPIOx 							= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx 							= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx                         = USART2;
	usart2Comm.USART_Config.USART_baudrate 		= USART_BAUDRATE_19200;
	usart2Comm.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity 		= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_mode 			= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
//usart2Comm.USART_Config.USART_enableIntRX
//usart2Comm.USART_Config.USART_enableIntTX
	//Cargando la configuracion del usart2
	USART_Config(&usart2Comm);




	// Configuracion de la comunicacion serial GPS

	/*handlerPinTXGPS.pGPIOx 							= GPIOA;
	handlerPinTXGPS.GPIO_PinConfig.GPIO_PinNumber 		= PIN_11;
	handlerPinTXGPS.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinTXGPS.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF8;
	GPIO_Config(&handlerPinTXGPS);
	*/

	handlerPinRXGPS.pGPIOx 								= GPIOA;
	handlerPinRXGPS.GPIO_PinConfig.GPIO_PinNumber 		= PIN_12;
	handlerPinRXGPS.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinRXGPS.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF8;
	GPIO_Config(&handlerPinRXGPS);

	usart6Comm.ptrUSARTx                        = USART6;
	usart6Comm.USART_Config.USART_baudrate 		= USART_BAUDRATE_9600;
	usart6Comm.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart6Comm.USART_Config.USART_parity 		= USART_PARITY_NONE;
	usart6Comm.USART_Config.USART_mode 			= USART_MODE_RXTX;
	usart6Comm.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
	usart6Comm.USART_Config.USART_interrupt     = 0;
	//Cargando la configuracion del usart6
	USART_Config(&usart6Comm);


	// PWM --------------------------------------------------------------------------------------------------

	// SERVO 1 ----------------------------------------------------------------------------------------------
	// Configuracion del pin para el servo1
	handlerPin1PwmChannel.pGPIOx 								= GPIOC;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinNumber 		= PIN_7;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPin1PwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	GPIO_Config(&handlerPin1PwmChannel);

	// Configuracion del timer para el servo1
	handlerSignal1PWM.ptrTIMx 									= TIM3;
	handlerSignal1PWM.config.channel							= PWM_CHANNEL_2;
	handlerSignal1PWM.config.duttyCicle							= duttyValue1;
	handlerSignal1PWM.config.periodo							= 20000; // 20ms 5OHz
	handlerSignal1PWM.config.prescaler					    	= 16;
	// Cargamos la configuración del timer
	pwm_Config(&handlerSignal1PWM);

	//Activamos la senal
	enableOutput(&handlerSignal1PWM);
	startPwmSignal(&handlerSignal1PWM);


	// SERVO 2 ----------------------------------------------------------------------------------------------
	// Configuracion del pin para el servo2
	handlerPin2PwmChannel.pGPIOx 								= GPIOA;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPin2PwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	GPIO_Config(&handlerPin2PwmChannel);

	// Configuracion del timer para el servo2
	handlerSignal2PWM.ptrTIMx 									= TIM3;
	handlerSignal2PWM.config.channel							= PWM_CHANNEL_1;
	handlerSignal2PWM.config.duttyCicle							= duttyValue2;
	handlerSignal2PWM.config.periodo							= 20000; // 20ms 5OHz
	handlerSignal2PWM.config.prescaler					    	= 16;
	// Cargamos la configuración del timer
	pwm_Config(&handlerSignal2PWM);

	//Activamos la senal
	enableOutput(&handlerSignal2PWM);
	startPwmSignal(&handlerSignal2PWM);



}


void printMsgGPS (void)
{
	if (usart6DataReceived != '\0')
	{
		writeChar(&usart2Comm, usart6DataReceived);
		usart6DataReceived = '\0';
	}
}

void movimientoServo(int Z, int A)
{
				//colocamos el angulo de zenith
				sprintf(bufferMsg, "Zenit:       %02d \n", (int)(Z));
				writeMsg(&usart2Comm, bufferMsg);

				//calculamos el dutty correspondiente
				duttyValue1=convertToDutty1(Z);
				if (duttyValue1<660)
				{
					duttyValue1=660;
				}
				if (duttyValue1>2200)
				{
					duttyValue1=2200;
				}

				//colocamos el nuevo valor de dutty asociado al zenith
				sprintf(bufferMsg, "Dutty asociado al Zenith:       %02d \n", (int)(duttyValue1));
				writeMsg(&usart2Comm, bufferMsg);

				//cargamos el nuevo valor de dutty
				updateDuttyCycle(&handlerSignal1PWM, duttyValue1);


				//AZIMUTH----------------------------------------------------

				Azimut =A;

				//colocamos el angulo de azimuth
				sprintf(bufferMsg, "Azimuth:       %02d \n", (int)(Azimut));
				writeMsg(&usart2Comm, bufferMsg);

				//calculamos el dutty correspondiente
				duttyValue2=convertToDutty2(Azimut);
				if (duttyValue2<840)
				{
					duttyValue2=840;
				}
				if (duttyValue2>2180)
				{
					duttyValue2=2180;
				}

				//colocamos el nuevo valor de dutty asociado al azimuth
				sprintf(bufferMsg, "Dutty asociado al Azimuth:       %02d \n", (int)(duttyValue2));
				writeMsg(&usart2Comm, bufferMsg);

				//cargamos el nuevo valor de dutty
				updateDuttyCycle(&handlerSignal2PWM, duttyValue2);
}


int16_t convertToDutty1 (double angle)
{
	return 660+(1540*angle/180);
}


int16_t convertToDutty2 (double angle)
{
	if (angle>270 || angle<90)
	{
		sprintf(bufferMsg, "Ubicar el equipo al Norte ! \n");
		writeMsg(&usart2Comm, bufferMsg);

		if (angle<90)
		{
			newDutty= 1600-(angle*760/90);
		}
		if (angle>270)
		{
			angle=360-angle;
			newDutty= 1600+(angle*580/90);
		}
	}

	if (angle<270 && angle>90)
	{
		sprintf(bufferMsg, "Ubicar el equipo al Sur ! \n");
		writeMsg(&usart2Comm, bufferMsg);

		if (angle<180)
		{
			angle=180-angle;
			newDutty= 1600+(angle*580/90);
		}
		if (angle>180)
		{
			angle=angle-180;
			newDutty= 1600-(angle*760/90);
		}
	}

	return newDutty;
}


//Callback del Timer2 - Hacemos un blinky
void BasicTimer2_Callback(void)
{
	GPIOxTooglePin(&handlerBoardPin);
	GPIOxTooglePin(&handlerPinLED2);
}

//Callback del userButton --> hacer algo
void callback_extInt13(void)
{
		__NOP();
}

// Esta funcion se ejecuta cada vez que un caractera es recibido
// por el puerto USART2
void usart2Rx_Callback (void)
{
	usart2DataReceived =getRxData();
}

// por el puerto USART6
void usart6Rx_Callback (void)
{
	usart6DataReceived =getRxData();
}


