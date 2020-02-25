/*
 * Hello LCD !!!
 *
 * Created: 12/02/2020 3:01:00
 * Author : Native_programmer
 */ 



/**Include Free RTOS*/
#include "UART_USER_INTERFACE.h"
#include "UART_PostBConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "Atmega32Registers.h"
#include "BitManipulation.h"
#include "Data_Types.h"
#include "DIO.h"
#include "String_Manipulation.h"
#include "ADC.h"
	

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


SemaphoreHandle_t Switch0_Couting_Semaphore;
SemaphoreHandle_t Uart_Receive_New_Charachter_Semaphore;
SemaphoreHandle_t Temp_Receive_New_Number_Semaphore;
SemaphoreHandle_t UART_RTOS_New_Data_Semaphore;
uinteg8_t  u8_UART_character='A';
uinteg32_t u32_TEMP_Number='T';
TaskHandle_t SYS_Handler=NULL;
TaskHandle_t Switches_Handler=NULL;
TaskHandle_t UART_rec_Handler=NULL;
TaskHandle_t LCD_Display_Update_Handler=NULL;
TaskHandle_t Temp_Sensor_Send_Data_Handler=NULL;

/***********************PROTOTYPE_IMPLEMENTATION*******************************/
void System_Init (void *Pv_Parameter)                        /**     No  Periodicity           -- Priority is 4**//**Execution time range micro    **/
{
    uinteg8_t gu8_LCD_Status=0;
	
	vTaskSuspend(UART_rec_Handler);	
	vTaskSuspend(Switches_Handler);
	vTaskSuspend(LCD_Display_Update_Handler);
	vTaskSuspend(Temp_Sensor_Send_Data_Handler);	

		
	while(1)
	{
	    LCD_init();
		ADC_INIT(&gstr_Temp_sensorConfig);
		Switch_init(switch0);
		UART_Init(&RTOS_UART_CONFIG);
		vTaskResume(UART_rec_Handler);
		vTaskResume(Switches_Handler);
		vTaskResume(Temp_Sensor_Send_Data_Handler);
		vTaskResume(LCD_Display_Update_Handler);	
        vTaskSuspend(SYS_Handler);
	}
}

