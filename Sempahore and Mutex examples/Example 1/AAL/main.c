/*
 * LCD and Keypad for RTOS.c
 *
 * Created: 08/02/2020 13:42:41
 * Author : Native_programmer
 */ 

#include "FreeRTOS.h"
#include "task.h"
#include "Atmega32Registers.h"
#include "BitManipulation.h"
#include "Data_Types.h"
#include "DIO.h"


#define Switch_Is_Set 1
#define C0            8
#define C1            9
#define R0            10
#define R1            11
#define LED           12 


#define Led_init(LED)                          DIO_Init_Pin(LED,1);
#define Led_Toggle(LED)                        DIO_toggle_Pin(LED);
#define Row_init(R)				               DIO_Init_Pin(R,0);
#define Col_init(C)				               DIO_Init_Pin(C,1);
#define toggle_col(C)                          DIO_toggle_Pin(C);
#define read_row(R,Copy_pu8_input_value)       DIO_Read_Pin(R,Copy_pu8_input_value);
#define set_col(C)                             DIO_Write_Pin(C,1);
#define Reset_col(C)                           DIO_Write_Pin(C,0);

/**Include Free RTOS*/
uinteg8_t gu8_key1=0;
uinteg8_t gu8_key2=0;
uinteg8_t gu8_key3=0;
uinteg8_t gu8_key4=0;
TaskHandle_t SYS_Handler=NULL;	
TaskHandle_t LCD_Handler=NULL;	
TaskHandle_t keypad_Handler=NULL;	
TaskHandle_t LED_Handler=NULL;	
/****************************LOCAL_MACROS**********************************/
#define LED_ON  1
#define LED_OFF 0

/*****************************GLOBAL_VAR*************************************/
static uinteg8_t gu8_LCD_Status=1;
static uinteg8_t gu8_KEYPAD_Status=NULL;

/***********************PROTOTYPE_IMPLEMENTATION*******************************/

void System_Init (void *Pv_Parameter)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;

	vTaskSuspend(keypad_Handler);
	vTaskSuspend(LCD_Handler);
	
	while(1)
	{
		gu8_LCD_Status=LCD_init();
		xLastWakeTime=xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		if(gu8_LCD_Status==NULL)
		{
		    Debug_By_Toggle(22);
		    /*Led init*/
		    Led_init(LED);
		    /*keypad init*/
		    Row_init(R0);
		    Row_init(R1);
		    Col_init(C0);
		    Col_init(C1);			
			vTaskResume(keypad_Handler);
			vTaskResume(LCD_Handler);			
			vTaskSuspend(SYS_Handler);


			




		}
	}
}

void Check_Keypad(void*Pv_Parameter)/**Delay by 25 milisecond  -- Priority is 2**/
{
	uinteg8_t value_of_keypad;


	while (1)
	{
		
        Debug_By_Toggle(23);
		set_col(C0);
		Reset_col(C1);
		read_row(R0,&value_of_keypad);
		
		if(value_of_keypad==Switch_Is_Set)
		{
		    Debug_By_Toggle(24);
			gu8_key1++;
		}
		else
		{
			read_row(R1,&value_of_keypad);
			if(value_of_keypad==Switch_Is_Set)
			{
				gu8_key3++;
			}
			
		}
		vTaskDelay(10/portTICK_PERIOD_MS);	
		
		
		Reset_col(C0);
		set_col(C1);
		read_row(R0,&value_of_keypad);
		if(value_of_keypad==Switch_Is_Set)
		{
			Debug_By_Toggle(24);
			gu8_key2++;
		}
		else
		{
			read_row(R1,&value_of_keypad);
			if(value_of_keypad==Switch_Is_Set)
			{
				gu8_key4++;
			}
			
		}
		vTaskDelay(10/portTICK_PERIOD_MS);						
	}
}

void LCD_DISP (void *Pv_Paramaeter)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;
				
	while(1)
	{
  	
		if(gu8_key1>=6)
		{
			Debug_By_Toggle(25);
		    LCD_displayCharacter('C');
			gu8_key1=0;	
			vTaskDelay(104/portTICK_PERIOD_MS);
		}
		else
		{
			gu8_key1=0;	
			vTaskDelay(120/portTICK_PERIOD_MS);
			/*Do nothing*/
		}	
	}
}
/*

void Led1_Toggle_On_Switch1_Task(void*Pv_Parameter)/ **Delay by 50 milisecond  -- Priority is 1** /
{
	
		while(1)
		{
			Debug_By_Toggle(25);
		}
/ *
	while (1)
	{
     
		if(gu8_key1==6)
		{
			Led_Toggle(LED);			
		}
		else
		{
		    / *Do nothing* /
		}

		gu8_key1=0;
		vTaskDelay(120/portTICK_PERIOD_MS);
	}   * /                                                                                                                   																																														
}
*/

int main(void)
{

    xTaskCreate(System_Init,NULL,600,NULL,2,&SYS_Handler);
    xTaskCreate(Check_Keypad,NULL,100,NULL,1,&keypad_Handler);
/*	xTaskCreate(Led1_Toggle_On_Switch1_Task,NULL,100,NULL,2,&LED_Handler);*/
    xTaskCreate(LCD_DISP,NULL,100,NULL,0,&LCD_Handler);	


	/*Os start  */
	vTaskStartScheduler();

}
