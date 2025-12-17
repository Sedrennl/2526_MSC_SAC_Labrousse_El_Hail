/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_

#include "main.h"
#include "user_interface/shell.h"

// ------------------- Définitions -------------------
#define PWM_FREQUENCY_HZ      20000U    // Fréquence PWM : 20 kHz
#define PWM_RESOLUTION_BITS   10U       // Résolution minimum 10 bits
#define PWM_DUTY_CYCLE_PERCENT 60U      // Pour les tests, rapport cyclique à 60%
#define MIN_DEAD_TIME_NS      147U      // Temps mort : 147 ns (DeadTime = 25)

// ------------------- Prototypes de fonctions -------------------
void motor_init(void);
void motor_set_duty_cycle(uint8_t duty_cycle);
void motor_stop(void);
uint32_t motor_set_dead_time(uint16_t dead_time_ns);

// Commandes shell
int motor_cmd_start(h_shell_t *h_shell, int argc, char **argv);
int motor_cmd_set_duty(h_shell_t *h_shell, int argc, char **argv);
int motor_cmd_stop(h_shell_t *h_shell, int argc, char **argv);

#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