void Send_Switch_State(void*Pv_Parameter)                    /** Periodicity 50 milisecond     -- Priority is 3**//**Execution time range micro    **/
{

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;

	/*local variable for reading the value of switch now*/
	uinteg8_t u8_value_of_switch;
	static uinteg8_t su8_switch_high_count;
	Switch0_Couting_Semaphore=xSemaphoreCreateCounting(4,0);
	while(1)
	{
		
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(27);
		/*Read switch 0 */
		Read_Switch(switch0,&u8_value_of_switch);
		/*if it is high increment the global variable */
		if(u8_value_of_switch==HIGH)
		{
			if(uxSemaphoreGetCount(Switch0_Couting_Semaphore)<4)
			{
				/*Increment counting semaphore*/
				xSemaphoreGive(Switch0_Couting_Semaphore);
			}          
		}
		else
		{
			/*Do nothing*/
		}
		Debug_By_Toggle(27);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void UART_Send_Data_Received(void*Pv_Parameter)					/** Periodicity 200 milisecond     -- Priority is 2**//**Execution time range micro    **/
{
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;
    vSemaphoreCreateBinary(Uart_Receive_New_Charachter_Semaphore);
    vSemaphoreCreateBinary(	UART_RTOS_New_Data_Semaphore);
	while(1)
	{

		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(28);
		xSemaphoreTake(Uart_Receive_New_Charachter_Semaphore,0);
        if(UART_RecByte(&u8_UART_character)!=UART_RTOS_Not_New_Data)
		{
			xSemaphoreTake(UART_RTOS_New_Data_Semaphore,0);	
		}
		xSemaphoreGive(Uart_Receive_New_Charachter_Semaphore);
		Debug_By_Toggle(28);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void Temp_Sensor_Send_Data(void*Pv_Parameter)				    /** Periodicity 200 milisecond    -- Priority is 2**//**Execution time range micro    **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;
	vSemaphoreCreateBinary(Temp_Receive_New_Number_Semaphore);
	while(1)
	{
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(29);
		xSemaphoreTake(Temp_Receive_New_Number_Semaphore,0);	
        ADC_READ(&u32_TEMP_Number);
		u32_TEMP_Number=u32_TEMP_Number/2.049;
		xSemaphoreGive(Temp_Receive_New_Number_Semaphore);
		Debug_By_Toggle(29);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void LCD_Display_Update(void*Pv_Parameter)                      /** Periodicity 200 milisecond    -- Priority is 1**//**Execution time range micro    **/
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;
	uinteg8_t u8_Switch0_Sempahore_Count_Buffer;
	static su8_index_Uart_buffer=1;
	uinteg8_t str_buffer_for_Uart_RTOS_queue[16]={0};
	uinteg8_t u8_buffer_for_temp_RTOS_queue=0;
	static uinteg8_t su8_Latched_State_Of_Switch0=LOW;	
	while(1)
	{
			
		xLastWakeTime=xTaskGetTickCount();
		Debug_By_Toggle(30);
		u8_Switch0_Sempahore_Count_Buffer=uxSemaphoreGetCount(Switch0_Couting_Semaphore);		
		while(uxSemaphoreGetCount(Switch0_Couting_Semaphore)!=0)
		{
			xSemaphoreTake(Switch0_Couting_Semaphore,0);
		}		
		if(u8_Switch0_Sempahore_Count_Buffer>=2)
		{
           if(su8_Latched_State_Of_Switch0==LOW)
           {
			    su8_index_Uart_buffer=1;
				LCD_clearScreen();
           }		
		   su8_Latched_State_Of_Switch0=HIGH;
		
		}
		else
		{
		   su8_Latched_State_Of_Switch0=LOW;			
		}
		if(uxSemaphoreGetCount(UART_RTOS_New_Data_Semaphore)==0)
		{
			xSemaphoreGive(UART_RTOS_New_Data_Semaphore);	
			xSemaphoreTake(Uart_Receive_New_Charachter_Semaphore,0);
			if(su8_index_Uart_buffer==1)
			{
				LCD_clearScreen();
			}
			LCD_goToRowColumn(2,su8_index_Uart_buffer);
			LCD_displayCharacter(u8_UART_character);
			if(su8_index_Uart_buffer==16) 
			{
				su8_index_Uart_buffer=1;
			}
			else
			{
               su8_index_Uart_buffer++;				
			}
			xSemaphoreGive(Uart_Receive_New_Charachter_Semaphore);					
		}
		else
		{

			
		}
		xSemaphoreTake(Temp_Receive_New_Number_Semaphore,0);	
		LCD_integerToString(u32_TEMP_Number,1,1);
		xSemaphoreGive(Temp_Receive_New_Number_Semaphore);	

		Debug_By_Toggle(30);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

int main(void)
{
    /**Create the task of System_Init where it uses 100 word from stack and has priority 4*/
    xTaskCreate(System_Init               ,NULL,100,NULL,4,&SYS_Handler);		
    /**Create the task of Check_Switches where it uses 100 word from stack and has priority 3*/
    xTaskCreate(Send_Switch_State         ,NULL,100,NULL,3,&Switches_Handler);	
    /**Create the task of UART_rec where it uses 100 word from stack and has priority 3*/
    xTaskCreate(UART_Send_Data_Received   ,NULL,100,NULL,2,&UART_rec_Handler);	
    /**Create the task of Temp_Sensor_Read_Value where it uses 100 word from stack and has priority 3*/
    xTaskCreate(Temp_Sensor_Send_Data     ,NULL,100,NULL,2,&Temp_Sensor_Send_Data_Handler);
    /**Create the task of UART_rec where it uses 100 word from stack and has priority 3*/
    xTaskCreate(LCD_Display_Update        ,NULL,100,NULL,1,&LCD_Display_Update_Handler);

	/*Os start  */
	vTaskStartScheduler();
}
