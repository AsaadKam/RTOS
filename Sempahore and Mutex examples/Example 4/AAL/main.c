/*
 * Hello LCD !!!
 *
 * Created: 12/02/2020 3:01:00
 * Author : Native_programmer
 */ 



/**Include Free RTOS*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "Atmega32Registers.h"
#include "BitManipulation.h"
#include "Data_Types.h"
#include "DIO.h"
	

/****************************LOCAL_MACROS**********************************/
#define INPUT                                  0
#define OUTPUT                                 1
#define LED_ON                                 1
#define LED_OFF                                0
#define switch0                                8
#define switch1                                9
#define LED0								   10
#define LED1								   11
#define LCD_state_row_0                        0
#define LCD_state_row_1                        1
#define Switch_init(S)                         DIO_Init_Pin(S,INPUT);
#define Led_init(LED)                          DIO_Init_Pin(LED,OUTPUT);
#define Led_Turn_Off(LED)                      DIO_Write_Pin(LED,LED_OFF);
#define Led_Turn_On(LED)                       DIO_Write_Pin(LED,LED_ON);
#define Led_Toggle(LED)                        DIO_toggle_Pin(LED);
#define Read_Switch(S,Copy_pu8_input_value)    DIO_Read_Pin(S,Copy_pu8_input_value);
/*****************************GLOBAL_VAR*************************************/

SemaphoreHandle_t Switch0_Couting_Semaphore;
SemaphoreHandle_t Switch1_Couting_Semaphore;
SemaphoreHandle_t Switch0_is_set_Semaphore;
SemaphoreHandle_t Switch1_is_set_Semaphore;
SemaphoreHandle_t Led_On_Off_Semaphore;
uinteg8_t Switch1_ON_Array[16]=  "S0 O S1 C L ON ";
uinteg8_t Switch0_ON_Array[16]=  "S0 C S1 O L ON ";	
uinteg8_t Switches_OFF_Array[16]="S0 O S1 O L OFF";
TaskHandle_t Led_On_Off_Handler=NULL;
TaskHandle_t SYS_Handler=NULL;
TaskHandle_t Switch0_Handler=NULL;
TaskHandle_t Switch1_Handler=NULL;
TaskHandle_t Monitoring_Handler=NULL;
TickType_t xMutualLastWakeTime;

                         /***********************PROTOTYPE_IMPLEMENTATION*******************************/
						 
void System_Init (void *Pv_Parameter)             /**     No  Periodicity         -- Priority is 4**//**Execution time range micro    **/
{
    uinteg8_t gu8_LCD_Status=0;

	vTaskSuspend(Monitoring_Handler);
	vTaskSuspend(Led_On_Off_Handler);
	vTaskSuspend(Switch0_Handler);
	vTaskSuspend(Switch1_Handler);	
		
	while(1)
	{
	    LCD_init();
		Switch_init(switch0);
		Switch_init(switch1);		
		Led_init(LED0);
		Led_init(LED1);		
		vTaskResume(Monitoring_Handler);
		vTaskResume(Led_On_Off_Handler);
		vTaskResume(Switch0_Handler);		
		vTaskResume(Switch1_Handler);			
		vTaskSuspend(SYS_Handler);
	}
}

void Check_Switch0(void*Pv_Parameter)             /** Periodicity 50 milisecond   -- Priority is 3**//**Execution time range micro    **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;
    Switch0_Couting_Semaphore=xSemaphoreCreateCounting(2,0);		
	/*local variable for reading the value of switch now*/
	uinteg8_t u8_value_of_switch;
	uinteg8_t u8_switch_high_count;	

	
	while(1)
	{
		
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(22);
		/*Read switch 0 */
		Read_Switch(switch0,&u8_value_of_switch);
		/*if it is high increment the global variable */
		if(u8_value_of_switch==HIGH)
		{
			if(uxSemaphoreGetCount(Switch0_Couting_Semaphore)<2)
			{

				/*Increment counting semaphore*/
				xSemaphoreGive(Switch0_Couting_Semaphore);
			}

		}
		else
		{
			/*Do nothing*/
		}
		Debug_By_Toggle(22);		
		vTaskDelayUntil( &xLastWakeTime, xFrequency );			
	}
}

