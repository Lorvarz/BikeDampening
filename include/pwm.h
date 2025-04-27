#ifndef PWM_H
#define PWM_H

#include "stm32f0xx.h"
#include <math.h>   // for M_PI
#include <stdio.h>

void setup_pwm(void);
void set_compression_damp(int deg);
void set_rebound_damp(int deg);
void set_pos(int pulse_width_us, int motor);
void take_button_input(void);
void EXTI4_15_IRQHandler(void);

#endif /* PWM_H */
