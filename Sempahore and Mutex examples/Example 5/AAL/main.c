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
#include "String_Manipulation.h"
	

/****************************LOCAL_MACROS**********************************/
#define Display_Congratulation                 0
#define Clear_Display                          1
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


SemaphoreHandle_t Switch0_is_set_Semaphore;
SemaphoreHandle_t Switch1_is_set_Semaphore;
SemaphoreHandle_t Switch0_Couting_Semaphore;
SemaphoreHandle_t Switch1_Couting_Semaphore;
SemaphoreHandle_t Win_Semaphore;
uinteg8_t Congratulation_player1_Array[16]= "    Player 1";
uinteg8_t Congratulation_player2_Array[16]= "    Player 2";	
uinteg8_t Equal_players_Array[16]="Players are even";
uinteg8_t General_Array[16]={0};
	
TaskHandle_t SYS_Handler=NULL;
TaskHandle_t Switches_Handler=NULL;
TaskHandle_t Lcd_Display_Count_Handler=NULL;
TaskHandle_t Lcd_Display_Congratulation_Handler=NULL;


/***********************PROTOTYPE_IMPLEMENTATION*******************************/
void System_Init (void *Pv_Parameter)                        /**     No  Periodicity         -- Priority is 4**//**Execution time range micro    **/
{
    uinteg8_t gu8_LCD_Status=0;
	
	vTaskSuspend(Switches_Handler);
	vTaskSuspend(Lcd_Display_Count_Handler);
	vTaskSuspend(Lcd_Display_Congratulation_Handler);
		
	while(1)
	{
	    LCD_init();
		Switch_init(switch0);
		Switch_init(switch1);	
		vTaskResume(Switches_Handler);
		vTaskResume(Lcd_Display_Count_Handler);	
		vTaskSuspend(SYS_Handler);
	}
}

