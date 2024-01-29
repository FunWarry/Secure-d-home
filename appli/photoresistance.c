/*
 * photoresistance.c
 *
 *  Created on: 21 d�c. 2023
 *      Author: Alexandre GASCOIN
 *
 *      Ce module est d�di� � la photor�sistance.
 *      Elle capte le niveau de luminosit� en faisant varier sa r�sistance.
 */

#include "photoresistance.h"
#include "stm32f1_adc.h"
#include "systick.h"

static volatile uint16_t t = 0;
static int8_t adc_id[ADC_CHANNEL_NB];
char reponse2 [50];

/**
 * @brief reproduit le temps avec une unit� en millisecondes
 * @pre avoir d�clar� la variable t en volatile
 * @post d�cr�mente la varible t pour fabriquer un timer
 */
void light_adc_process_1ms(void)
{
	if(t)
		t--;
}

/**
 * @brief active le timer et initialise l'adc
 * @pre avoir d�clar� la variable t en variable
 * @post � l'issue active en permanence des conversions analogiques
 * @post ajoute la fonction light_adc_process_1ms � la routine d'interruption
 */
void init_light(void){
	ADC_init();
	Systick_add_callback_function(light_adc_process_1ms);
	printf("Analog to Digital Converter\n");
}

/**
 * @brief lit la photor�sistance pour conna�tre le niveau de luminosit� environnant
 * @pre avoir initialis� le timer
 * @pre avoir initialis� l'ADC
 * @return renvoie une cha�ne de caract�re qui indique la valeur de la r�sistance variable.
 * Plus cette r�sistance est haute et plus il fait sombre.
 */
char* get_data_light(void){
	int16_t value;
	int16_t millivolt;
	int16_t resistanceLdr;
	int16_t control;
	control = 0;
	while(!control)
	{
	if(!t){
		t = 400;
		if(adc_id[ADC_8] != -1){
			value = ADC_getValue(ADC_8);
			millivolt = (int16_t)((((int32_t)value)*3300)/4096);		//On la convertie en volts
			resistanceLdr = (int16_t)((int32_t)(3300-millivolt)*1000/millivolt);
			printf("La resistance est a : %4d Ohms", resistanceLdr);
			sprintf(reponse2, "La resistance est a : %4d Ohms", resistanceLdr);
			control = 1;
			return reponse2;
		}
		printf("\n");
	}
	}
	return NULL;
}
