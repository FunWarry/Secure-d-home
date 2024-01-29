/*
 * MCP9701.c
 *
 *  Created on: 21 d�c. 2023
 *      Author: Alexandre GASCOIN
 *		Ce module est d�di� au capteur de temp�rature.
 *		Le capteur renvoie la temp�rature ambiante.
 */

#include "MCP9701.h"
#include "stm32f1_adc.h"
#include "systick.h"

static volatile uint16_t t = 0;
static int8_t adc_id[ADC_CHANNEL_NB];
char reponse3 [50];

/**
 * @brief reproduit le temps avec une unit� en millisecondes
 * @pre avoir d�clar� la variable t en volatile
 * @post d�cr�mente la varible t pour fabriquer un timer
 */
void MCP_adc_process_1ms(void)
{
	if(t)
		t--;
}

/**
 * @brief appelle la fonction MCP_adc_process_1ms en continue
 * @post ajoute la fonction MCP_adc_process_1ms � la routine d'interruption
 */
void init_MCP(void){
	Systick_add_callback_function(MCP_adc_process_1ms);
}

/**
 * @brief lit la temp�rature mesur�e par le capteur
 * @pre avoir initialis� l'ADC (methode init_Light)
 * @pre avoir lanc� la fonction init_MCP pour le timer
 * @return renvoie la temp�rature ambiante en cha�ne de caract�re et en degr� Celsius
 */
char* recup_data_MCP(void){
	int16_t value;
	int16_t millivolt;
	if(!t){
		t = 400;
		if(adc_id[ADC_0] != -1){
			value = ADC_getValue(ADC_0);
			millivolt = (int16_t)((((int32_t)value)*3300)/4096);		//On la convertie en volts
			millivolt /= 33;
			printf("Il fait une temperature de : %1d�C", millivolt);
			sprintf(reponse3, "Il fait une temperature de : %1d�C", millivolt);
			return reponse3;
		}
		printf("\n");
	}
	return NULL;
}
