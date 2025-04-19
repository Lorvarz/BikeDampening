//File should contain all the different testing def below active main

#define active //Main
// #define testSD_1 // Header itself
// #define testSD_2 // Header + Header (test Append)
// #define testSD_3 // Header + Hard coded values for data writes
// #define testSD_4 // Header + Variable data writes


#ifdef active
#include <stdio.h>
#include "stm32f0xx.h"

int main() {
    internal_clock();
}
#endif

#ifdef testSD_1
#include <stdio.h>
#include "ff.h"

int main() {
    internal_clock();
    // init_usart5();
    // enable_tty_interrupt();
    // setbuf(stdin,0);
    // setbuf(stdout,0);
    // setbuf(stderr,0);
    
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
#include <stdio.h>
#include "ff.h"

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    
    FATFS fstorage;
    FATFS* fs = &fstorage;

    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1);
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }
    write_header();
    write_header();
    // full_data_write(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    // half_data_write(0, 1, 1, 2, 3, 4, 5, 6);
    command_shell();
}
#endif

#ifdef testSD_3
#include <stdio.h>
#include "ff.h"

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    
    FATFS fstorage;
    FATFS* fs = &fstorage;

    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1);
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }

    char* temp_0 = "0";
    char* temp_1 = "1";
    char* temp_2 = "2";
    char* temp_3 = "3";
    char* temp_4 = "4";
    char* temp_5 = "5";
    char* temp_6 = "6";
    char* temp_7 = "7";
    char* temp_8 = "8";
    char* temp_9 = "9";
    char* temp_10 = "10";
    char* temp_11 = "11";
    char* temp_12 = "12";

    write_header();
    full_data_write(temp_0, temp_1, temp_2, temp_3, temp_4, temp_5, temp_6, temp_7, temp_8, temp_9, temp_10, temp_11, temp_12);
    half_data_write(temp_0, temp_1, temp_1, temp_2, temp_3, temp_4, temp_5, temp_6);
    command_shell();
}
#endif