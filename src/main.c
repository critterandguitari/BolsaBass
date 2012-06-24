/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/



#ifdef __USE_CMSIS
#include "stm32f4xx.h"
#endif

/* Includes ------------------------------------------------------------------*/

#include "arm_math.h"
#include "CS4344.h"
#include "pp6.h"
#include "miditof.h"
#include "mode_filter_man.h"
#include "mode_simple_sin.h"

extern unsigned int software_index;
extern unsigned int hardware_index;
extern short play_buf[];
extern uint32_t sample_clock;


extern pocket_piano pp6;



void Delay(__IO uint32_t nCount);


int main(void)
{

//for(;;);
	Delay(20000);

  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */


	pp6_leds_init();
	pp6_knobs_init();

	pp6_keys_init();

	// init codec
	CS4344_init();

	float32_t sig, f;
	q15_t  wave;



	f = 50.0;
	uint32_t k, k_last, kdown, note, note_last, i;
	k = note = note_last = 0;

	mode_filter_man_init();
	mode_simple_sin_init();
	while (1)	{

	    /* Update WWDG counter */
	    //WWDG_SetCounter(127);

		/*
		 * Control Rate
		 */
		if (!(sample_clock & 0x3F)){
			GPIO_ToggleBits(GPIOE, GPIO_Pin_0);
			sample_clock++;
			pp6_keys_update();
			pp6_knobs_update();

			k = pp6_get_keys();
			kdown = 0;
			for (i = 0; i < 16; i++) {
				if (!((k >> i) & 1)) {
					pp6_set_note(i);
					kdown = 1;
					break;
				}
			}
			if ((k != k_last) && kdown) {
				pp6_set_note_start();
				k_last = k;
			}
			//mode_filter_man_control_process();
			mode_simple_sin_control_process();
		}

		/*
		 * Sample Rate
		 */
		if (software_index != hardware_index){
			if (software_index & 1){   // channel

				//sig = mode_filter_man_sample_process();
				sig = mode_simple_sin_sample_process();

				arm_float_to_q15(&sig, &wave, 1);

				// put the r
				play_buf[software_index] = wave;
				software_index++;
				software_index &= 0xf;
			}
			else {   // channel 2, do the same thing
				// put the l
				play_buf[software_index] = wave;
				software_index++;
				software_index &= 0xf;
	    	}
	    }
	}
}

/*
filter_man_control_process(){


}*/



/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
