/*
 * DHT11.c
 *
 *  Created on: 21 d�c. 2023
 *      Author: Alexandre GASCOIN
 *      Ce module est d�di� au capteur d'humditi�, le DHT11.
 *      Il renvoie l'humidit� et la temp�rature environnante.
 */

#include "DHT11_modif.h"
#include "dht11.h"
#include "systick.h"

char reponse1[50];

/**
 * @brief initialise le capteur d'humidit�
 * @pre avoir correctement branch� le capteur
 * @post active la lecture du p�riph�rique indiqu�
 */
void init_DHT11(void)
{
	DHT11_init(GPIOB, GPIO_PIN_12);
}

/**
 * @brief lit l'information du capteur
 * @pre avoir initialis� le capteur
 * @return on obtient une cha�ne de caract�re qui diff�re selon les cas :
 * 1er cas : tout se passe bien et on obtient l'humidit� et la temp�rature
 * 2�me cas : le programme renvoie une erreur
 * 3�me cas : le programme tourne en rond donc il le d�tecte et renvoie timeout.
 */
char* read_DHT11(void)
{
	static uint8_t humidity_int;
	static uint8_t humidity_dec;
	static uint8_t temperature_int;
	static uint8_t temperature_dec;
	static uint8_t control;
	control = 0;

	while(!control)
	{
		switch(DHT11_state_machine_get_datas(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec))
		{
			case END_OK:
				debug_printf("DHT11 h=%d,%d | t=%d,%d\n",humidity_int, humidity_dec, temperature_int, temperature_dec);
				sprintf(reponse1, "DHT11 h=%d,%d | t=%d,%d\n",humidity_int, humidity_dec, temperature_int, temperature_dec);
				HAL_Delay(1500);
				control = 1;
				return reponse1;
				break;
			case END_ERROR:
				debug_printf("DHT11 read error (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				sprintf(reponse1, "DHT11 read error (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				HAL_Delay(1500);
				control = 1;
				return reponse1;
				break;
			case END_TIMEOUT:
				debug_printf("DHT11 timeout (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				sprintf(reponse1, "DHT11 timeout (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				HAL_Delay(1500);
				control = 1;
				return reponse1;
				break;
			default:
				break;
		}
	}
	return NULL;
}
