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
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	uint8_t pastKey;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  //sys intialization
  HAL_Init();
  SystemClock_Config();
  LCD_init();
  Keypad_Config();
  LCD_write_intro_message();
  

  while (1)
  {
	  while( (Return_ValidKeyPressLCD() =! '*') ){
		  //wait for initial star press
	  }
    uint8_t press_count=0;//to track number of user inputs
    uint8_t LCD_in[4]={}; //array to store button press input 
    //prompt 4 user inputs here
    while(press_count<4);//to store 4 inputs
    {
      LCD_in[press_count]= Return_ValidKeyPressLCD();
      if(LCD_in[press_count]='*'){
        break;}
      else
      {
      //call function to update LCD with each button press
      press_count++;
      }
    }
    //after 4 user inputs, parse them and update LCD if needed
    minutes= ((LCD_in[3]*10)+(LCD_in[2]));
    seconds= ((LCD_in[1]*10)+(LCD_in[0]));
    parse_flag=0;
    if(minutes > 59)
      {
        minutes =59;
        parse_flag=1;
      }
    else
     {
      //nop
     }
    if(seconds > 59)
     {
      seconds = 59;
      parse_flag=1;
     }
    else
      {
      //nop
      }
    if (parse_flag=1)
      {
      uint8_t parsed_LCD_in[4]={};
      }
    else
      {
        //nop
      }

    //initalize countdown logic
    uint8_t countbegin=0;
    uint8_t countdone=0;//flag for when countdown is finished
    countbegin=return_ValidKeyPressLCD();
    if(countbegin = '*')
    {
      break;
    }
    else
    {
      while(countdone=0){//countdown loop
      uint8_t countbreak=0; 
      if(Keypad_IsAnyKeyPressed()){//check if star key is pressed
				  countbreak = Keypad_WhichKeyIsPressed();}
      else{}//nop
      
      if(countbreak== '*'){
          break;
        }
      else{
      //do systick timer delay here
      //if minutes count >0 decrement minutes
      //else if seconds count >0 decrement seconds
      //update LCD after either one
      //else countdone=1
        }
      }
    }
    }
	  delay_us(10000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
