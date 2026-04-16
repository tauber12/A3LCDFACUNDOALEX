/*
 * lcd.h
 *
 *  Created on: Apr 15, 2026
 *      Author: alexm
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#define LCD_EN 1
#define LCD_RS 1
#define LCD_DATA_BITS 1

//function prootypes
void LCD_init( void );
void LCD_pulse_ENA( void );
void LCD_4b_command( uint8_t command );
void LCD_command( uint8_t command );
void LCD_write_char( uint8_t letter );

#endif /* INC_LCD_H_ */
