#ifndef MPU_6050H
#define MPU_6050H

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
extern void nano_wait(int);
void mpu6050_init(0x68)
void mpu6050_init(0x69)

#endif