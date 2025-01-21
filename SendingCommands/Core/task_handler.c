/*
 * task_handler.c
 *
 *  Created on: Jan 21, 2025
 *      Author: demom
 */

#include "main.h"

void menu_task(void *param){
	uint32_t cmd_addr;

	const char* msg_menu = "\n========================\n"
								"|         Menu         |\n"
								"========================\n"
									"LED effect    ----> 0\n"
									"Date and time ----> 1\n"
									"Exit          ----> 2\n"
									"Enter your choice here : ";

	while(1){
	xQueueSend(q_print,&msg_menu,portMAX_DELAY);
	xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);

	}
}

void led_task(void *param){

	while(1){

	}
}

void rtc_task(void *param){

	while(1){

	}
}

void print_task(void *param){

	uint32_t *msg;
	while(1){
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg),HAL_MAX_DELAY);

	}
}

void cmd_handler_task(void *param){

	while(1){

	}
}


