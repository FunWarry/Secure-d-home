/*
 * CNY70.c
 *
 *  Created on: 21 d�c. 2023
 *      Author: Alexandre GASCOIN
 *      Ce module est d�di� au capteur infrarouge CNY70.
 *      Il sert pour l'allumage de l'�cran.
 */


#include "CNY70.h"
#include "stm32f1_gpio.h"

/**
 * @brief initialise le capteur infrarouge
 * @pre avoir correctement branch� le capteur
 * @post initialise et formate la lecture de donn�e sur le p�riph�rique
 */
void init_CNY70(void){
	BSP_GPIO_PinCfg(GPIOB,GPIO_PIN_13,GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
}

/**
 * @brief capte s'il y a quelque chose devant le capteur
 * @pre avoir initialis� le capteur
 * @return renvoie 1 s'il trouve quelque chose, 0 sinon
 */
uint8_t read_CNY70(void){
	if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)){
		return 1;
	}else{
		return 0;
	}
}
