//File should contain all the different testing def below active main

// #define active //Main

#include <stdio.h>
#include "stm32f0xx.h"
#include "commands.h"
#include "mpu_i2c.h"
#include "SD.h"
#include "ff.h"


#ifdef active


int main() {
    internal_clock();
    
    /* Set up for SD card writes */
    enable_tty_interrupt();
    set_sd_stream();
    char* fn = "test.csv";
    SD_setup(fn); //mount and remove previous file by name fn
    write_header(fn);

    /* Set up for alram */
    setupDAC();
    setupTIM6();

    /* Set up for pwm*/
    void take_button_input(); 

    for (;;){
        __WFI();
    }
}
#endif




//Alarm tests
// #define TEST_ALARM

//Hard coded SD tests
#define testSD   // command shell to validate
// #define testSD_1 // Header itself
// #define testSD_2 // Header + Header (test Append)
// #define testSD_3 // Hard coded number 
// #define testSD_4 // Header + multi number data writes

//Variable SD tests
// #define testSD_5 // Header + variable data writes for infinity
// #define testSD_6 // csv data type with rand data writes for infinity

//Integration Tests
// #define test_int // timer call sd writes
// #define i2c_test


#ifdef testSD
int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    set_sd_stream();

        
    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }


    command_shell();
}
#endif


#ifdef testSD_1


int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    set_sd_stream(0;)
    
    FATFS fstorage;
    FATFS* fs = &fstorage;

    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1);
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }
    write_header();
    // full_data_write(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    // half_data_write(0, 1, 1, 2, 3, 4, 5, 6);
    command_shell();
}
#endif

#ifdef testSD_2


int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    set_sd_stream();
    
    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }

    //RM previous test file
    res = f_unlink("test.txt");
        if (res != FR_OK)
            print_error(res, "this.txt");

    write_header2();
    write_header2();
    write_header2();
    test_cat("test.txt");
    // char ** args = {"", "test.txt"};
    // cat(2, args);
    // command_shell();
}
#endif

#ifdef testSD_3
#include <stdio.h>
#include "ff.h"


int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    set_sd_stream();
    
    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }

    //RM previous test file
    res = f_unlink("test.txt");
        if (res != FR_OK)
            print_error(res, "this.txt");

    // write_header();
    // write_header();
    char * buffer= "\n";
    f_printf(stdout, buffer);
    char* input = "200021220121\n";
    input_IMU(input);
    // test_cat("test.txt");
    command_shell();
}
#endif

#ifdef testSD_4



int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    set_sd_stream();
    
    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }

    //RM previous test file
    res = f_unlink("test.txt");
        if (res != FR_OK)
            print_error(res, "this.txt");


    char* time = "2:45";
    char* IMU1_x_Accel = "1";
    char* IMU1_y_Accel = "2";
    char* IMU1_z_Accel = "3";
    char* IMU1_x_Rot = "4";
    char* IMU1_y_Rot = "5";
    char* IMU1_z_Rot = "6";
    char* IMU2_x_Accel = "7";
    char* IMU2_y_Accel = "8";
    char* IMU2_z_Accel = "9";
    char* IMU2_x_Rot = "10";
    char* IMU2_y_Rot = "11";
    char* IMU2_z_Rot = "12";

    write_header("test.txt");
    full_data_write("test.txt", time,                       //IMU 1+2 prints
        IMU1_x_Accel, IMU1_y_Accel, IMU1_z_Accel,           //IMU 1 Acceleration prints
        IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot,                 //IMU 1 Rotation prints
        IMU2_x_Accel, IMU2_y_Accel, IMU2_z_Accel,           //IMU 2 Acceleration prints
        IMU2_x_Rot, IMU2_y_Rot, IMU2_z_Rot);                //IMU 2 Rotation prints

    char* IMU_num = "1";
    half_data_write("test.txt", time, IMU_num,              //IMU 1 Prints
        IMU1_x_Accel, IMU1_y_Accel,           //Acceleration prints
        IMU1_z_Accel, IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot);  //Rotation prints
    IMU_num = "2";
    half_data_write("test.txt", time, IMU_num,              //IMU 2 prints
        IMU1_x_Accel, IMU1_y_Accel,           //Acceleration prints
        IMU1_z_Accel, IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot);  //Rotation prints     
    test_cat("test.txt");
    command_shell();
}
#endif

