/*
 *******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : lcd.h
 * @brief          : header file for lcd.c; contains function prototypes for
 * LCD and LED configuration, initialization, and display functions; defines
 * for LCD control pin masks, data bit mask, and port assignments for
 * NHD-0216HZ-FSW-FBW-33V3C 2x16 character LCD in 4-bit parallel mode
 * project         : EE 329 S'26 A3
 * authors         : Alex Tauber
 * version         : 0.1
 * date            : 260415
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * PIN ASSIGNMENTS (all on GPIOC)
 *      signal      – ODR bit mask – pin
 * LCD  DB4-DB7     – 0x0F         – PC0-PC3  (4-bit data bus, high nibble)
 * LCD  EN          – 0x10         – PC4      (enable, falling edge triggered)
 * LCD  RS          – 0x40         – PC6      (register select: 0=cmd, 1=data)
 * LCD  RW          – GND                     (write only, tied low)
 * LED              – 0x100        – PC8      (timer done indicator)
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l4xx_hal.h"
#include <string.h>

/* port assignments */
#define LED_PORT GPIOC               // LED output port
#define LCD_PORT GPIOC               // LCD data and control port

/* LCD control pin masks (ODR bit values) */
#define LED_PIN       0x100          // PC8  - timer done LED
#define LCD_EN        0x10           // PC4  - enable line (falling edge latches data)
#define LCD_RS        0x40           // PC6  - register select (0=instruction, 1=data)
#define LCD_DATA_BITS 0xF            // PC0-PC3 - 4-bit data bus (DB4-DB7)

/* lcd.c function declarations */
void LED_Config( void );                                    // configure PC8 as LED output
void LCD_init( void );                                      // initialize LCD in 4-bit mode
void LCD_pulse_ENA( void );                                 // generate EN falling edge pulse
void LCD_4b_command( uint8_t command );                     // send high nibble only (wake-up)
void LCD_command( uint8_t command );                        // send full byte in 4-bit mode
void LCD_write_char( uint8_t letter );                      // write single ASCII character
void LCD_write_string( char string[] );                     // write null-terminated string
void LCD_set_cursor( uint8_t row, uint8_t column );         // set DDRAM address by row/column
void LCD_write_intro_message( void );                       // display EE329 A3 TIMER intro
void Prompt_user_4_numbers( void );                         // display digit entry prompt
void Update_entered_numbers( uint8_t nums[4], uint8_t currentCnt ); // update LCD with each digit press
void Wait_for_4_User_Digits( uint8_t lcd_In[4] );           // block until 4 digits entered
void Update_time( uint8_t lcd_In[4] );                      // update LCD countdown display

#endif /* INC_LCD_H_ */
