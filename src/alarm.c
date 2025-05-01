#include "stm32f0xx.h"

#ifndef ALARM_C
#include "alarm.h"
#include <math.h>

// ---------------------------- setup macros/funcs ----------------------------
#define rightFill(numBits) (~(~0 << (numBits)))
#define clearBits(register, left, right) register &= ~(rightFill(((left) + 1)) - rightFill(right))
#define setBits(register, left, right, seq) clearBits(register, left, right); \ 
    register |= (seq) << (right)

#define enClock(peripheral) RCC->AHBENR |= RCC_AHBENR_##peripheral##EN
#define enPortClock(port) enClock(GPIO##port)

#define acknoledgeTimInter(num) TIM##num->SR &= ~TIM_SR_UIF

typedef enum pinMode {
    INPUT_PIN,
    OUTPUT_PIN,
    ANALOG_PIN
} pinMode;

void setPinMode(GPIO_TypeDef* port, uint8_t start, uint8_t end, pinMode mode) {
    uint8_t pinSet;
    if (mode == INPUT_PIN)
    {
        pinSet = 0b00;
    } else if (mode == OUTPUT_PIN)
    {
        pinSet = 0b01;
    } else if (mode == ANALOG_PIN)
    {
        pinSet = 0b11;
    } else return;

    for (int idx = start; idx <= end; idx++) {
        setBits(port->MODER, 2*idx + 1, 2*idx, pinSet);
    }
}

typedef enum outType {
    PUSH_PULL,
    OPEN_DRAIN
} outType;

void setPinOutType(GPIO_TypeDef* port, uint8_t start, uint8_t end, outType type) {
    uint8_t pinSet;
    if (type == PUSH_PULL)
    {
        pinSet = 0b0;
    } else if (type == OPEN_DRAIN)
    {
        pinSet = 0b1;
    } else return;

    for (int idx = start; idx <= end; idx++) {
        setBits(port->OTYPER, idx, idx, pinSet);
    }
}

typedef enum pullType {
    NO_PULL,
    PULL_UP,
    PULL_DOWN
} pullType;

void setPinPull(GPIO_TypeDef* port, uint8_t start, uint8_t end, pullType type) {
    uint8_t pinSet;
    if (type == NO_PULL)
    {
        pinSet = 0b00;
    } else if (type == PULL_UP)
    {
        pinSet = 0b01;
    } else if (type == PULL_DOWN)
    {
        pinSet = 0b10;
    } else return;

    for (int idx = start; idx <= end; idx++) {
        setBits(port->PUPDR, 2*idx + 1, 2*idx, pinSet);
    }
}


// ---------------------------- constants ----------------------------
#define BASE_CLK (48000000)
#define TIM6_PSC (6 - 1)
#define TIM6_ARR (400 - 1)
#define SOUND_FREQ (1000)
#define N (1000)
#define RATE (20000)
#define VOLUME (440)

short int wavetable[N];
int step = (SOUND_FREQ * N / RATE) * (1<<16);;
int offset = 0;

// #define M_PI 3.14159
static void init_wavetable(void) {
    for(int i=0; i < N; i++)
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
}

// ---------------------------- TIM6 config ----------------------------

void setupDAC()
{
    enPortClock(A);
    setPinMode(GPIOA, 4, 4, ANALOG_PIN);
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    setBits(DAC1->CR, 5, 3, 0b000);
    DAC1->CR |= DAC_CR_TEN1;
    DAC1->CR |= DAC_CR_EN1;
    init_wavetable();
}

void setupTIM6()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = TIM6_PSC;
    TIM6->ARR = TIM6_ARR;
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_IRQn);
    NVIC_SetPriority(TIM6_IRQn, 15);
    TIM6->CR1 |= TIM_CR1_CEN;
    setBits(TIM6->CR2, 6, 4, 0b010); //Master update mode
}

bool isConnected = true;
uint8_t numPulses = 3;
uint16_t pulseLength = 10; // measured in cycles
uint16_t intervalLength = 50; // measured in ISR calls
uint16_t resetLength = 150; // measured in ISR calls

uint8_t pulsesPlayed = 0; // how many pulses have played since last start
uint16_t cyclesPlayed = 0; //how many cycles have played
uint16_t intervalCounter = 0; // how many calls have passed
bool inReset = false;

void TIM6_DAC_IRQHandler()
{
    acknoledgeTimInter(6);

    bool newStable = isConnStable();
    if (!isConnected && newStable)
    {
        isConnected = true;
        inReset = false;
        intervalCounter = 0;
        pulsesPlayed = 0;
        cyclesPlayed = 0;
        return;
    }else if (isConnected && newStable)
    {
        return;
    } else if (isConnected && !newStable)
    {
        isConnected = false;
        inReset = false;
        intervalCounter = 0;
        pulsesPlayed = 0;
        cyclesPlayed = 0;
    }

    
    if (pulsesPlayed < numPulses)
    {
        offset += step;
        if (offset >= (N << 16))
        {
            offset -= (N<<16);
            if (++cyclesPlayed == pulseLength)
            {
                pulsesPlayed++;
                cyclesPlayed = 0;
            }
        }

        int sample = wavetable[offset >> 16];
        sample *= VOLUME;
        sample >>= 17;
        sample += 2048;
        DAC->DHR12R1 = sample;
    } else if (!inReset && intervalCounter < intervalLength)
    {   
        intervalCounter++;
        pulsesPlayed = 0;
        if (intervalCounter == intervalLength)
        {
            inReset = true;
            intervalCounter = 0;
        }
    } else if (inReset && intervalCounter < resetLength)
    {
        if (++intervalCounter == resetLength)
        {
            inReset = false;
            intervalCounter = 0;
            pulsesPlayed = 0;
            cyclesPlayed = 0;
        }
    }
}

bool isConnStable()
{
    // return false;
    return (SDStable() && mpuStable());
}

#endif