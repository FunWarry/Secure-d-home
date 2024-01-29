/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "systick.h"
#include "hc-sr04.h"
#include "MCP9701.h"
#include "photoresistance.h"
#include "CNY70.h"
#include "DHT11_modif.h"
#include "tft_ili9341/stm32f1_ili9341.h"

/**
 * @brief change l'�tat de la led s�lectionn�e
 * @post allume ou �teint la led
 * @param b
 */
void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

/**
 * @brief lit si le boutton est press�
 * @return 0 ou 1 en fonction de l'�tat du boutton
 */
bool_e readButton(void)
{
	return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}

static volatile uint32_t t = 1000000;

/**
 * @brief reproduit le temps avec une unit� en millisecondes
 * @pre d�clarer la variable t comme volatile
 * @post d�cr�mente la variable t toutes les millisecondes pour cr�er un timer
 */
void process_ms(void)
{
	if(t)
		t--;
}

/**
 * @brief methode principale du projet
 * @post cette m�thode est �xecut�e en premier et tourne en t�che de fond ex�cutant sans fin la machine � �tat
 */
int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la premi�re �tape de la fonction main().
	HAL_Init();

	//Active les horloges des p�riph�riques GPIO
	BSP_GPIO_Enable();

	//Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	UART_init(UART2_ID,115200);

	//"Indique que les printf sortent vers le p�riph�rique UART2."
	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms � la liste des fonctions appel�es automatiquement chaque ms par la routine d'interruption du p�riph�rique SYSTICK
	Systick_add_callback_function(&process_ms);

	typedef enum
	{
		INIT,
		FIRST_ROUND,
		SCREEN_ON,
		WAIT,
		SCREEN_OFF
	}state_e;

	static state_e state = INIT;
	static char mem_HCSR04 [100];
	static char mem_DHT11 [100];
	static char mem_Light [100];
	static char mem_Temp [100];

	while(1)	//boucle de t�che de fond
	{
		switch(state)
		{
		case INIT:
			ILI9341_Init();
			init_HCSR04();
			init_DHT11();
			init_light();
			init_MCP();
			init_CNY70();
			state = FIRST_ROUND;
			break;
		case FIRST_ROUND:
			sprintf(mem_HCSR04, HCSR04_read());
			sprintf(mem_DHT11, read_DHT11());
			sprintf(mem_Light, get_data_light());
			sprintf(mem_Temp, recup_data_MCP());
			state = SCREEN_ON;
			break;
		case SCREEN_ON:
			ILI9341_Init();
			ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
			ILI9341_printf(15,10, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, mem_HCSR04);
			ILI9341_printf(15,60, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, mem_DHT11);
			ILI9341_printf(15,110, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, mem_Light);
			ILI9341_printf(15,180, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, mem_Temp);
			state = WAIT;
			t = 7000;
			break;
		case WAIT:
			if(read_CNY70()){
				t = 7000;
			}else if(!read_CNY70() && !t){
				state = SCREEN_OFF;
				ILI9341_Init();
				ILI9341_Fill(ILI9341_COLOR_BLACK);
			}
			break;
		case SCREEN_OFF:
			if (read_CNY70()){
				state = SCREEN_ON;
			}else{
				sprintf(mem_HCSR04, HCSR04_read());
				sprintf(mem_DHT11, read_DHT11());
				sprintf(mem_Light, get_data_light());
				sprintf(mem_Temp, recup_data_MCP());
			}
			break;
		default:
			break;
		}
	}
}
