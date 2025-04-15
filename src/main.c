//File should contain all the different testing def below active main

#define active //Main
// #define testSD //SD and LCD testing


#ifdef active
#include <stdio.h>
#include "stm32f0xx.h"

int main() {
    internal_clock();
}
#endif

#ifdef testSD
#include <stdio.h>
#include "ff.h"

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    
    // FATFS fstorage;
    // FATFS* fs = &fstorage;

    // FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    // res = f_mount(fs, "", 1);
    // if (res != FR_OK){
    //     print_error(res, "Error occurred while mounting");
    // }
    // write_header();
    // full_data_write(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    // half_data_write(0, 1, 1, 2, 3, 4, 5, 6);
    command_shell();
}
#endif