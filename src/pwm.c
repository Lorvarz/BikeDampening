#include "stm32f0xx.h"
#include <math.h>   // for M_PI
#include <stdio.h>
#include "pwm.h"

void setup_pwm(void) {
    // Generally the steps are similar to those in setup_tim3
    // except we will need to set the MOE bit in BDTR. 
    // Be sure to do so ONLY after enabling the RCC clock to TIM1.

    //Enable RCC clock for TIM1
    RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; //enable RCC clock GPIOA

    //Set PA8-PA11 to alternate function mode in GPIOA_MODER
    // Set PA8-PA11 to alternate function mode ('10')
    GPIOA -> MODER &= ~(0xFF << (8 * 2));
    GPIOA -> MODER |= (0xAA << (8 * 2));

    //Set GPIO8-11 to AF2 in AFR register
    GPIOA->AFR[1] &= ~(0xFFFF << 0);
    GPIOA->AFR[1] |= (0x2222 << 0);

    // RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;

    //Enable the MOE bit in the TIM1 BDTR register
    TIM1 -> BDTR |= TIM_BDTR_MOE;

    //48 MHz -> 1kHz
    TIM1 -> PSC = 480 - 1;

    //1kHz -> 50 Hz
    TIM1 -> ARR = 2000 - 1;

    // Configure channels 1 and 2 for PWM mode 1
    TIM1->CCMR1 &= ~(0b111 << 4 | 0b111 << 12);
    TIM1->CCMR1 |= (0b110 << 4 | 0b110 << 12);

    //Enable all four channel outputs in CCER register
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    //Enable Timer 1
    TIM1->CR1 |= TIM_CR1_CEN;
}

//Takes a number in between 0 and 90, and sets the motor position to that degree
void set_compression_damp(int deg) {
    //pulse width in microseconds dependent on deg
    int pulse_width = 500 + ((deg * 1000) / 180);

    set_pos(pulse_width, 1);
}

void set_rebound_damp(int deg) {
    int pulse_width = 500 + ((deg * 1000) / 90);

    set_pos(pulse_width, 2);
}

//takes in microsecond pulse width and outputs PWM signal and outputs it to it's respective motor
void set_pos(int pulse_width_us, int motor)
{
    if(motor == 1) //if motor is the compression damper
        TIM1 -> CCR1 = pulse_width_us / 10;
    else if (motor ==  2) //if motor is rebound damper
        TIM1 -> CCR2 = pulse_width_us / 10;
}

void take_button_input()
{
    static int toggle = 0;

    // Enable GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Set PC5 as input (00)
    GPIOC->MODER &= ~(0x3 << (5 * 2));

    // Optional: Enable pull-down resistor for PC5
    GPIOC->PUPDR &= ~(0x3 << (5 * 2));
    GPIOC->PUPDR |= (0x2 << (5 * 2));

    static int prev_state = 0;

    // Configure EXTI line 5 for PC5
    SYSCFG->EXTICR[1] &= ~(0xF << 4); // Clear EXTI5 bits
    SYSCFG->EXTICR[1] |= (0x2 << 4);  // Set EXTI5 to port C

    EXTI->IMR |= (1 << 5);   // Unmask interrupt
    EXTI->FTSR |= (1 << 5);  // Trigger on falling edge (button press)

    NVIC_EnableIRQ(EXTI4_15_IRQn); // Enable EXTI4_15 interrupt in NVIC
}

// Interrupt handler for EXTI line 5 (PC5)
void EXTI4_15_IRQHandler(void) {
    static int toggle = 0;

    // Check if EXTI line 5 triggered the interrupt
    if (EXTI->PR & (1 << 5)) {
        // Clear the pending interrupt flag
        EXTI->PR |= (1 << 5);

        // Toggle compression damper between 0 and 90 degrees
        if (toggle) {
            set_compression_damp(0);
        } else {
            set_compression_damp(90);
        }
        toggle = !toggle;
    }
}