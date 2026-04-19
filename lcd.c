/*
 * lcd.c
 *
 *  Created on: Apr 15, 2026
 *      Author: alexm
 */

#include "lcd.h"
#include "delay.h"
#include "keypad.h"

void LED_Config(void){

	LED_PORT -> MODER &= ~(GPIO_MODER_MODE7);
	LED_PORT -> MODER |= (GPIO_MODER_MODE7_0);
	LED_PORT -> OTYPER &= ~(GPIO_OTYPER_OT7);
	LED_PORT -> PUPDR &= ~(GPIO_PUPDR_PUPD7);
	LED_PORT -> OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED7_Pos);
	LED_PORT -> BRR |= (GPIO_PIN_7);

}

void LCD_init( void )  {   // RCC & GPIO config removed - leverage A1, A2 code
	SysTick_Init(); //initialize SysTick timer
	//initialize clk for GPIOC
	RCC -> AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);

	// PC0-7 as general purpose push-pull outputs, no pull, high speed
	LCD_PORT -> MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
	                           GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);

	LCD_PORT -> MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
	                           GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);

	LCD_PORT -> OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 |
	                           GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

	LCD_PORT -> PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
	                           GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);

	LCD_PORT -> OSPEEDR |=  (3 << GPIO_OSPEEDR_OSPEED0_Pos) | (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
	                          (3 << GPIO_OSPEEDR_OSPEED2_Pos) | (3 << GPIO_OSPEEDR_OSPEED3_Pos) |
	                          (3 << GPIO_OSPEEDR_OSPEED4_Pos) | (3 << GPIO_OSPEEDR_OSPEED5_Pos) |
	                          (3 << GPIO_OSPEEDR_OSPEED6_Pos) | (3 << GPIO_OSPEEDR_OSPEED7_Pos);

	LCD_PORT -> BRR     |=  (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
	                           GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	//LCD_PORT -> ODR |= LCD_EN;
   delay_us( 40000 );                     // power-up wait 40 ms
   for ( int idx = 0; idx < 3; idx++ ) {  // wake up 1,2,3: DATA = 0011 XXXX
      LCD_4b_command( 0x30 );             // HI 4b of 8b cmd, low nibble = X
      delay_us( 200 );
   }

   //full LCD initialization
   LCD_4b_command( 0x20 ); // fcn set #4: 4b cmd set 4b mode - next 0x28:2-line
   delay_us( 40 );
   LCD_command( 0x28 ); // fcn set: 8b cmd set 2 - line
   delay_us( 40 );
   LCD_command( 0x0C ); // display ON/OFF cntrl: 8b cmd set display ON
   delay_us( 40 );
   LCD_command( 0x06 ); //entry mode set: 8b cmd set auto increment
   delay_us( 40 );         // remainder of LCD init removed - see LCD datasheets

   LCD_command( 0xF );
   delay_us( 2000 );

}

void LCD_pulse_ENA( void )  {
// ENAble line sends command on falling edge
// set to restore default then clear to trigger
   LCD_PORT->ODR   |= ( LCD_EN );         	// ENABLE = HI
   delay_us( 5 );                         // TDDR > 320 ns
   LCD_PORT->ODR   &= ~( LCD_EN );        // ENABLE = LOW
   delay_us( 5 );                         // low values flakey, see A3:p.1
}

void LCD_4b_command( uint8_t command )  {
// LCD command using high nibble only - used for 'wake-up' 0x30 commands
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS ); 	// clear DATA bits
   LCD_PORT->ODR   |= ( command >> 4 );   // DATA = command
   delay_us( 5 );
   LCD_pulse_ENA( );
}

void LCD_command( uint8_t command )  {
// send command to LCD in 4-bit instruction mode
// HIGH nibble then LOW nibble, timing sensitive
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( (command>>4) & LCD_DATA_BITS ); // HIGH shifted low
   delay_us( 5 );
   LCD_pulse_ENA( );                                    // latch HIGH NIBBLE

   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( command & LCD_DATA_BITS );      // LOW nibble
   delay_us( 5 );
   LCD_pulse_ENA( );                                    // latch LOW NIBBLE
}

void LCD_write_char( uint8_t letter )  {
// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
   LCD_PORT->ODR   |= (LCD_RS);       // RS = HI for data to address
   delay_us( 5 );
   LCD_command( letter );             // character to print
   LCD_PORT->ODR   &= ~(LCD_RS);      // RS = LO
}

void LCD_write_string( char string[] ) {

	for( uint8_t idx = 0; idx < strlen(string); idx++ ){
		LCD_write_char(string[idx]);
		delay_us( 5 );
	}

}

void LCD_set_cursor( uint8_t row, uint8_t column) {

	uint8_t addr = (row == 1) ? ( 0x40 + column ) : column;
	LCD_command( 0x80 | addr );

}

void LCD_write_intro_message( void ) {
	LCD_set_cursor(0, 0);
	LCD_write_string("EE 329 A3 TIMER ");
	LCD_set_cursor(1,0);
	LCD_write_string("*=SET #=GO 00:00");
	LCD_set_cursor(0,0);
}

void Prompt_user_4_numbers( void ){
	LCD_set_cursor(0, 0);
	LCD_write_string("ENTER 4 DIGITS: ");
	LCD_set_cursor(1,0);
	LCD_write_string("MM:SS  ->  XX:XX");
	LCD_set_cursor(1,11);
}

void Update_entered_numbers( uint8_t nums[4] , uint8_t currentCnt ){
	LCD_set_cursor(1,11);
	delay_us(50);
	switch(currentCnt){

		case 1: 	LCD_write_char(nums[currentCnt-1]); break;
		case 2:  LCD_set_cursor(1,12);
					LCD_write_char(nums[currentCnt-1]); break;
		case 3:  LCD_set_cursor(1,14);
					LCD_write_char(nums[currentCnt-1]); break;
		case 4:  LCD_set_cursor(1,15);
					LCD_write_char(nums[currentCnt-1]); break;
		default: break;
	}
}

void Wait_for_4_User_Digits(uint8_t lcd_In[4]){

   uint8_t press_Cnt = 0; //to track number of user inputs
	memset( lcd_In, 0, sizeof(lcd_In) ); //clear input array

	while(  press_Cnt < 4 ) { //to store 4 inputs

		lcd_In[ press_Cnt ] = Return_ValidKeyPressLCD();

		if( lcd_In[ press_Cnt] == '*' ){
			//reset digit array and press_Cnt
			press_Cnt = 0;
			memset( lcd_In, 0, sizeof(lcd_In) );
			Prompt_user_4_numbers();
			continue;

		}

		else if ( lcd_In[press_Cnt] == '#' ) continue; // ignore or handle GO press during input

		else {
			//call function to update LCD with each button press
			press_Cnt++;
			delay_us(1000);
			Update_entered_numbers( lcd_In,  press_Cnt);

			if( press_Cnt == 2){
				LCD_set_cursor(1,14);
			}
			while( Keypad_IsAnyKeyPressed() != 0 ){} //wait until button released to continue loop;
		}

	}
	LCD_set_cursor(0,0);
	LCD_write_string("# -> BEGIN      ");
	LCD_set_cursor(1,0);
	LCD_write_string("* -> RESET");
	LCD_set_cursor(1,16);

}