void Check_Switch1(void*Pv_Parameter)             /** Periodicity 50 milisecond   -- Priority is 3**//**Execution time range micro    **/
{

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;
	Switch1_Couting_Semaphore=xSemaphoreCreateCounting(2,0);
	/*local variable for reading the value of switch now*/
	uinteg8_t u8_value_of_switch;
	uinteg8_t u8_switch_high_count;

	
	while(1)
	{

		xLastWakeTime=xTaskGetTickCount();
	    Debug_By_Toggle(23);
		/*Read switch 0 */
		Read_Switch(switch1,&u8_value_of_switch);
		/*if it is high increment the global variable */
		if(u8_value_of_switch==HIGH)
		{
			if(uxSemaphoreGetCount(Switch1_Couting_Semaphore)<2)
			{

				/*Increment counting semaphore*/
				xSemaphoreGive(Switch1_Couting_Semaphore);
			}
		}
		else
		{
			/*Do nothing*/
		}
	    Debug_By_Toggle(23);		
		vTaskDelayUntil( &xLastWakeTime, xFrequency );		
	}
}

void Monitoring_Task(void *Pv_Paramaeter)         /** Periodicity 200 milisecond  -- Priority is 2**//**Execution time 104 milisecond **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;
	static uinteg8_t su8_Switch0_state_Flag=0;
	static uinteg8_t su8_Switch1_state_Flag=0;
	static uinteg8_t su8_Write_New_String_Flag=1;
	uinteg8_t  u8_Switch0_Sempahore_Count_Buffer=0;
	uinteg8_t  u8_Switch1_Sempahore_Count_Buffer=0;
	
	vSemaphoreCreateBinary(Switch0_is_set_Semaphore);
	vSemaphoreCreateBinary(Switch1_is_set_Semaphore);
	
	while(1)
	{
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(30);
		u8_Switch0_Sempahore_Count_Buffer=uxSemaphoreGetCount(Switch0_Couting_Semaphore);
		u8_Switch1_Sempahore_Count_Buffer=uxSemaphoreGetCount(Switch1_Couting_Semaphore);
		
		
		while(uxSemaphoreGetCount(Switch0_Couting_Semaphore)!=0)
		{
			xSemaphoreTake(Switch0_Couting_Semaphore,0);
		}
		while(uxSemaphoreGetCount(Switch1_Couting_Semaphore)!=0)
		{
			xSemaphoreTake(Switch1_Couting_Semaphore,0);
		}
		
		if((u8_Switch0_Sempahore_Count_Buffer>=2)&&(u8_Switch1_Sempahore_Count_Buffer<2))
		{
			xSemaphoreTake(Switch0_is_set_Semaphore,0);
			LCD_displayStringRowColumn(1,1,Switch0_ON_Array);
		}
		else if((u8_Switch0_Sempahore_Count_Buffer<2)&&(u8_Switch1_Sempahore_Count_Buffer>=2))
		{
			xSemaphoreTake(Switch1_is_set_Semaphore,0);
			LCD_displayStringRowColumn(1,1,Switch1_ON_Array);
		}		
		else
		{
			LCD_displayStringRowColumn(1,1,Switches_OFF_Array);
		}
		Debug_By_Toggle(30);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
 
void Led_On_Off_Task(void *Pv_Parameter)          /** Periodicity 200 milisecond  -- Priority is 1**//**Execution time range micro    **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;			vSemaphoreCreateBinary(Led_On_Off_Semaphore);
    uinteg8_t u8_LCD_ROW_0_index=0,u8_LCD_ROW_1_index=0;
	while(1)
	{
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(24);
		if(uxSemaphoreGetCount(Switch0_is_set_Semaphore)==0)
		{
			Led_Turn_On(LED0);
			xSemaphoreGive(Switch0_is_set_Semaphore);
		}
		else if(uxSemaphoreGetCount(Switch1_is_set_Semaphore)==0)
		{
			Led_Turn_On(LED0);
			xSemaphoreGive(Switch1_is_set_Semaphore);
		}
		else
		{
			Led_Turn_Off(LED0);			
		}   
		Debug_By_Toggle(24);		
		vTaskDelayUntil( &xLastWakeTime, xFrequency );				
	}
}


int main(void)
{
	/**Create the task of System_Init where it uses 100 word from stack and has priority 4*/
	xTaskCreate(System_Init,NULL,100,NULL,4,&SYS_Handler);
	/**Create the task of Check_Switch0 where it uses 100 word from stack and has priority 3*/
	xTaskCreate(Check_Switch0,NULL,100,NULL,3,&Switch0_Handler);
	/**Create the task of Check_Switch0 where it uses 100 word from stack and has priority 3*/
	xTaskCreate(Check_Switch1,NULL,100,NULL,3,&Switch1_Handler);	
	/*Create the task of Monitoring_Task where it uses 100 word from stack and has priority 1*/
	xTaskCreate(Monitoring_Task,NULL,100,NULL,2,&Monitoring_Handler);	
	/**Create the task of Led_On_Off_Task where it uses 100 word from stack and has priority 2*/
	xTaskCreate(Led_On_Off_Task,NULL,100,NULL,1,&Led_On_Off_Handler);   	

	/*Os start  */
	vTaskStartScheduler();
}
