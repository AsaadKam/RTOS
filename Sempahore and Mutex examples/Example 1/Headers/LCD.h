/*
 * LCD_cfg.h
 *
 *  Created on: Jan 3, 2020
 *      Author: H
 */

#ifndef MCAL_LCD_LCD_H_
#define MCAL_LCD_LCD_H_


#include "Data_Types.h"
#include "Atmega32Registers.h"
#include "BitManipulation.h"
#include "DIO.h"
#include"lcd_ret_val.h"

/***********************INCLUDES***************************************/
typedef uinteg8_t ERROR_TYPE_t; 
#define NULL ((uinteg8_t*)(0))
/****************************TYPE_DEF*****************************************/

//typedef uinteg8_t ERROR_TYPE_t;

/*********************************Config_MACROS************************************/
/* LCD HW Pins */
#define RS 1
#define RW 2
#define E  3
#define LCD_CTRL_PORT PORTA
#define LCD_CTRL_PORT_DIR DDRA
#define LCD_DATA_PORT PORTA
#define LCD_DATA_PORT_DIR DDRA
/* LCD Commands */
#define CLEAR_COMMAND 0x01
#define FOUR_BITS_DATA_MODE 0x02
#define TWO_LINE_LCD_Four_BIT_MODE 0x28
#define TWO_LINE_LCD_Eight_BIT_MODE 0x38
#define CURSOR_OFF 0x0C
#define CURSOR_ON 0x0E
#define SET_CURSOR_LOCATION 0x80

/* LCD Data bits mode configuration */
#define DATA_BITS_MODE 4

/* Use higher 4 bits in the data port */
#define UPPER_PORT_PINS


/***************************FUNCTION_PROTOTYPES*****************************/
ERROR_TYPE_t LCD_init(void);
ERROR_TYPE_t LCD_sendCommand(uinteg8_t u8_command);
ERROR_TYPE_t LCD_displayCharacter(uinteg8_t u8_data);
ERROR_TYPE_t LCD_displayString(const uinteg8_t * ptr_String,uinteg8_t * u8_dummy);
void LCD_clearScreen(void);
void LCD_displayStringRowColumn(uinteg8_t row,uinteg8_t col,const uinteg8_t *Str, uinteg8_t * u8_flag);
void LCD_goToRowColumn(uinteg8_t u8_row,uinteg8_t u8_col);

#endif /* MCAL_LCD_LCD_H_ */
