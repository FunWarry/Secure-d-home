/*
 * hc-sr04.c
 *
 *  Created on: 8 janv. 2024
 *      Author: Alexandre GASCOIN
 *      Ce module est d�di� au capteur � ultrasons, le HC-SR04.
 *      Le trigger d�clenche un signal et le echo re�oit le signal. Il faut donc configurer une sortie et une entr�e.
 */

#include "hc-sr04.h"
#include "HC-SR04/HCSR04.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_timer.h"
#include "stm32f1_gpio.h"
#include "stm32f1_extit.h"
#include <stdio.h>


//Si La p�riode de mesure est inf�rieure, elle ne sera respect�e que si le capteur � pr�sent� un �cho moins long.
#define PERIOD_MEASURE			100

char reponse[50];

/**
 * @brief initialise le capteur � ultrasons HC-SR04
 * @pre avoir correctement branch� le capteur
 * @post associe les voies echo et trigger aux pins correspondant.
 */
void init_HCSR04(void)
{
	static uint8_t id_sensor;

	if(HCSR04_add(&id_sensor, GPIOB, GPIO_PIN_7, GPIOB, GPIO_PIN_8) != HAL_OK)
	{
		printf("HCSR04 non ajout� - erreur g�nante\n");
	}else{
		printf("HCSR04 ajout�\n");
	}
}

/**
 * @brief lit la donn�e per�ue par l'ECHO
 * @pre avoir initialis� le capteur
 * @return renvoie une cha�ne de caract�re en fonction de 3 �tats diff�rents :
 * 1er cas : tout se passe bien, on re�oie la distance entre le capteur et le premier truc qui est devant
 * 2�me cas : une erreur s'est produite
 * 3�me cas : le programme boucle et donc on nous renvoie le timeout
 */
char* HCSR04_read(void)
{
	typedef enum
	{
		LAUNCH_MEASURE,
		RUN,
		WAIT_DURING_MEASURE,
		WAIT_BEFORE_NEXT_MEASURE
	}state_e;

	static state_e state = LAUNCH_MEASURE;
	static uint32_t tlocal;
	static uint8_t id_sensor;
	static uint8_t control;
	uint16_t distance;
	control = 0;
	while(!control)
	{
		//ne pas oublier d'appeler en t�che de fond cette fonction.
		HCSR04_process_main();
		switch(state)
		{
			case LAUNCH_MEASURE:
				HCSR04_run_measure(id_sensor);
				tlocal = HAL_GetTick();
				state = WAIT_DURING_MEASURE;
				break;
			case WAIT_DURING_MEASURE:
				switch(HCSR04_get_value(id_sensor, &distance))
				{
					case HAL_BUSY:
						//rien � faire... on attend...
						break;
					case HAL_OK:
						printf("sensor %d - distance : %d\n", id_sensor, distance);
						sprintf(reponse, "sensor %d - distance : %d\n", id_sensor, distance);
						state = WAIT_BEFORE_NEXT_MEASURE;
						control = 1;
						return reponse;
						break;
					case HAL_ERROR:
						printf("sensor %d - erreur ou mesure non lancee\n", id_sensor);
						sprintf(reponse, "sensor %d - erreur ou mesure non lancee\n", id_sensor);
						state = WAIT_BEFORE_NEXT_MEASURE;
						control = 1;
						return reponse;
						break;
					case HAL_TIMEOUT:
						printf("sensor %d - timeout\n", id_sensor);
						sprintf(reponse, "sensor %d - timeout\n", id_sensor);
						state = WAIT_BEFORE_NEXT_MEASURE;
						control = 1;
						return reponse;
						break;
				}
				break;
			case WAIT_BEFORE_NEXT_MEASURE:
				if(HAL_GetTick() > tlocal + PERIOD_MEASURE)
					state = LAUNCH_MEASURE;
				break;
			default:
				break;
		}
	}
	return NULL;
}
