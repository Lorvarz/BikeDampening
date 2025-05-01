#include "stm32f0xx.h"
#include <stdint.h>
#include "tty.h"
#include "fifo.h"
#include <stdio.h>
#include <math.h>
#include "mpu_i2c.h"
#include "ff.h"
#include "string.h"
#include <stdlib.h>
#include "SD.h"
#include "oled.h"

#include "commands.h"
#include "alarm.h"

bool SD_is_setup = false;
bool SD_write_error = false;

void write_header(const char * path){
    const char str[] = "Date/Time,IMU1 X Acceleration,IMU1 Y Acceleration,IMU1 Z Acceleration,"
    "IMU1 X Rotation,IMU1 Y Rotation,IMU1 Z Rotation,IMU2 X Acceleration,IMU2 Y Acceleration,"
    "IMU2 Z Acceleration,IMU2 X Rotation,IMU2 Y Rotation,IMU2 Z Rotation";
    // const char str[] = "hellooooo\n";
    int chk = input_IMU(str, path, 1);
    if (chk) printf("failed to write header");
}
void write_header2(const char * path){
    const char str[] = "hi";
    
    int chk = input_IMU(str, path, 1);
    if (chk) printf("failed to write header");
}
void full_data_write(const char * path, char* time, char* IMU1_x_Accel, char* IMU1_y_Accel, char* IMU1_z_Accel,
    char* IMU1_x_Rot, char* IMU1_y_Rot, char* IMU1_z_Rot,
    char* IMU2_x_Accel, char* IMU2_y_Accel, char* IMU2_z_Accel, 
    char* IMU2_x_Rot, char* IMU2_y_Rot, char* IMU2_z_Rot){
        int chk = 0;

        chk |= input_IMU(time, path, 0);

        chk |= input_IMU(IMU1_x_Accel, path, 0);
        chk |= input_IMU(IMU1_y_Accel, path, 0);
        chk |= input_IMU(IMU1_z_Accel, path, 0);

        chk |= input_IMU(IMU1_x_Rot, path, 0);
        chk |= input_IMU(IMU1_y_Rot, path, 0);
        chk |= input_IMU(IMU1_z_Rot, path, 0);

        chk |= input_IMU(IMU2_x_Accel, path, 0);
        chk |= input_IMU(IMU2_y_Accel, path, 0);
        chk |= input_IMU(IMU2_z_Accel, path, 0);

        chk |= input_IMU(IMU2_x_Rot, path, 0);
        chk |= input_IMU(IMU2_y_Rot, path, 0);
        chk |= input_IMU(IMU2_z_Rot, path, 1);


        if (chk) 
        {
            SD_write_error = true;
            printf("failed to write somewhere, %d", chk);
            return;
        }

        SD_write_error = false;
    }

void half_data_write(const char * path, char* time, char* IMU_num,char* IMU_x_Accel, char* IMU_y_Accel, 
    char* IMU_z_Accel, char* IMU_x_Rot, char* IMU_y_Rot, char* IMU_z_Rot){
        int chk = 0;
        char* zero = "";

        if (IMU_num[0] == '1'){
        
            chk |= input_IMU(time, path, 0);

            chk |= input_IMU(IMU_x_Accel, path, 0);
            chk |= input_IMU(IMU_y_Accel, path, 0);
            chk |= input_IMU(IMU_z_Accel, path, 0);

            chk |= input_IMU(IMU_x_Rot, path, 0);
            chk |= input_IMU(IMU_y_Rot, path, 0);
            chk |= input_IMU(IMU_z_Rot, path, 1);

            // chk |= input_IMU(zero, path, 0);
            // chk |= input_IMU(zero, path, 0);
            // chk |= input_IMU(zero, path, 0);

            // chk |= input_IMU(zero, path, 0);
            // chk |= input_IMU(zero, path, 0);
            // chk |= input_IMU(zero, path, 1);
        }
        else {
            chk |= input_IMU(time, path, 0);

            chk |= input_IMU(zero, path, 0);
            chk |= input_IMU(zero, path, 0);
            chk |= input_IMU(zero, path, 0);

            chk |= input_IMU(zero, path, 0);
            chk |= input_IMU(zero, path, 0);
            chk |= input_IMU(zero, path, 0);

            chk |= input_IMU(IMU_x_Accel, path, 0);
            chk |= input_IMU(IMU_y_Accel, path, 0);
            chk |= input_IMU(IMU_z_Accel, path, 0);

            chk |= input_IMU(IMU_x_Rot, path, 0);
            chk |= input_IMU(IMU_y_Rot, path, 0);
            chk |= input_IMU(IMU_z_Rot, path, 1);
        }

        if (chk) 
        {
            SD_write_error = true;
            printf("failed to write somewhere, %d", chk);
            return;
        }

        SD_write_error = false;

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
    SD_is_setup = true;
}

