/*
 * Swictch on hold by RTOS
 *
 * Created: 12/02/2020 3:01:00
 * Author : Native_programmer
 */ 



/**Include Free RTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include "Atmega32Registers.h"
#include "BitManipulation.h"
#include "Data_Types.h"
#include "DIO.h"
	

/****************************LOCAL_MACROS**********************************/
#define LED_ON  1
#define LED_OFF 0
#define switch0       0
#define LED           1


#define Led_init(LED)                          DIO_Init_Pin(LED,LED_ON);
#define Led_Turn_Off(LED)                      DIO_Write_Pin(LED,LED_OFF);
#define Led_Toggle(LED)                        DIO_toggle_Pin(LED);
#define Switch_init(S)                         DIO_Init_Pin(S,HIGH);
#define Read_Switch(S,Copy_pu8_input_value)    DIO_Read_Pin(S,Copy_pu8_input_value);
/*****************************GLOBAL_VAR*************************************/
static uinteg8_t gu8_LCD_Status=1;
static uinteg8_t gu8_KEYPAD_Status=NULL;
	   uinteg8_t gu8_Switch0_Count=0;
	   TaskHandle_t SYS_Handler=NULL;
	   TaskHandle_t LCD_Handler=NULL;
	   TaskHandle_t keypad_Handler=NULL;
	   TaskHandle_t LED_Handler=NULL;
/***********************PROTOTYPE_IMPLEMENTATION*******************************/

void System_Init (void *Pv_Parameter)
{	
	while(1)
	{
		/*Debug by toggle led at pin 22 test init. happened*/	
		Debug_By_Toggle(22);
		/*Led init*/
		Led_init(LED);
		/*Switch init*/
		Switch_init(switch0);
		/*Suspend the task*/
		vTaskSuspend(SYS_Handler);	
	}
}

void Check_Switch(void*Pv_Parameter)/**Delay by 25 milisecond  -- Priority is 2**/
{
	/*local variable for reading the value of switch now*/
	uinteg8_t value_of_switch;

	while (1)
	{
		/*Debug by toggle led at pin 23 test init. happened*/		
        Debug_By_Toggle(23); 
		/*Read switch 0 */
		Read_Switch(switch0,&value_of_switch);
		/*if it is high increment the global variable */
		if(value_of_switch==HIGH)
		{
			gu8_Switch0_Count++;
		}
		else
		{
			/*Do nothing*/
		}
		/*Make the periodicity 50 milisecond*/				
		vTaskDelay(50/portTICK_PERIOD_MS);	
	}
}

void Led1_Toggle_On_Switch0_Task(void*Pv_Parameter)/**Delay by 50 milisecond  -- Priority is 1**/
{
	static uinteg8_t su8_Last_Switch0_State=LOW;
	static uinteg8_t su8_Switch0_HIGH_State_Count=0;
	static uinteg8_t su8_Led0_Periodicity_Toggle_Count=0;	
	       uinteg8_t u8_Current_Switch0_State=LOW;
	while (1)
	{
        /*Check whether switch is switched or it it is noise*/
		if(gu8_Switch0_Count==2)
		{
			/*Let state High*/
			u8_Current_Switch0_State=HIGH;			
		}
		else
		{
			/*Let state Low*/			
			u8_Current_Switch0_State=LOW;	
		}
		/**If the last state is low and current is high start count**/
		if((su8_Last_Switch0_State==LOW )&& (u8_Current_Switch0_State==HIGH ) )
		{
			su8_Switch0_HIGH_State_Count=1;
		}
		/**If the last state is high and current is high continue count**/		
		else if((su8_Last_Switch0_State==HIGH) && (u8_Current_Switch0_State==HIGH)  )
		{
			su8_Switch0_HIGH_State_Count++;
		}
		/**If the last state is high and current is low start or the last state is low 
		   and current is low see if the count of High state*/		
		else if(((su8_Last_Switch0_State==HIGH) && (u8_Current_Switch0_State==LOW))|| ((su8_Last_Switch0_State==LOW) && (u8_Current_Switch0_State==LOW)) )
		{
			/*If the count is less than 2second*/
			if(su8_Switch0_HIGH_State_Count<20)
			{
				/*Turn off the led*/
				Led_Turn_Off(LED);
			}
			
			/*If the count is equal or more than 2second and count less than or equal 40 */			
			else if((su8_Switch0_HIGH_State_Count>=20) &&(su8_Switch0_HIGH_State_Count<=40))
			{
				/**start count the counter of periodicity at which we will toggle*/
				su8_Led0_Periodicity_Toggle_Count++;
				/*Check the count of counter equivalent to 400 millisecond*/
				if(su8_Led0_Periodicity_Toggle_Count==4)
				{
					/*Toggle the Led**/
					Led_Toggle(LED);
					/*Make the  counter of periodicity equal zero*/
					su8_Led0_Periodicity_Toggle_Count=0;
				}
				else
				{
					/*Do nothing*/
				}				
			}	
			/*If the count is  more than 4second */		
			else if(su8_Switch0_HIGH_State_Count>40)
			{
				/**start count the counter of periodicity at which we will toggle*/				
				su8_Led0_Periodicity_Toggle_Count++;
				/*Check the count of counter equivalent to 100 millisecond*/				
				if(su8_Led0_Periodicity_Toggle_Count==1)
				{
					/*Toggle the Led**/					
					Led_Toggle(LED);
					/*Make the  counter of periodicity equal zero*/					
					su8_Led0_Periodicity_Toggle_Count=0;
				}		
				else
				{
					/*Do nothing*/
				}						
			}	
			else
			{
				/*Do nothing*/
			}			
		}
		/*Make the last switch0 state equal current switch0 state*/
        su8_Last_Switch0_State=u8_Current_Switch0_State;
		/*Make switch0 count equal zero*/		
		gu8_Switch0_Count=0;
		/*Make the periodicity 100milisecond*/			
		vTaskDelay(100/portTICK_PERIOD_MS);
	} 
	                                                                                                                   																																														
}

int main(void)
{
    /**Create the task of System_Init where it uses 600 word from stack and has priority 2*/
    xTaskCreate(System_Init,NULL,600,NULL,2,&SYS_Handler);
    /**Create the task of Check_Switch where it uses 100 word from stack and has priority 1*/
    xTaskCreate(Check_Switch,NULL,100,NULL,1,&keypad_Handler);
    /**Create the task of Led1_Toggle_On_Switch0_Task where it uses 100 word from stack and has priority 0*/	
	xTaskCreate(Led1_Toggle_On_Switch0_Task,NULL,100,NULL,0,&LED_Handler);

	/*Os start  */
	vTaskStartScheduler();

}
