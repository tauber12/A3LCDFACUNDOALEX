/*
 * lcd.h
 *
 *  Created on: Apr 15, 2026
 *      Author: alexm
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l4xx_hal.h"
#include <string.h>

#define LED_PORT GPIOC
#define LCD_PORT GPIOC
#define LCD_EN 0x10
#define LCD_RS 0x40
#define LCD_DATA_BITS 0xF

//function protoypes
void LED_Config( void );
void LCD_init( void );
void LCD_pulse_ENA( void );
void LCD_4b_command( uint8_t command );
void LCD_command( uint8_t command );
void LCD_write_char( uint8_t letter );
void LCD_write_string( char string[] );
void LCD_set_cursor( uint8_t row, uint8_t column);
void LCD_write_intro_message( );


#endif /* INC_LCD_H_ */