void Check_Switches(void*Pv_Parameter)                      /** Periodicity 50 milisecond   -- Priority is 3**//**Execution time range micro    **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;

	/*local variable for reading the value of switch now*/
	uinteg8_t u8_value_of_switch;

	uinteg8_t u8_switch_high_count;
	Switch0_Couting_Semaphore=xSemaphoreCreateCounting(3,0);
	Switch1_Couting_Semaphore=xSemaphoreCreateCounting(3,0);
	while(1)
	{
		
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(22);
		/*Read switch 0 */
		Read_Switch(switch0,&u8_value_of_switch);
		/*if it is high increment the global variable */

		if(u8_value_of_switch==HIGH)
		{

			if(uxSemaphoreGetCount(Switch0_Couting_Semaphore)<3)
			{
				/*Increment counting semaphore*/
				xSemaphoreGive(Switch0_Couting_Semaphore);
			}

		}
		else
		{
			/*Do nothing*/
		}
		/*Read switch 1*/
		Read_Switch(switch1,&u8_value_of_switch);
		/*if it is high increment the global variable */
		if(u8_value_of_switch==HIGH)
		{
			if(uxSemaphoreGetCount(Switch1_Couting_Semaphore)<3)
			{
				/*Increment counting semaphore*/
				xSemaphoreGive(Switch1_Couting_Semaphore);
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

void Lcd_Display_Count(void *Pv_Parameter)                   /** Periodicity 150 milisecond  -- Priority is 2**//**Execution time range micro    **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 150;

	uinteg8_t u8_Switch0_Sempahore_Count_Buffer;
	uinteg8_t u8_Switch1_Sempahore_Count_Buffer;
	static integ8_t ss8_player1_count=0;
	static integ8_t ss8_player2_count=0;
	static uinteg8_t su8_Latched_State_Of_Switch0=LOW;	
	static uinteg8_t su8_Latched_State_Of_Switch1=LOW;
	uinteg8_t u8_index_array=0;
	
	while(1)
	{
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(23);
		
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
		
		if((u8_Switch0_Sempahore_Count_Buffer>=2)&&(u8_Switch1_Sempahore_Count_Buffer>=2))
		{
           if((su8_Latched_State_Of_Switch0==LOW)&&(su8_Latched_State_Of_Switch1==LOW))
		   {
			   ss8_player1_count++;
			   ss8_player2_count++;	

		   }
           else if((su8_Latched_State_Of_Switch0==HIGH)&&(su8_Latched_State_Of_Switch1==LOW))
           {
			   if(ss8_player1_count>0)
			   {
		           ss8_player1_count--;				   
			   }
	           ss8_player2_count++;
           }
           else if((su8_Latched_State_Of_Switch0==LOW)&&(su8_Latched_State_Of_Switch1==HIGH))
           {
	           ss8_player1_count++;
			   if(ss8_player2_count>0)
			   {
				   ss8_player2_count--;
			   }
           }		
		   su8_Latched_State_Of_Switch0=HIGH;
		   su8_Latched_State_Of_Switch1=HIGH;   		   
		}
		else if((u8_Switch0_Sempahore_Count_Buffer>=2)&&(!(u8_Switch1_Sempahore_Count_Buffer>=2)))
		{
           if((su8_Latched_State_Of_Switch0==LOW))
           {
	           ss8_player1_count++;
			   if(ss8_player2_count>0)
			   {
				   ss8_player2_count--;
			   }
           }
			su8_Latched_State_Of_Switch0=HIGH;
			su8_Latched_State_Of_Switch1=LOW;
		}
		else if((!(u8_Switch0_Sempahore_Count_Buffer>=2))&&(u8_Switch1_Sempahore_Count_Buffer>=2))
		{
           if((su8_Latched_State_Of_Switch1==LOW))
           {
			   if(ss8_player1_count>0)
			   {
				   ss8_player1_count--;
			   }
	           ss8_player2_count++;
           }
           /*Here is the problem*/
		   	su8_Latched_State_Of_Switch0=LOW;
		   	su8_Latched_State_Of_Switch1=HIGH;
		}
		else if((!(u8_Switch0_Sempahore_Count_Buffer>=2))&&(!(u8_Switch1_Sempahore_Count_Buffer>=2)))
		{
	        su8_Latched_State_Of_Switch0=LOW;		
	        su8_Latched_State_Of_Switch1=LOW;			   	
		}

		
		LCD_displayStringRowColumn(1,1,"Player1=");
		LCD_integerToString(ss8_player1_count,1,9);
		LCD_displayString(" ");
		LCD_displayStringRowColumn(2,1,"Player2=");		
		LCD_integerToString(ss8_player2_count,2,9);
		LCD_displayString(" ");		
		
		if((ss8_player1_count==50)&&(ss8_player2_count==50))
		{
			/*Display string of we are equal*/
			ss8_player1_count=0;
			ss8_player2_count=0;
			LCD_clearScreen();
            LCD_displayStringRowColumn(1,1,Equal_players_Array);
			vTaskResume(Lcd_Display_Congratulation_Handler);	
			vTaskSuspend(Switches_Handler);
			vTaskSuspend(Lcd_Display_Count_Handler);
		}
		else if((ss8_player1_count<50)&&(ss8_player2_count==50))
		{
			ss8_player1_count=0;
			ss8_player2_count=0;
			LCD_clearScreen();			
            LCD_displayStringRowColumn(1,1,Congratulation_player2_Array);
			vTaskResume(Lcd_Display_Congratulation_Handler);
			vTaskSuspend(Switches_Handler);
			vTaskSuspend(Lcd_Display_Count_Handler);
		}
		else if((ss8_player1_count==50)&&(ss8_player2_count<50))
		{
			ss8_player1_count=0;
			ss8_player2_count=0;
			LCD_clearScreen();			
            LCD_displayStringRowColumn(1,1,Congratulation_player1_Array);
			vTaskResume(Lcd_Display_Congratulation_Handler);
			vTaskSuspend(Switches_Handler);
			vTaskSuspend(Lcd_Display_Count_Handler);		
		}
		else
		{
			/*Do nothing*/
		}		
    
		Debug_By_Toggle(23);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void Lcd_Display_Congratulation(void *Pv_Paramaeter)         /** Periodicity 200 milisecond  -- Priority is 1**//**Execution time 104 milisecond **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;	
	static uinteg8_t su8_Cogratulation_state=Display_Congratulation;
	static uinteg8_t su8_Lcd_Display_Congratulation_Count=0;
    
	while(1)
	{
		
		xLastWakeTime=xTaskGetTickCount();		
		Debug_By_Toggle(24);    

        su8_Lcd_Display_Congratulation_Count++;		

		if(su8_Lcd_Display_Congratulation_Count<25)	
		{	
			switch(su8_Cogratulation_state)
			{
				case Display_Congratulation :
				{

					LCD_displayStringRowColumn(2,1," Congratulation!!");					
					su8_Cogratulation_state=Clear_Display;
				}
				break;
				case Clear_Display:
				{
					LCD_displayStringRowColumn(2,1,"               ");
					su8_Cogratulation_state=Display_Congratulation;				
				}
				break;			
			}
		}
		else
		{
			LCD_clearScreen();				
			su8_Lcd_Display_Congratulation_Count=0;
			vTaskResume(Switches_Handler);	
			vTaskResume(Lcd_Display_Count_Handler);			
			vTaskSuspend(Lcd_Display_Congratulation_Handler);
		}
		Debug_By_Toggle(24);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );			
	}
}

int main(void)
{
    /**Create the task of System_Init where it uses 100 word from stack and has priority 4*/
    xTaskCreate(System_Init,NULL,100,NULL,4,&SYS_Handler);		
    /**Create the task of Check_Switches where it uses 100 word from stack and has priority 3*/
    xTaskCreate(Check_Switches,NULL,100,NULL,3,&Switches_Handler);	
    /**Create the task of Lcd_Display_Count where it uses 100 word from stack and has priority 2*/
    xTaskCreate(Lcd_Display_Count,NULL,100,NULL,2,&Lcd_Display_Count_Handler);	
	/*Create the task of Lcd_Display_Congratulation where it uses 100 word from stack and has priority 1*/
	xTaskCreate(Lcd_Display_Congratulation,NULL,100,NULL,1,&Lcd_Display_Congratulation_Handler);

	/*Os start  */
	vTaskStartScheduler();
}
