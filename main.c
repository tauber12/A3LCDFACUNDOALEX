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
/*
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : main.c
 * @brief          : main program body, FSM logic for LCD timer
 * project         : EE 329 S'26 A3
 * authors         : Alex Tauber
 * version         : 0.1
 * date            : 260415
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * MAIN PLAN :
 * initialize system peripherals (LCD, keypad, SysTick, LED); display intro
 * message; enter FSM loop. States: CHECKKEYPRESS waits for initial '*' press;
 * ENTER_DIGITS prompts user for 4 timer digits, validates and caps at 59:59,
 * waits for '#' to start or '*' to re-enter; TIMER counts down one second per
 * iteration using calibrated for-loop delay, checks for '*' to reset, updates
 * LCD each tick, transitions to DONE at 00:00; DONE illuminates LED and waits
 * for any keypress to return to ENTER_DIGITS.
 *******************************************************************************
 *
 *
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"

void SystemClock_Config(void);

/*-----------------------------------------------------------------------------
 * typedef : State_t
 * values  : STATE_CHECKKEYPRESS, STATE_ENTER_DIGITS, STATE_TIMER, STATE_DONE
 * action  : defines FSM states for main loop switch decoder; enables constant
 *           '*' detection across all states of operation
 * authors : Alex Tauber
 * date    : 260415
 *----------------------------------------------------------------------------*/
typedef enum {
    STATE_CHECKKEYPRESS,
    STATE_ENTER_DIGITS,
    STATE_TIMER,
    STATE_DONE
} State_t;

/*-----------------------------------------------------------------------------
 * function : main( )
 * INs      : none
 * OUTs     : int (never returns)
 * action   : initializes system peripherals; displays intro message; enters
 *            infinite FSM loop decoding current state and executing
 *            corresponding logic for LCD timer operation
 * authors  : Alex Tauber
 * version  : 0.1
 * date     : 260415
 *----------------------------------------------------------------------------*/
