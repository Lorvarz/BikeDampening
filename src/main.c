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

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    command_shell();
}
#endif