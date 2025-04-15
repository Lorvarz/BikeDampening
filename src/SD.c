#include "stm32f0xx.h"
#include <stdint.h>
#include "tty.h"
#include "fifo.h"
#include "commands.h"
#include <stdio.h>


void write_header(){
    const char str[] = "Date/Time,IMU1 X Acceleration,IMU1 Y Acceleration,IMU1 Z Acceleration,"
    "IMU1 X Rotation,IMU1 Y Rotaion,IMU1 Z Rotation,IMU2 X Acceleration,IMU2 Y Acceleration,"
    "IMU2 Z Acceleration,IMU2 X Rotation,IMU2 Y Rotaion,IMU2 Z Rotation\n";
    int chk = input_IMU(str);
    if (chk) printf("failed to write header");
}
void full_data_write(int time, int IMU1_x_Accel, int IMU1_y_Accel, int IMU1_z_Accel,
    int IMU1_x_Rot, int IMU1_y_Rot, int IMU1_z_Rot,
    int IMU2_x_Accel, int IMU2_y_Accel, int IMU2_z_Accel, 
    int IMU2_x_Rot, int IMU2_y_Rot, int IMU2_z_Rot){
        int chk;
        char * nl = '/n';

        chk |= input_IMU(time);

        chk |= input_IMU(IMU1_x_Accel);
        chk |= input_IMU(IMU1_y_Accel);
        chk |= input_IMU(IMU1_z_Accel);

        chk |= input_IMU(IMU1_x_Rot);
        chk |= input_IMU(IMU1_y_Rot);
        chk |= input_IMU(IMU1_z_Rot);

        chk |= input_IMU(IMU2_x_Accel);
        chk |= input_IMU(IMU2_y_Accel);
        chk |= input_IMU(IMU2_z_Accel);

        chk |= input_IMU(IMU2_x_Rot);
        chk |= input_IMU(IMU2_y_Rot);
        chk |= input_IMU(IMU2_z_Rot);

        chk |= input_IMU(nl);

        if (chk) printf("failed to write somewhere, %d", chk);
        

    }

void half_data_write(int time, int IMU_num,int IMU_x_Accel, int IMU_y_Accel, 
    int IMU_z_Accel, int IMU_x_Rot, int IMU_y_Rot, int IMU_z_Rot){
        int chk;
        char * nl = '/n';

        if (IMU_num == 1){
        
            chk |= input_IMU(time);

            chk |= input_IMU(IMU_x_Accel);
            chk |= input_IMU(IMU_y_Accel);
            chk |= input_IMU(IMU_z_Accel);

            chk |= input_IMU(IMU_x_Rot);
            chk |= input_IMU(IMU_y_Rot);
            chk |= input_IMU(IMU_z_Rot);

            chk |= input_IMU(0);
            chk |= input_IMU(0);
            chk |= input_IMU(0);

            chk |= input_IMU(0);
            chk |= input_IMU(0);
            chk |= input_IMU(0);
        }
        else {
            chk |= input_IMU(time);

            chk |= input_IMU(0);
            chk |= input_IMU(0);
            chk |= input_IMU(0);

            chk |= input_IMU(0);
            chk |= input_IMU(0);
            chk |= input_IMU(0);

            chk |= input_IMU(IMU_x_Accel);
            chk |= input_IMU(IMU_y_Accel);
            chk |= input_IMU(IMU_z_Accel);

            chk |= input_IMU(IMU_x_Rot);
            chk |= input_IMU(IMU_y_Rot);
            chk |= input_IMU(IMU_z_Rot);
        }

        chk |= input_IMU(nl);
    }


void init_usart5() {
    //Enable the RCC clocks for GPIOC and GPIOD
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    //setup gpioc12 to USART5_TX
    GPIOC->MODER |= 0x02000000;
    GPIOC->MODER &= 0xfeffffff;
    GPIOC->AFR[1] |= 0x00020000;
    GPIOC->AFR[1] &= 0xfff2ffff;

    //setup for gpiod2 to USART5_RX
    GPIOD->MODER |= 0x20;
    GPIOD->MODER &= 0xffffffef;
    GPIOD->AFR[0] |= 0x200;
    GPIOD->AFR[0] &= 0xfffff2ff;

    //enable RCC clock to the USART5 peripheral
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;
    USART5->CR1 &= ~(USART_CR1_UE); //turn off the UE bit
    USART5->CR1 &= ~(USART_CR1_M); //Set M0 and M1 to 0 (8 bit word length)
    USART5->CR1 &= ~(USART_CR1_PCE); //turn off parity control
    USART5->CR2 &= ~(USART_CR2_STOP); //turn off STOP bits (00) 1 stop bit
    USART5->CR1 &= ~(USART_CR1_OVER8); //oversampling by  16
    USART5->BRR = 0x1A1; //sysclk / Baud rate

    USART5->CR1 |= USART_CR1_TE; //enable transmit
    USART5->CR1 |= USART_CR1_RE; //enable receive
    USART5->CR1 |= USART_CR1_UE; //enable usart5


    while (!(USART5->ISR & USART_ISR_TEACK) & !(USART5->ISR & USART_ISR_REACK));
}

