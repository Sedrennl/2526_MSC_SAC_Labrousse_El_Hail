/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"

// ------------------- Définitions -------------------
#define PWM_FREQUENCY_HZ      20000U    // Fréquence PWM : 20 kHz
#define PWM_RESOLUTION_BITS   10U       // Résolution minimum 10 bits
#define PWM_DUTY_CYCLE_PERCENT 60U      // Pour les tests, rapport cyclique à 60%
#define MIN_DEAD_TIME_NS      300U      // Temps mort minimum !!!!300 ns !!!!