int main(void)
{
  /* system initialization */
  HAL_Init();
  SystemClock_Config();
  LCD_init();
  Keypad_Config();
  SysTick_Init();
  LED_Config();

  State_t state = STATE_CHECKKEYPRESS; // initialize FSM to first state
  uint8_t inputtedDigits[4];           // array to store 4 user-inputted digits

  LCD_write_intro_message();           // display EE329 A3 TIMER intro message

  /* main FSM loop - runs indefinitely */
  while (1)
  {
	  delay_us(1000);
	  switch(state){ // decode current state

	  	  /*-------------------------------------------------------------------------
	  	   * STATE_CHECKKEYPRESS : wait for initial '*' keypress to begin timer entry
	  	   * transitions to STATE_ENTER_DIGITS on '*' press; remains otherwise
	  	   *------------------------------------------------------------------------*/
	  	  case STATE_CHECKKEYPRESS:

	  		  // wait for '*' press; transition to digit entry state if detected
	  		  state = (Return_ValidKeyPressLCD() == '*') ? STATE_ENTER_DIGITS : STATE_CHECKKEYPRESS;
	  		  break;

	  	  /*-------------------------------------------------------------------------
	  	   * STATE_ENTER_DIGITS : prompt user for 4 timer digits; validate and cap
	  	   * input at 59:59; wait for '#' to start timer or '*' to re-enter digits
	  	   *------------------------------------------------------------------------*/
		  case STATE_ENTER_DIGITS:

			  Prompt_user_4_numbers();              // display digit entry prompt on LCD
			  Wait_for_4_User_Digits(inputtedDigits); // block until 4 valid digits entered

			  /* validate inputted digits and cap at 59:59 if exceeded */
			  if( inputtedDigits[0] > '5' && inputtedDigits[2] > '5' ) {

				  /* both minutes tens and seconds tens exceed 5 - cap both to 59 */
			      inputtedDigits[0] = '5';
			      inputtedDigits[1] = '9';
			      inputtedDigits[2] = '5';
			      inputtedDigits[3] = '9';
			      LCD_set_cursor(1, 11);
			      LCD_write_string("59");
			      LCD_set_cursor(1, 14);
			      LCD_write_string("59");

			  } else if( inputtedDigits[0] > '5' ) {

				  /* minutes tens digit exceeds 5 - cap minutes to 59 */
				  inputtedDigits[0] = '5';
				  inputtedDigits[1] = '9';
			      LCD_set_cursor(1, 11);
			      LCD_write_string("59");

			  } else if( inputtedDigits[2] > '5' ) {

				  /* seconds tens digit exceeds 5 - cap seconds to 59 */
				  inputtedDigits[2] = '5';
				  inputtedDigits[3] = '9';
			      LCD_set_cursor(1, 14);
			      LCD_write_string("59");
			  }

			  /* wait for '#' to start timer or '*' to re-enter digits */
			  while(1){

				  uint8_t press = Return_ValidKeyPressLCD(); // block until valid keypress

				  if( press == '#' ) {

					  /* '#' detected - start timer, update LCD header, transition state */
					  state = STATE_TIMER;
					  LCD_set_cursor(0, 0);
					  LCD_write_string("EE 329 A3 TIMER ");
					  break;

				  } else if( press == '*' ) {

					  /* '*' detected - re-enter digit entry state */
					  state = STATE_ENTER_DIGITS;
				  	  break;
			  	  }
			  }
			  break;

	  	  /*-------------------------------------------------------------------------
	  	   * STATE_TIMER : count down inputted time one second per iteration;
	  	   * check for '*' each ~50ms chunk to minimize reset latency; update LCD
	  	   * each tick; transition to STATE_DONE when 00:00 reached
	  	   *------------------------------------------------------------------------*/
		  case STATE_TIMER:

			  /* calibrated for-loop delay for ~1 second total;
			   * split into 17 x ~51.2ms chunks to allow '*' detection mid-second */
	          for( uint16_t idx = 0; idx < 17; idx++){

	        	  /* check for '*' press each chunk - break immediately if detected */
	        	  if( Keypad_IsAnyKeyPressed() ){
	        		 if( Keypad_WhichKeyIsPressed() == 0xA ){
	        			 state = STATE_ENTER_DIGITS;
	        			 break;
	        		 }
	        	  }
	        	  delay_us(51200); // ~51.2ms delay per chunk (calibrated for 1s total)
	          }

	          /* check if timer has reached 00:00 - transition to done state */
	          if(inputtedDigits[0]=='0' && inputtedDigits[1]=='0'
	            && inputtedDigits[2]=='0' && inputtedDigits[3]=='0'){

	            state = STATE_DONE;
	            break;

	          /* decrement ones (sec) if nonzero */
	          } else if(inputtedDigits[3]>'0'){
	            inputtedDigits[3]--;

	          /* decrement tens (sec), reset ones (sec) to 9 */
	          } else if(inputtedDigits[2]>'0'){
	            inputtedDigits[2]--;
	            inputtedDigits[3]='9';

	          /* decrement ones (min), reset seconds to 59 */
	          } else if(inputtedDigits[1]>'0'){
	            inputtedDigits[1]--;
	            inputtedDigits[2]='5';
	            inputtedDigits[3]='9';

	          /* decrement tens (min), reset ones (min) to 9, reset seconds to 59 */
	          } else if(inputtedDigits[0]>'0'){
	            inputtedDigits[0]--;
	            inputtedDigits[1]='9';
	            inputtedDigits[2]='5';
	            inputtedDigits[3]='9';
	          }

	          /* second '*' check after decrement logic - catches press during LCD update */
	          if( Keypad_IsAnyKeyPressed() ){
	        	  if( Return_ValidKeyPressLCD() == '*' ){
	        		  state = STATE_ENTER_DIGITS;
	        		  break;
	        	  }
	          }

	          Update_time(inputtedDigits); // update LCD with new countdown time
	          state = STATE_TIMER;         // remain in timer state for next tick
	          break;

	  	  /*-------------------------------------------------------------------------
	  	   * STATE_DONE : timer has reached 00:00; illuminate LED; wait for any
	  	   * keypress to turn off LED and return to digit entry state
	  	   *------------------------------------------------------------------------*/
		  case STATE_DONE:

			  LED_PORT -> BSRR |= LED_PIN; // turn on LED to signal timer completion

			  /* wait for '#' or '*' press to reset and return to digit entry */
	          while(1){
	        	  if( Return_ValidKeyPressLCD() == '#' || Return_ValidKeyPressLCD() == '*') {
	        		  LED_PORT -> BRR |= LED_PIN; // turn off LED
	        		  state = STATE_ENTER_DIGITS;
	        		  break;
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

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
