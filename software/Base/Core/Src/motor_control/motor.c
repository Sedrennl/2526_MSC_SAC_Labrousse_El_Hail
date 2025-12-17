/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"
#include "tim.h"
#include <stdio.h>
#include <stdlib.h>

// ------------------- Définitions -------------------
#define PWM_FREQUENCY_HZ      20000U    // Fréquence PWM : 20 kHz
#define PWM_RESOLUTION_BITS   10U       // Résolution minimum 10 bits
#define PWM_DUTY_CYCLE_PERCENT 60U      // Pour les tests, rapport cyclique à 60%
#define MIN_DEAD_TIME_NS      147U      // Temps mort : 147 ns (DeadTime = 25)

/**
 * @brief  Configure le temps mort par écriture directe dans le registre
 * @param  dead_time_ns : Temps mort souhaité en nanosecondes (max 170 ns)
 * @retval Temps mort réel configuré en ns
 */
uint32_t motor_set_dead_time(uint16_t dead_time_ns)
{
    // Calcul de la valeur DeadTime
    // Formule : DeadTime = dead_time_ns / t_clk
    // avec t_clk = 1/170MHz = 5.88 ns

    const float t_clk_ns = 1000.0f / 170.0f;  // = 5.88 ns
    uint8_t dead_time_value = (uint8_t)((float)dead_time_ns / t_clk_ns);

    // Vérification de la contrainte : temps mort <= 170 ns
    if (dead_time_value > 28)
    {
        dead_time_value = 28;  // Limite à 28 (164.6 ns)
        printf("Attention: Temps mort limite a 164.6 ns (DeadTime=28)\r\n");
    }

    // Écriture directe dans le registre BDTR (Break and Dead-Time Register)
    // Le champ DTG (Dead-Time Generator) est sur les bits [7:0]
    uint32_t bdtr = TIM1->BDTR;
    bdtr &= ~(0xFF);  // Efface les bits [7:0]
    bdtr |= dead_time_value;  // Écrit la nouvelle valeur
    TIM1->BDTR = bdtr;

    // Calcul du temps mort réel
    uint32_t real_dead_time_ns = (uint32_t)(dead_time_value * t_clk_ns);

    printf("DeadTime register = %d\r\n", dead_time_value);

    return real_dead_time_ns;
}

/**
 * @brief  Initialise les PWM pour la commande du hacheur (ponts U et V)
 * @note   - Fréquence PWM : 20 kHz
 *         - Temps mort : 147 ns (DeadTime = 25)
 *         - Résolution : 8500 niveaux > 1024 (10 bits) ✓
 *         - Rapport cyclique initial : 60%
 *         - Signaux complémentaires avec temps mort
 * @retval None
 */
void motor_init(void)
{
    // Démarrage des PWM complémentaires pour ponts U et V
    // Les signaux sont automatiquement complémentaires avec temps mort

    // Pont U : CH1 (PA8) et CH1N (PB13) - Complémentaires
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);      // U_PWM_H (PA8)
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);   // U_PWM_L (PB13) - Complémentaire de PA8

    // Pont V : CH2 (PA9) et CH2N (PB14) - Complémentaires
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);      // V_PWM_H (PA9)
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);   // V_PWM_L (PB14) - Complémentaire de PA9

    // Configuration du temps mort : 147 ns (DeadTime = 25)
    uint32_t dead_time_real = motor_set_dead_time(147);

    printf("Temps mort configure: %lu ns (contrainte <= 170 ns)\r\n", dead_time_real);
    printf("Signaux complementaires avec temps mort actifs\r\n");

    // Initialisation du rapport cyclique à 60% pour les tests
    motor_set_duty_cycle(PWM_DUTY_CYCLE_PERCENT);
}

/**
 * @brief  Définit le rapport cyclique des PWM du hacheur
 * @param  duty_cycle : Rapport cyclique en pourcentage (0 à 100%)
 * @note   Le même rapport cyclique est appliqué aux ponts U et V
 * @retval None
 */
void motor_set_duty_cycle(uint8_t duty_cycle)
{
    // Limitation de la valeur entre 0 et 100%
    if (duty_cycle > 100)
        duty_cycle = 100;

    // Calcul de la valeur du registre CCR (Compare)
    // Formule : CCR = (duty_cycle * (Period + 1)) / 100
    // Avec Period = 8499, on a : CCR = (duty_cycle * 8500) / 100
    uint32_t ccr_value = (duty_cycle * (htim1.Init.Period + 1)) / 100;

    // Application du rapport cyclique sur les 2 ponts (U et V)
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr_value);  // Pont U
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, ccr_value);  // Pont V
}

/**
 * @brief  Arrête toutes les PWM du hacheur
 * @note   Désactive les 4 sorties PWM (U+, U-, V+, V-)
 * @retval None
 */
void motor_stop(void)
{
    // Arrêt des PWM du pont U
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

    // Arrêt des PWM du pont V
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

/**
 * @brief  Commande shell : Démarre les PWM du moteur à 60%
 */
int motor_cmd_start(h_shell_t *h_shell, int argc, char **argv)
{
    (void)h_shell;  // Paramètre non utilisé
    (void)argc;
    (void)argv;

    motor_init();
    printf("PWM demarrees : U et V a 60%%\r\n");

    return 0;
}

/**
 * @brief  Commande shell : Change le rapport cyclique des PWM
 */
int motor_cmd_set_duty(h_shell_t *h_shell, int argc, char **argv)
{
    (void)h_shell;  // Paramètre non utilisé

    if (argc < 2)
    {
        printf("Usage: pwm_set <duty_cycle>\r\n");
        printf("Exemple: pwm_set 75\r\n");
        return -1;
    }

    // Conversion de l'argument en nombre
    int duty_cycle = atoi(argv[1]);

    if (duty_cycle < 0 || duty_cycle > 100)
    {
        printf("Erreur: duty_cycle doit etre entre 0 et 100\r\n");
        return -1;
    }

    motor_set_duty_cycle((uint8_t)duty_cycle);
    printf("Rapport cyclique change a %d%%\r\n", duty_cycle);

    return 0;
}

/**
 * @brief  Commande shell : Arrête les PWM du moteur
 */
int motor_cmd_stop(h_shell_t *h_shell, int argc, char **argv)
{
    (void)h_shell;  // Paramètre non utilisé
    (void)argc;
    (void)argv;

    motor_stop();
    printf("PWM arretees\r\n");

    return 0;
}
