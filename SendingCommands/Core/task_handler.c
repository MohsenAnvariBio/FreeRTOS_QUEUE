/*
 * task_handler.c
 *
 *  Created on: Jan 21, 2025
 *      Author: demom
 */

#include "main.h"

const char *msg_inv = "////Invalid option////\n";

void menu_task(void *param){
	uint32_t cmd_addr;
	command_t *cmd;
	int option;
	const char* msg_menu = "\n========================\n"
								"|         Menu         |\n"
								"========================\n"
									"LED effect    ----> 0\n"
									"Date and time ----> 1\n"
									"Exit          ----> 2\n"
									"Enter your choice here : ";

	while(1){
		xQueueSend(q_print,&msg_menu,portMAX_DELAY);

		//wait for menu commands
		xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);
		cmd = (command_t*)cmd_addr;

		if(cmd->len == 1)
		{
			option = cmd->payload[0] - 48; // convert ASCII to number by subtracting 48.
			switch(option)
			{
				case 0:
					curr_state = sLedEffect;
					xTaskNotify(handle_led_task,0,eNoAction);
					break;
				case 1:
					curr_state = sRtcMenu;
					xTaskNotify(handle_rtc_task,0,eNoAction);
					break;
				case 2: /*implement exit */
					break;
				default:
					xQueueSend(q_print,&msg_inv,portMAX_DELAY);
					continue;
			}

		}else{
			//invalid entry
			xQueueSend(q_print,&msg_inv,portMAX_DELAY);
			continue;
		}


		//wait to run again when some other task notifies
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

	}
}

void led_task(void *param){
	uint32_t cmd_addr;
		command_t *cmd;
		const char* msg_led = "========================\n"
							  "|      LED Effect     |\n"
							  "========================\n"
							  "(none,e1,e2,e3,e4)\n"
							  "Enter your choice here : ";

	while(1){
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		xQueueSend(q_print,&msg_led,portMAX_DELAY);

	}
}

void rtc_task(void *param){
	const char* msg_rtc1 = "========================\n"
								"|         RTC          |\n"
								"========================\n";

	const char* msg_rtc2 = "Configure Time            ----> 0\n"
							"Configure Date            ----> 1\n"
							"Enable reporting          ----> 2\n"
							"Exit                      ----> 3\n"
							"Enter your choice here : ";


	const char *msg_rtc_hh = "Enter hour(1-12):";
	const char *msg_rtc_mm = "Enter minutes(0-59):";
	const char *msg_rtc_ss = "Enter seconds(0-59):";

	const char *msg_rtc_dd  = "Enter date(1-31):";
	const char *msg_rtc_mo  ="Enter month(1-12):";
	const char *msg_rtc_dow  = "Enter day(1-7 sun:1):";
	const char *msg_rtc_yr  = "Enter year(0-99):";

	const char *msg_conf = "Configuration successful\n";
	const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";


	uint32_t cmd_addr;
	command_t *cmd;

	static int rtc_state = 0;
	int menu_code;

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;


	#define HH_CONFIG 		0
	#define MM_CONFIG 		1
	#define SS_CONFIG 		2

	#define DATE_CONFIG 	0
	#define MONTH_CONFIG 	1
	#define YEAR_CONFIG 	2
	#define DAY_CONFIG 		3


	while(1){
		/*Notify wait (wait till someone notifies) */
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

		/*Print the menu and show current date and time information */
		xQueueSend(q_print,&msg_rtc1,portMAX_DELAY);

	}
}

void print_task(void *param){

	uint32_t *msg;
	while(1){
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg),HAL_MAX_DELAY);

	}
}

void cmd_handler_task(void *param)
{
	BaseType_t ret;
	command_t cmd;
	while(1){
		/*Implement notify wait */
		ret = xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		if(ret == pdTRUE){
			/*process the user data(command) stored in input data queue */
			process_command(&cmd);
		}
	}
}


void process_command(command_t *cmd)
{
	extract_command(cmd);

	switch(curr_state)
	{
		case sMainMenu:
			xTaskNotify(handle_menu_task,(uint32_t)cmd , eSetValueWithOverwrite);
		break;

		case sLedEffect:
			xTaskNotify(handle_led_task,(uint32_t)cmd , eSetValueWithOverwrite);
		break;

		case sRtcMenu:
		case sRtcTimeConfig:
		case sRtcDateConfig:
		case sRtcReport:
			xTaskNotify(handle_rtc_task,(uint32_t)cmd , eSetValueWithOverwrite);
		break;

	}

}


int extract_command(command_t *cmd)
{
	uint8_t item;
	BaseType_t  status;

	// Check the queue is empty or not
	status = uxQueueMessagesWaiting(q_data );
	if(!status) return -1;


	uint8_t i =0;
	do
	{
		// Get the data from Queue
		status = xQueueReceive(q_data,&item,0);
		if(status == pdTRUE) cmd->payload[i++] = item;
	}while(item != '\n');

	cmd->payload[i-1] = '\0';
	cmd->len = i-1; /*save  length of the command excluding null char */

	return 0;
}


