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
#define switch0                                0
#define LED									   1
#define LCD_state_row_0                        0
#define LCD_state_row_1                        1
#define Switch_init(S)                         DIO_Init_Pin(LED,INPUT);
#define Led_init(LED)                          DIO_Init_Pin(LED,OUTPUT);
#define Led_Turn_Off(LED)                      DIO_Write_Pin(LED,LED_OFF);
#define Led_Toggle(LED)                        DIO_toggle_Pin(LED);
#define Read_Switch(S,Copy_pu8_input_value)    DIO_Read_Pin(S,Copy_pu8_input_value);
/*****************************GLOBAL_VAR*************************************/
SemaphoreHandle_t String_Row_1_Semaphore;
SemaphoreHandle_t String_Row_2_Semaphore;
SemaphoreHandle_t Switch_Couting_Semaphore;
SemaphoreHandle_t Switch_is_set_Semaphore;
SemaphoreHandle_t LCD_Init_Semaphore;
uinteg8_t Transmitted_Row_0_Array[16]="Hello LCD!!";
uinteg8_t Transmitted_Row_1_Array[16]="overwritten!!";
uinteg8_t LCD_Row_0_Array[16]={0};
uinteg8_t LCD_Row_1_Array[16]={0};		
TaskHandle_t String_transmiting_Handler=NULL;
TaskHandle_t SYS_Handler=NULL;
TaskHandle_t Switch_Handler=NULL;
TaskHandle_t LCD_Handler=NULL;

/***********************PROTOTYPE_IMPLEMENTATION*******************************/
void System_Init (void *Pv_Parameter)            /**     No  Periodicity         -- Priority is 3**/
{
    uinteg8_t gu8_LCD_Status=0;

	vTaskSuspend(LCD_Handler);
	vTaskSuspend(String_transmiting_Handler);
	vTaskSuspend(Switch_Handler);
		
	while(1)
	{
	    LCD_init();
		vTaskResume(LCD_Handler);
		vTaskResume(String_transmiting_Handler);
		vTaskResume(Switch_Handler);		
		vTaskSuspend(SYS_Handler);
	}
}

void Check_Switch(void*Pv_Parameter)             /** Periodicity 50 milisecond   -- Priority is 2**/
{
    Debug_By_Toggle(23);		

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;
    Switch_Couting_Semaphore=xSemaphoreCreateCounting(4,0);		
	/*local variable for reading the value of switch now*/
	uinteg8_t u8_value_of_switch;
	uinteg8_t u8_switch_high_count;	

	
	while(1)
	{

		xLastWakeTime=xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, xFrequency );	
		/*Read switch 0 */
		Read_Switch(switch0,&u8_value_of_switch);
		/*if it is high increment the global variable */
		if(u8_value_of_switch==HIGH)
		{
			if(uxSemaphoreGetCount(Switch_Couting_Semaphore)<4)
			{
				/*Increment counting semaphore*/
				xSemaphoreGive(Switch_Couting_Semaphore);
			}
			Debug_By_Toggle(23);
		}
		else
		{
			/*Do nothing*/
		}
	}
}

void String_transmiting_Task(void *Pv_Parameter) /** Periodicity 400 milisecond  -- Priority is 1**/
{
    Debug_By_Toggle(24);	

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 400;
    vSemaphoreCreateBinary(String_Row_1_Semaphore);	
    vSemaphoreCreateBinary(String_Row_2_Semaphore);	
    uinteg8_t u8_LCD_ROW_0_index=0,u8_LCD_ROW_1_index=0;
	
	while(1)
	{

		xLastWakeTime=xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, xFrequency );		
		xSemaphoreTake( String_Row_1_Semaphore, ( TickType_t ) 0 );
		/*Transferring from one array to another array*/
        u8_LCD_ROW_0_index=0;		
        while(Transmitted_Row_0_Array[u8_LCD_ROW_0_index])
		{
			LCD_Row_0_Array[u8_LCD_ROW_0_index]=Transmitted_Row_0_Array[u8_LCD_ROW_0_index];
			u8_LCD_ROW_0_index++;
		}

		xSemaphoreGive(String_Row_1_Semaphore);
				
		xSemaphoreTake( String_Row_2_Semaphore, ( TickType_t ) 0 );	
		/*Transferring from one array to another array*/		
		u8_LCD_ROW_1_index=0;			
        while(Transmitted_Row_1_Array[u8_LCD_ROW_1_index])
        {
	        LCD_Row_1_Array[u8_LCD_ROW_1_index]=Transmitted_Row_1_Array[u8_LCD_ROW_1_index];
	        u8_LCD_ROW_1_index++;
        }
		 
		xSemaphoreGive(String_Row_2_Semaphore);			
	}
}

