#ifndef __SD_H
#define __SD_H
#include "stdlib.h"

int full_data_write(const char * path, char* time, 
    char*  IMU1_x_Accel, char*  IMU1_y_Accel, char*  IMU1_z_Accel,
    char*  IMU1_x_Rot, char*  IMU1_y_Rot, char*  IMU1_z_Rot,
    char*  IMU2_x_Accel, char*  IMU2_y_Accel, char*  IMU2_z_Accel, 
    char*  IMU2_x_Rot, char*  IMU2_y_Rot, char*  IMU2_z_Rot);

int write_header();

int half_data_write(const char* path, char*  time, char*  IMU_num,
    char*  IMU_x_Accel, char*  IMU_y_Accel, char*  IMU_z_Accel, 
    char*  IMU_x_Rot, char*  IMU_y_Rot, char*  IMU_z_Rot);

void write_header2(const char * path);

void init_usart5();

char interrupt_getchar();

int __io_putchar(int c);

void enable_tty_interrupt(void);

void USART3_8_IRQHandler(void);

void init_spi1_slow();

void enable_sdcard();

void disable_sdcard();

void init_sdcard_io();

void sdcard_io_high_speed();

void init_lcd_spi();

_Bool SDStable();

int imu_val_update(int raw);

char* int_to_str(int read);

void set_sd_stream();
#endif