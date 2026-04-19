/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */

typedef enum { //create states for FSM logic to aid  constant '*" checks
    STATE_CHECKKEYPRESS,
    STATE_ENTER_DIGITS,
    STATE_TIMER,
    STATE_DONE
} State_t;

int main(void)
{
  //sys intialization
  HAL_Init();
  SystemClock_Config();
  LCD_init();
  Keypad_Config();
  SysTick_Init();

  State_t state = STATE_CHECKKEYPRESS; // initialize first state
  uint8_t inputtedDigits[4]; //create uint8_t array to store user inputs

  LCD_write_intro_message(); // EE329 A3 TIMER message

  while (1) //loop indefinitely
  {
	  delay_us(1000);
	  switch(state){ //decode current state

	  	  case STATE_CHECKKEYPRESS: //check for keypress in this state

	  		  state =  (Return_ValidKeyPressLCD() == '*') ? STATE_ENTER_DIGITS : STATE_CHECKKEYPRESS; //wait for initial "*' press
	  		  break;

		  case STATE_ENTER_DIGITS:  //prompt for timer digits, cap if above 59:59

			  Prompt_user_4_numbers();
			  Wait_for_4_User_Digits(inputtedDigits);

			  if( inputtedDigits[0] > '5' && inputtedDigits[2] > '5' ) {
			      inputtedDigits[0] = '5';
			      inputtedDigits[1] = '9';
			      inputtedDigits[2] = '5';
			      inputtedDigits[2] = '9';
			      LCD_set_cursor(1, 11);
			      LCD_write_string("59");
			      LCD_set_cursor(1, 14);
			      LCD_write_string("59");

			  } else if( inputtedDigits[0] > '5' ) {
				    inputtedDigits[0] = '5';
				    inputtedDigits[1] = '9';
			      LCD_set_cursor(1, 11);
			      LCD_write_string("59");

			  } else if( inputtedDigits[2] > '5' ) {
				    inputtedDigits[2] = '5';
				    inputtedDigits[3] = '9';
			      LCD_set_cursor(1, 14);
			      LCD_write_string("59");
			  }

			  while(1){
				  if( Return_ValidKeyPressLCD() == '#' )
          {
					  state =  STATE_TIMER;
					  break;
				  } 
          else if( Return_ValidKeyPressLCD() == '*' )
          {
					  state = STATE_ENTER_DIGITS;
				  	  break;
			  	}
			  }

		  case STATE_TIMER:
        delay_us(250000); //delay for about 1 second
        if(inputtedDigits[0]=='0' && inputtedDigits[1]=='0' 
          && inputtedDigits[2]=='0' && inputtedDigits[3]=='0')
        {
          state= STATE_DONE;
          break;
        }
        else if(inputtedDigits[3]>'0') //check ones (sec)
        {
          inputtedDigits[3]--; //decrement seconds count
        }
        else if(inputtedDigits[2]>'0') //check tens (sec)
        {
          inputtedDigits[2]--; //decrements tens (sec) 
          inputtedDigits[3]='9'; //set ones(sec) to nine
        }
        else if(inputtedDigits[1]>'0') //check ones (min)
        {
          inputtedDigits[1]--;//decrement ones (min)
          inputtedDigits[2]='5'; //set seconds to 59
          inputtedDigits[3]='9';
        }
        else if(inputtedDigits[0]>'0') //check tens (min)
        {
          inputtedDigits[0]--;//decrement tens (min)
          inputtedDigits[1]='9';//set ones (min) to 9
          inputtedDigits[2]='5';//set seconds to 59
          inputtedDigits[3]='9';
        }
        LCD_set_cursor(1,15); //update LCD with new coutdown time
        LCD_write_char(inputtedDigits[3]);
        LCD_set_cursor(1,14);
        LCD_write_char(inputtedDigits[2]);
        LCD_set_cursor(1,12);
        LCD_write_char(inputtedDigits[1]);
        LCD_set_cursor(1,11);
        LCD_write_char(inputtedDigits[0]);

		  case STATE_DONE:       
        LED_PORT->BSRR = 0x10;//turn on LED
        while(1){
				  if( Return_ValidKeyPressLCD() == '#' || Return_ValidKeyPressLCD() == '*')
          {
            LED_PORT -> BRR =0x10;//turn off LED
					  state =  STATE_ENTER_DIGITS;
					  break;
				  } 
          else()
          {
            //nop
			  	}
			  }
	   }

    }
 }


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
