/*
 * app.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "app.h"

#include "user_interface/shell.h"
#include "motor_control/motor.h"

static char shell_uart2_received_char;

void init_device(void){
// Initialisation user interface
	// SHELL
	hshell1.drv.transmit = shell_uart2_transmit;
	hshell1.drv.receive = shell_uart2_receive;
	shell_init(&hshell1);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&shell_uart2_received_char, 1);

	// Ajout des commandes PWM
	shell_add(&hshell1, "pwm_start", motor_cmd_start, "Demarre les PWM a 60%");
	shell_add(&hshell1, "pwm_set", motor_cmd_set_duty, "Change le rapport cyclique (0-100%)");
	shell_add(&hshell1, "pwm_stop", motor_cmd_stop, "Arrete les PWM");

	// LED
	led_init();

	// BUTTON
//	button_init();
//
// Initialisation motor control
	// MOTOR
	motor_init();
	// ASSERV (PID)
//	asserv_init();
//
// Initialisation data acquistion
	// ANALOG INPUT
//	input_analog_init();
	// ENCODER INPUT
//	input_encoder_init();
}

uint8_t shell_uart2_transmit(const char *pData, uint16_t size)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)pData, size, HAL_MAX_DELAY);
	return size;
}

uint8_t shell_uart2_receive(char *pData, uint16_t size)
{
	*pData = shell_uart2_received_char;
	return 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&shell_uart2_received_char, 1);
		shell_run(&hshell1);
	}
}

void loop(){

}
