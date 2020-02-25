#include"UART_PostBConfig.h"




const UART_Confg_Stuct_t RTOS_UART_CONFIG={   UART_trasmit_Disable
                                             ,UART_Receive_Enable
											 ,_bps_9600_
											 ,UART_Parity_Disabled
											 ,UART_One_Stop_bit
											 ,UART_Frame_8_bit_
											 ,UART_FreeRTOS_mode_enable         };

