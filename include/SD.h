#ifndef __SD_H
#define __SD_H
#include "stdlib.h"

int full_data_write(int time, int IMU1_x_Accel, int IMU1_y_Accel, int IMU1_z_Accel,
    int IMU1_x_Rot, int IMU1_y_Rot, int IMU1_z_Rot,
    int IMU2_x_Accel, int IMU2_y_Accel, int IMU2_z_Accel, 
    int IMU2_x_Rot, int IMU2_y_Rot, int IMU2_z_Rot);

int write_header();

int half_data_write(int time, int IMU_num,int IMU_x_Accel, int IMU_y_Accel, 
    int IMU_z_Accel, int IMU_x_Rot, int IMU_y_Rot, int IMU_z_Rot);
#endif