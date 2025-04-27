#ifndef MPU_I2C
#define MPU_I2C

#include "stm32f0xx.h"


#define GROUND_NPU (0x68)
#define HIGH_NPU (0x69)

typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
} AccelData;

void internal_clock();
void enable_ports();
void init_i2c();
void i2c_stop();
void i2c_waitidle();
void i2c_clearnack();
int i2c_checknack();
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir);

#endif