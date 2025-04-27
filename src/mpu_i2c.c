/**
 ******************************************************************************
 * @file    main.c
 * @author  Niraj Menon
 * @date    Sep 23, 2024
 * @brief   ECE 362 I2C Lab Student template
 ******************************************************************************
 */

#include "stm32f0xx.h"
#include <stdio.h>
#include <stdbool.h>

#include "mpu_i2c.h"
#include "alarm.h"

// used for accelerometer data
typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
} AccelData;

//===========================================================================
// Configure SDA and SCL.
//===========================================================================
void enable_ports(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER &= ~((3 << (6 * 2)) | (3 << (7 * 2)));      // clear
    GPIOB->MODER |= ((2 << (6 * 2)) | (2 << (7 * 2)));       // set to alternate function mode

    GPIOB->OTYPER |= (1 << 6) | (1 << 7); //open drain
    GPIOB->AFR[0] &= ~((0xF << (6 * 4)) | (0xF << (7 * 4))); // clear afr
    GPIOB->AFR[0] |= ((0x1 << (6 * 4)) | (0x1 << (7 * 4)));  // set af1 for i2c1

    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
}

//===========================================================================
// Configure I2C1.
//===========================================================================
void init_i2c(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // enable clock

    I2C1->CR1 &= ~I2C_CR1_PE; // disable peripheral

    I2C1->CR1 &= ~I2C_CR1_ANFOFF; // analog noise filter off

    I2C1->CR1 |= I2C_CR1_ERRIE; // error interrupts enable

    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH; // disable clock stretching

    I2C1->TIMINGR = (5 << 28) | (3 << 20) | (3 << 16) | (3 << 8) | (9 << 0); // reference table 85 of family reference

    I2C1->CR2 &= ~I2C_CR2_ADD10; // 7 bit addressing mode

    I2C1->CR1 |= I2C_CR1_PE; // enable peripheral
}

//===========================================================================
// Send a START bit.
//===========================================================================
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir)
{
    // 0. Take current contents of CR2 register.
    uint32_t tmpreg = I2C1->CR2;

    // 1. Clear the following bits in the tmpreg: SADD, NBYTES, RD_WRN, START, STOP
    tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    // 2. Set read/write direction in tmpreg.
    if (dir == 1)
    {
        tmpreg |= I2C_CR2_RD_WRN;
    }
    else
    {
        tmpreg &= ~I2C_CR2_RD_WRN;
    }
    // 3. Set the target's address in SADD (shift targadr left by 1 bit) and the data size.
    tmpreg |= ((targadr << 1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);

    // 4. Set the START bit.
    tmpreg |= I2C_CR2_START;

    // 5. Start the conversion by writing the modified value back to the CR2 register.
    I2C1->CR2 = tmpreg;
}

//===========================================================================
// Send a STOP bit.
//===========================================================================
void i2c_stop(void)
{
    // 0. If a STOP bit has already been sent, return from the function.
    // Check the I2C1 ISR register for the corresponding bit.
    if (I2C1->ISR & I2C_ISR_STOPF)
    {
        return;
    }
    // 1. Set the STOP bit in the CR2 register.
    I2C1->CR2 |= I2C_CR2_STOP;
    // 2. Wait until STOPF flag is reset by checking the same flag in ISR.
    while ((I2C1->ISR & I2C_ISR_STOPF) == 0)
        ;
    // 3. Clear the STOPF flag by writing 1 to the corresponding bit in the ICR.
    I2C1->ICR |= I2C_ICR_STOPCF;
}

//===========================================================================
// Wait until the I2C bus is not busy. (One-liner!)
//===========================================================================
void i2c_waitidle(void)
{
    while (I2C1->ISR & I2C_ISR_BUSY);
}

//===========================================================================
// Send each char in data[size] to the I2C bus at targadr.
//===========================================================================
int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size)
{
    i2c_waitidle();              // wait until i2c is idle
    i2c_start(targadr, size, 0); // send a start condition to target address with the write bit set

    for (int i = 0; i < size; i++)
    {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_TXIS) == 0)
        {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack())
            {
                i2c_clearnack();
                i2c_stop();
                return -1;
            }
        }
        I2C1->TXDR = data[i] & I2C_TXDR_TXDATA;
    }

    while ((I2C1->ISR & I2C_ISR_TC) == 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0)
        ; // loop until the tc and nackf are not set
    if (i2c_checknack())
    {
        i2c_clearnack();
        return -1;
    }

    i2c_stop();
    return 0;
}

//===========================================================================
// Receive size chars from the I2C bus at targadr and store in data[size].
//===========================================================================
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size)
{
    uint8_t *buf = (uint8_t *)data;
    i2c_waitidle();
    i2c_start(targadr, size, 1);
    for (int i = 0; i < size; i++)
    {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_RXNE) == 0)
        {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack())
            {
                i2c_clearnack();
                i2c_stop();
                return -1;
            }
        }
        buf[i] = I2C1->RXDR & I2C_RXDR_RXDATA;
    }
    i2c_stop();
    return 0;
}

//===========================================================================
// Clear the NACK bit. (One-liner!)
//===========================================================================
void i2c_clearnack(void)
{
    I2C1->ICR |= I2C_ICR_NACKCF;
}

//===========================================================================
// Check the NACK bit. (One-liner!)
//===========================================================================
int i2c_checknack(void)
{
    return (I2C1->ISR & I2C_ISR_NACKF) ? 1 : 0;
}

int __io_putchar(int c) {
    // TODO copy from STEP2
    if(c == '\n')
    {
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE));

    USART5->TDR = c;
    return c;
}
//===========================================================================
// __io_getchar
//===========================================================================
int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    return interrupt_getchar();
}
//===========================================================================
// IRQHandler for USART5
//===========================================================================

#define FIFOSIZE 16
extern char serfifo[FIFOSIZE];
extern int seroffset;
extern struct fifo input_fifo;

int USART3_8_IRQHandler(void){
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    } 
}

static bool mpuError = false;
int mpu_read_accel(uint8_t addr, AccelData *accel)
{
    uint8_t reg = 0x3B; // Start of accelerometer data 
    uint8_t raw[6];

    if (i2c_senddata(addr, &reg, 1) < 0) {
        mpuError = true;
        printf("MPU @ 0x%02X write failed\n", addr);
        return -1;
    }

    if (i2c_recvdata(addr, raw, 6) < 0) {
        mpuError = true;
        printf("MPU @ 0x%02X read failed\n", addr);
        return -1;
    }

    accel->ax = (raw[0] << 8) | raw[1];
    accel->ay = (raw[2] << 8) | raw[3];
    accel->az = (raw[4] << 8) | raw[5];
    
    mpuError = false;
    return 0;
}

void mpu6050_init(uint8_t addr)
{
    uint8_t config[2];
    config[0] = 0x6B;
    config[1] = 0x00;

    if (i2c_senddata(addr, config, 2) < 0)
    {
        printf("Failed to wake MPU @ 0x%02x\n", addr);
    }
    else
    {
        printf("MPU @ 0x%02x\n", addr);
    }
}

void setup_imu(){
    enable_ports();
    init_i2c();
    mpu6050_init(GROUND_NPU);
    mpu6050_init(HIGH_NPU);
    enable_tty_interrupt();
}

void inline mpuStable()
{
    return !mpuError;
}