char interrupt_getchar() {
    // Wait for a newline to complete the buffer.
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi"); // wait for an interrupt
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    return (int) interrupt_getchar();
}

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

int __io_putchar(int c) {
    // TODO copy from STEP2
    if (c == '\n'){
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

void enable_tty_interrupt(void) {
    USART5->CR1 |= USART_CR1_RXNEIE;//raise an interupt every time the RDR becomes not empty
        //NVIC->ISER[0] |= (0b1 << 29);
    NVIC_EnableIRQ(USART3_8_IRQn);
    USART5->CR3 |= USART_CR3_DMAR; //dma request
    
    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;
    DMA2_Channel2->CCR &= ~DMA_CCR_EN;  // First make sure DMA is turned off
    
    // The DMA channel 2 configuration goes here
    DMA2_Channel2->CMAR = (uint32_t) serfifo; //&?
    DMA2_Channel2->CPAR = (uint32_t) &USART5->RDR; //&?
    DMA2_Channel2->CNDTR = FIFOSIZE;
    DMA2_Channel2->CCR &= ~DMA_CCR_DIR;
    DMA2_Channel2->CCR &= ~(DMA_CCR_HTIE | DMA_CCR_TCIE);
    DMA2_Channel2->CCR &= ~(DMA_CCR_MSIZE);
    DMA2_Channel2->CCR &= ~(DMA_CCR_PSIZE);
    DMA2_Channel2->CCR |= DMA_CCR_MINC;
    DMA2_Channel2->CCR &= ~DMA_CCR_PINC;
    DMA2_Channel2->CCR |= DMA_CCR_CIRC;
    DMA2_Channel2->CCR |= DMA_CCR_PL;
    DMA2_Channel2->CCR &= ~(DMA_CCR_MEM2MEM);

    DMA2_Channel2->CCR |= DMA_CCR_EN; // re-enable the DMA
    
}

void USART3_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}

void init_spi1_slow(){
    //Enable the RCC clock for GPIOB and SPI1
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    //SP1
    SPI1->CR1 &= ~SPI_CR1_SPE; 
    SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0; //set Baud Rate to f/256 (0 would be opposite)
    SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;   //set word size to 10 bit (?111)
    SPI1->CR2 &= ~SPI_CR2_DS_3; //set word size to 10 bit (0???)
    SPI1->CR1 |= SPI_CR1_MSTR; //puts spi1 into master selected mode
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; //turns on software slave amnagment and internal slave select
    SPI1->CR2 |= SPI_CR2_FRXTH;//sets fifo reception threshold to ieceived an 8 bit
    SPI1->CR1 |= SPI_CR1_SPE; //enable communication ending config

    //GPIOB MODER
    GPIOB->MODER |= ((1<<11) | (1<<9) | (1<<7));
    GPIOB->MODER &= ~((1<<10) | (1<<8) | (1<<6));

    //GPIOB AFR setting pb3,4,5 to 0000
    GPIOB->AFR[0] &= ~(0x00FFF000);
}

void enable_sdcard(){
    GPIOB->BRR |= (1<<2);
}

void disable_sdcard(){
    GPIOB->BSRR |= (1<<2);
}

void init_sdcard_io(){
    init_spi1_slow();
    GPIOB->MODER |= (1<<4); //configure pb2 as output
    GPIOB->MODER &= ~(1<<5);
    disable_sdcard();
}

void sdcard_io_high_speed(){
    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_BR_0;
    SPI1->CR1 &= ~(SPI_CR1_BR_1 | SPI_CR1_BR_2);
    SPI1->CR1 |= SPI_CR1_SPE;

}

void init_lcd_spi(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    //configure pb8,11,14 as output
    GPIOB->MODER |= ((1<<28) | (1<<22) | (1<<16));
    GPIOB->MODER &= ~((1<<29) | (1<<23) | (1<<17));

    init_spi1_slow();
    sdcard_io_high_speed();
}