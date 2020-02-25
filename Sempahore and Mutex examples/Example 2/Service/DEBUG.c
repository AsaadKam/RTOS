/*
 * Debug.c
 *
 * Created: 25/01/2020 02:36:24
 *  Author: Native_programmer
 */ 

#include "DIO.h"

#define Debug_By_Toggle_State_1st_time    0
#define Debug_By_Toggle_State_Other_times 1
#define Debug_By_Toggle_State_Nothing     2


void Debug_By_Toggle(uinteg8_t u8_Pin)
{
	static uinteg8_t u8_Debug_By_Toggle_State=Debug_By_Toggle_State_1st_time;
	switch(u8_Debug_By_Toggle_State)
	{
		case Debug_By_Toggle_State_1st_time:
		{
			DIO_Init_Pin(u8_Pin,1);
			DIO_toggle_Pin(u8_Pin);	
			u8_Debug_By_Toggle_State=Debug_By_Toggle_State_Other_times;
		}
		break;
		case Debug_By_Toggle_State_Other_times:
		{
			DIO_toggle_Pin(u8_Pin);
		}
		break;
		case Debug_By_Toggle_State_Nothing:
		{
            /*Do Nothing*/
		}
		break;
	}

}