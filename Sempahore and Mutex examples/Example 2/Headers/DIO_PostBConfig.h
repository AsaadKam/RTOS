/*
 * Config.h
 *
 * Created: 02/11/2019 00:36:52
 *  Author: Native_programmer
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_
/* _________________Includes______________________________________________*/
#include "Data_Types.h"
/* _________________DIO Data type for configuration_______________________*/
typedef struct
{
	uinteg8_t Pin_Number;
	uinteg8_t Pin_Direction;	
}gstrDioConfig_t;
/* _________________DIO PINS_______________________*/

#define LED0          0
#define LED1          1
#define LED2          2
#define LED3          3
#define LED4          4
#define LED5          5
#define LED6          6
#define LED7          7
#define LED8          8
#define LED9          9
#define LED10         10//INT2
#define LED11         11
#define LED12         12
#define LED13         13
#define LED14         14
#define LED15         15
#define LED16         16
#define LED17         17
#define LED18         18
#define LED19         19
#define LED20         20
#define LED21         21
#define LED22         22
#define LED23         23
#define LED24         24
#define LED25         25
#define LED26         26//INT0
#define LED27         27//INT1
#define LED28         28
#define LED29         29
#define LED30         30
#define LED31         31
/*______________________Externs________________*/
extern gstrDioConfig_t  str_LED0_DioConfig;
/*______________________Defines________________*/
#define INPUT  0
#define OUTPUT 1

#endif /* CONFIG_H_ */