void LCD_DISP(void *Pv_Paramaeter)               /** Periodicity 200 milisecond  -- Priority is 0**/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;		
	static uinteg8_t su8_Flag=0;	
	static uinteg8_t su8_Write_New_String_Flag=1;
	       uinteg8_t  u8_Switch_Sempahore_Count_Buffer=0; 
		    
	while(1)
	{
		xLastWakeTime=xTaskGetTickCount();
		/*Get count of semaphore*/
		u8_Switch_Sempahore_Count_Buffer=uxSemaphoreGetCount(Switch_Couting_Semaphore);
		/*Decrement counting semaphore*/
		while(uxSemaphoreGetCount(Switch_Couting_Semaphore)!=0)
		{
			xSemaphoreTake( Switch_Couting_Semaphore, ( TickType_t ) 0 );
		}
		
	    if(u8_Switch_Sempahore_Count_Buffer>=2)
	    {
		    if(su8_Write_New_String_Flag==1)
		    {
			    su8_Write_New_String_Flag=0;
		        /**Protection with semaphore*/				
				xSemaphoreTake(String_Row_2_Semaphore, ( TickType_t ) 0 );
				
			    LCD_displayStringRowColumn(2,0,LCD_Row_1_Array);
				
			    xSemaphoreGive(String_Row_2_Semaphore);
		    }
	    }
	    else
	    {
		    /**Here i am going to clear,then i am going to write again**/
		    su8_Write_New_String_Flag=1;
		    
		    xSemaphoreTake(String_Row_2_Semaphore, ( TickType_t ) 0 );
		    
		    LCD_displayStringRowColumn(2,0,"               ");
		    
		    xSemaphoreGive(String_Row_2_Semaphore);
	    }

		/**Dispatch row 2 that is related to switch state*/
		if(su8_Flag==0)
		{
			/**Protection with semaphore*/
			xSemaphoreTake(String_Row_1_Semaphore, ( TickType_t ) 0 );
			
			LCD_displayStringRowColumn(1,0,LCD_Row_0_Array);
		
			xSemaphoreGive(String_Row_1_Semaphore);  
			su8_Flag=1;	
		}
		else if(su8_Flag)
		{
			/**Protection with semaphore*/			
			xSemaphoreTake(String_Row_1_Semaphore, ( TickType_t ) 0 );
						
			/**Here i am going to clear,then i am going to write again**/			
			LCD_displayStringRowColumn(1,0,"               ");
			
			xSemaphoreGive(String_Row_1_Semaphore); 
			su8_Flag=0; 			
		}
        else
		{
			/*Do Nothing*/
		}		
		vTaskDelayUntil( &xLastWakeTime, xFrequency );					
	}
}

int main(void)
{
    /**Create the task of System_Init where it uses 100 word from stack and has priority 3*/
    xTaskCreate(System_Init,NULL,100,NULL,1,&SYS_Handler);
    /**Create the task of Check_Switch where it uses 100 word from stack and has priority 2*/
    xTaskCreate(Check_Switch,NULL,100,NULL,2,&Switch_Handler);
    /**Create the task of String_transmiting_Task where it uses 100 word from stack and has priority 1*/
    xTaskCreate(String_transmiting_Task,NULL,100,NULL,1,&String_transmiting_Handler);   	
	/*Create the task of LCD_DISP where it uses 100 word from stack and has priority 0*/	
	xTaskCreate(LCD_DISP,NULL,100,NULL,0,&LCD_Handler);

	/*Os start  */
	vTaskStartScheduler();
}