__attribute((weak)) char interrupt_getchar() {
    // Wait for a newline to complete the buffer.
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi"); // wait for an interrupt
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}

__attribute((weak)) int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    return (int) interrupt_getchar();
}

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

__attribute((weak)) int __io_putchar(int c) {
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

// checks if the SD card's connection is stable
bool inline SDStable()
{
    return !SD_is_setup || 
            (!SD_write_error && (!(USART5->ISR & USART_ISR_TEACK) || !(USART5->ISR & USART_ISR_REACK)))
           ;
}

int imu_val_update(int raw){
    raw = raw * 1000;
   int bit_const = 16384; // 2^15/2 or 2^14
   int grav_const = 10; // m/s^2
   int div = bit_const * grav_const;
   return (round(raw/ div));

}

char* int_to_str(int read){
    //Check sign of raw data and remove for now
    int sign = 1;
    if (read < 1) sign = -1;
    read = abs(read);
    
    //Get the number of digits to write
    int dig_len;
    if (read < 1)
        dig_len = 1;
    else
        dig_len = log10(read) + 1;
    
    //Allocate space for the data to write (dig_len + sign + null)
    char* str = malloc( (dig_len + 2) * sizeof(char));
    for( int i = dig_len; i > 0; i--){
        int j = i;
        int k = 1;
        while (j > 1){
            k *= 10;
            j--;
        }
        int temp = (read / k) % 10;
        str[dig_len - i + 1] = temp + '0';
    }
    str[dig_len + 1] = '\0';
    if (str == NULL) str[0] = 0;
    if (sign == 1) str[0] = ' ';
    else str[0] = '-';
    return (str);
}

void set_sd_stream(){
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
}
void TIM2_50ms_Init(void)
{
    // Enable peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Timing (psc arr and cnt)
    TIM2->PSC = (uint16_t)(48000000U / 48000U - 1U); //PSC = (48 000 000 / 48 000) – 1 = 999
    TIM2->ARR = 20U - 1U;                                 /* period     */
    TIM2->CNT = 0;                                        /* reset CNT  */

    
    TIM2->DIER |= TIM_DIER_UIE; //Enable update interrupt

    TIM2->CR1  |= TIM_CR1_CEN; //start counter

    // NVIC setup
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);
}