#ifdef testSD_5
#include "time.h"
char* num_gen(int length){
    if (length <= 0) {
        return NULL;
    }

    char* str = (char*)malloc((length + 1) * sizeof(char));
    if (str == NULL) {
        return NULL;
    }

    

    for (int i = 0; i < length; i++) {
        srand(time(0));
        str[i] = (rand() * rand()) % 10 + '0';
    }
    str[length] = '\0';

    return str;
}
int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    set_sd_stream();
    
    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }

    //RM previous test file
    res = f_unlink("test.txt");
        if (res != FR_OK)
            print_error(res, "this.txt");

    //below is rand gen
    //printf(time(NULL));
    write_header("test.txt");
    for (;;){ //change back to int i=0; i < 3; i++ for non-infinity
    char* time = num_gen(4);
    char* IMU1_x_Accel = num_gen(5);
    char* IMU1_y_Accel = num_gen(5);
    char* IMU1_z_Accel = num_gen(5);
    char* IMU1_x_Rot = num_gen(5);
    char* IMU1_y_Rot = num_gen(5);
    char* IMU1_z_Rot = num_gen(5);
    char* IMU2_x_Accel = num_gen(5);
    char* IMU2_y_Accel = num_gen(5);
    char* IMU2_z_Accel = num_gen(5);
    char* IMU2_x_Rot = num_gen(5);
    char* IMU2_y_Rot = num_gen(5);
    char* IMU2_z_Rot = num_gen(5);

   
    full_data_write("test.txt", time,                       //IMU 1+2 prints
        IMU1_x_Accel, IMU1_y_Accel, IMU1_z_Accel,           //IMU 1 Acceleration prints
        IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot,                 //IMU 1 Rotation prints
        IMU2_x_Accel, IMU2_y_Accel, IMU2_z_Accel,           //IMU 2 Acceleration prints
        IMU2_x_Rot, IMU2_y_Rot, IMU2_z_Rot);                //IMU 2 Rotation prints

    char* IMU_num = "1";
    half_data_write("test.txt", time, IMU_num,              //IMU 1 Prints
        IMU1_x_Accel, IMU1_y_Accel,           //Acceleration prints
        IMU1_z_Accel, IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot);  //Rotation prints
    IMU_num = "2";
    half_data_write("test.txt", time, IMU_num,              //IMU 2 prints
        IMU1_x_Accel, IMU1_y_Accel,           //Acceleration prints
        IMU1_z_Accel, IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot);  //Rotation prints     
    }
    test_cat("test.txt");
    // command_shell();
}
#endif

#ifdef testSD_6
#include "time.h"
char* num_gen(int length){
    if (length <= 0) {
        return NULL;
    }

    char* str = (char*)malloc((length + 1) * sizeof(char));
    if (str == NULL) {
        return NULL;
    }

    

    for (int i = 0; i < length; i++) {
        srand(time(0));
        str[i] = (rand() * rand()) % 10 + '0';
    }
    str[length] = '\0';

    return str;
}
int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    set_sd_stream();

    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }

    //RM previous test file
    res = f_unlink("test.csv");
        if (res != FR_OK)
            print_error(res, "this.csv");

    //below is rand gen
    //printf(time(NULL));
    write_header("test.csv");
    for (;;){ //change back to int i=0; i < 3; i++ for non-infinity
    char* time = num_gen(4);
    char* IMU1_x_Accel = num_gen(5);
    char* IMU1_y_Accel = num_gen(5);
    char* IMU1_z_Accel = num_gen(5);
    char* IMU1_x_Rot = num_gen(5);
    char* IMU1_y_Rot = num_gen(5);
    char* IMU1_z_Rot = num_gen(5);
    char* IMU2_x_Accel = num_gen(5);
    char* IMU2_y_Accel = num_gen(5);
    char* IMU2_z_Accel = num_gen(5);
    char* IMU2_x_Rot = num_gen(5);
    char* IMU2_y_Rot = num_gen(5);
    char* IMU2_z_Rot = num_gen(5);


    full_data_write("test.csv", time,                       //IMU 1+2 prints
        IMU1_x_Accel, IMU1_y_Accel, IMU1_z_Accel,           //IMU 1 Acceleration prints
        IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot,                 //IMU 1 Rotation prints
        IMU2_x_Accel, IMU2_y_Accel, IMU2_z_Accel,           //IMU 2 Acceleration prints
        IMU2_x_Rot, IMU2_y_Rot, IMU2_z_Rot);                //IMU 2 Rotation prints

    char* IMU_num = "1";
    half_data_write("test.csv", time, IMU_num,              //IMU 1 Prints
        IMU1_x_Accel, IMU1_y_Accel,           //Acceleration prints
        IMU1_z_Accel, IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot);  //Rotation prints
    IMU_num = "2";
    half_data_write("test.csv", time, IMU_num,              //IMU 2 prints
        IMU1_x_Accel, IMU1_y_Accel,           //Acceleration prints
        IMU1_z_Accel, IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot);  //Rotation prints     
    }
    test_cat("test.csv");
}
#endif


#ifdef test_int
int main() {
    internal_clock();

    enable_ports();
    init_i2c();
    init_usart5();
    mpu6050_init(0x68);
    mpu6050_init(0x69);

    enable_tty_interrupt();

    // set_sd_stream();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    char* fn = "test.csv"; //mut be changed in tim2_irqhandler as well
    // SD_setup(fn); //mount and remove previous file by name fn
    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    // FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    FRESULT res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }
    write_header();
    
    TIM2_50ms_Init();
    

    for (;;){
        __WFI();
    }
}

#endif

#ifdef TEST_ALARM
int main()
{
    internal_clock();

    setupTIM6();
    setupDAC();

    while(1);
}

#endif

#ifdef i2c_test
int main() {
    internal_clock();

    enable_ports();
    init_i2c();
    init_usart5();
    mpu6050_init(0x68);
    mpu6050_init(0x69);
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    AccelData wheel, fork;

    while (1)
    {
        if(mpu_read_accel(0x68, &wheel) == 0)
            printf("Wheel: ax=%d  ay=%d  az=%d\n", wheel.ax, wheel.ay, wheel.az);
        else 
            printf("Failed to read wheel accel\n");
        if(mpu_read_accel(0x69, &fork) == 0)
            printf("Wheel: ax=%d  ay=%d  az=%d\n", fork.ax, fork.ay, fork.az);
        else 
            printf("Failed to read fork accel\n");

        nano_wait(5000000);
    }
}

#endif