void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;  
	spi1_display2("here");            //IMU 2 Rotation prints
    mpu6050_init(0x68);
    mpu6050_init(0x69);     
    AccelData wheel, fork;
    if (1)          // check update flag set?          
    {
        TIM2->SR &= ~TIM_SR_UIF;        // clear it (write 0)


        //Rotation data not collected
        //Wheel is defined as IMU 1
        //Fork is defined as IMU 2
        //mpu_read_accel(0x68, &wheel);
        //mpu_read_accel(0x69, &fork);
        uint16_t wheel_add = (0x68);
        uint16_t fork_add = (0x69);

        mpu_read_accel(wheel_add, &wheel);
        mpu_read_accel(fork_add, &fork);

        char* IMU1_x_Accel = int_to_str(imu_val_update(wheel.ax));
        char* IMU1_y_Accel = int_to_str(imu_val_update(wheel.ay));
        char* IMU1_z_Accel = int_to_str(imu_val_update(wheel.az));
        char* IMU2_x_Accel = int_to_str(imu_val_update(fork.ax));
        char* IMU2_y_Accel = int_to_str(imu_val_update(fork.ay));
        char* IMU2_z_Accel = int_to_str(imu_val_update(fork.az));
        
        char* time = "-";
        char* IMU1_x_Rot = "-";
        char* IMU1_y_Rot = "-";
        char* IMU1_z_Rot = "-";
        char* IMU2_x_Rot = "-";
        char* IMU2_y_Rot = "-";
        char* IMU2_z_Rot = "-";
        // char* str1 = " %c%c%c%c %c%c%c%c %c%c%c%c ", IMU1_x_Accel[0], IMU1_x_Accel[1], IMU1_x_Accel[2], IMU1_x_Accel[3],
		// 	IMU1_y_Accel[0], IMU1_y_Accel[1], IMU1_y_Accel[2], IMU1_y_Accel[3],
		// 	IMU1_z_Accel[0], IMU1_z_Accel[1], IMU1_z_Accel[2], IMU1_z_Accel[3];
		// char* str2 = " %c%c%c%c %c%c%c%c %c%c%c%c ", IMU2_x_Accel[0], IMU2_x_Accel[1], IMU2_x_Accel[2], IMU2_x_Accel[3],
		// 	IMU2_y_Accel[0], IMU2_y_Accel[1], IMU2_y_Accel[2], IMU2_y_Accel[3],
		// 	IMU2_z_Accel[0], IMU2_z_Accel[1], IMU2_z_Accel[2], IMU2_z_Accel[3];
        
        // spi1_display2(str2);
        printf("MPU @ 0x68: %s, %s, %s\n", IMU1_x_Accel, IMU1_y_Accel, IMU1_z_Accel);
        printf("MPU @ 0x69: %s, %s, %s\n", IMU2_x_Accel, IMU2_y_Accel, IMU2_z_Accel);
        full_data_write("test.csv", time,                   //IMU 1+2 prints
        IMU1_x_Accel, IMU1_y_Accel, IMU1_z_Accel,           //IMU 1 Acceleration prints
        IMU1_x_Rot, IMU1_y_Rot, IMU1_z_Rot,                 //IMU 1 Rotation prints
        IMU2_x_Accel, IMU2_y_Accel, IMU2_z_Accel,           //IMU 2 Acceleration prints
        IMU2_x_Rot, IMU2_y_Rot, IMU2_z_Rot);    

		// char* str1 = str_to_disp(IMU1_x_Accel, IMU1_y_Accel, IMU1_z_Accel);
		// spi1_display2(str1);            //IMU 2 Rotation prints                
    }
}

void SD_setup(char* fn){
    FATFS fstorage;
    FATFS* fs = &fstorage;

    //Mounting cmds
    FRESULT res = f_mount(NULL, "", 1); // make sure unmounted
    res = f_mount(fs, "", 1); //mount
    if (res != FR_OK){
        print_error(res, "Error occurred while mounting");
    }

    // //RM previous test file
    // res = f_unlink(fn);
    //     if (res != FR_OK && res != FR_NO_FILE)
    //         print_error(res, fn);

    
}

char* str_to_disp(char* x, char* y, char* z){
	char* str = malloc(16 * sizeof(char));
	char str_temp[] = "                ";
	strcpy(str, str_temp);
	int len;
	if (strlen(x) > 4) len = 4;
	else len = strlen(x);
	for(int i = len; i > 0; i--){
		char temp = x[len - i];
		char temp_2 = str[5 - i];
		str[5 - i] = x[len - i];
		// str[5 - i] = 'A';
		// int a = 1;
	}
	if (strlen(y) > 4) len = 4;
	else len = strlen(y);
	for(int i = len; i > 0; i--){
		str[11 - i] = y[len - i];
	}
	if (strlen(z) > 4) len = 4;
	else len = strlen(z);
	for(int i = len; i > 0; i--){
		str[15 - i] = z[len - i];
	}
	return str;
}