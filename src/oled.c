#include "stm32f0xx.h"
void init_spi1() {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //enable clock to spi1
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //enable clock to GPIOA


    GPIOA->MODER |= 0x80008800; //set p15,7,5 to alternate function
    GPIOA->MODER &= 0xBFFFBBFF; //cont

    GPIOA->AFR[1] &=  ~(0xF0000000); //PA15 alternate set to AF0 (0000)
    GPIOA->AFR[0] &=  ~(0xF0000000); //PA7 alt set to AF0 (0000) SPI1 MOSI
    GPIOA->AFR[0] &=  ~(0x00F00000); //PA5 alt set to AF0 (0000) SPI1 SCK


    SPI1->CR1 &= ~SPI_CR1_SPE; //disable communication for configuration
    SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0; //set Baud Rate to f/256 (0 would be opposite)
    SPI1->CR2 |= SPI_CR2_DS_Msk;   //set word size to 10 bit (1??1)
    SPI1->CR2 &= ~SPI_CR2_DS_2 & ~SPI_CR2_DS_1; //set word size to 10 bit (?00?)
    SPI1->CR1 |= SPI_CR1_MSTR; //puts spi1 into master selected mode
    SPI1->CR2 |= SPI_CR2_NSSP; //enables NSS pusle managment
    SPI1->CR2 |= SPI_CR2_SSOE; //enable SS output 
    SPI1->CR2 |= SPI_CR2_TXDMAEN; //tx buffer empty causes dma enable
    SPI1->CR1 |= SPI_CR1_SPE; //enable communication ending config

}
void init_spi2() {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; //enable clock to spi1 
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //enable clock to GPIOA


    GPIOB->MODER |= 0x8A000000; //set p15,13,12 to alternate function
    GPIOB->MODER &= 0xB5FFFFFF; //cont

    GPIOB->AFR[1] &=  ~(0xF0FF0000); //PA15 alternate set to AF0 (0000)
    // GPIOA->AFR[0] &=  ~(0xF0000000); //PA7 alt set to AF0 (0000) SPI1 MOSI
    // GPIOA->AFR[0] &=  ~(0x00F00000); //PA5 alt set to AF0 (0000) SPI1 SCK


    SPI2->CR1 &= ~SPI_CR1_SPE; //disable communication for configuration
    SPI2->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0; //set Baud Rate to f/256 (0 would be opposite)
    SPI2->CR2 |= SPI_CR2_DS_Msk;   //set word size to 10 bit (1??1)
    SPI2->CR2 &= ~SPI_CR2_DS_2 & ~SPI_CR2_DS_1; //set word size to 10 bit (?00?)
    SPI2->CR1 |= SPI_CR1_MSTR; //puts spi1 into master selected mode
    SPI2->CR2 |= SPI_CR2_NSSP; //enables NSS pusle managment
    SPI2->CR2 |= SPI_CR2_SSOE; //enable SS output 
    SPI2->CR2 |= SPI_CR2_TXDMAEN; //tx buffer empty causes dma enable
    SPI2->CR1 |= SPI_CR1_SPE; //enable communication ending config

}

void spi1_init_oled() {
    spi_cmd(0x38); //sets oled "function set"
    spi_cmd(0x08); // turns off display
    spi_cmd(0x01); //clears display
    nano_wait(2000000);
    spi_cmd(0x06); //set entry mode (right to left)
    spi_cmd(0x02); //set cursor to beginning
    spi_cmd(0x0c); //turns on display
}

void spi1_display1(const char *string) {
    spi_cmd(0x02);               //set cursor to beginning
    while (*string != '\0'){      //loops through string until null is reached
        spi_data(*string);        //writes each character to oled
        string = string + 1; //iterates to next char in string
    }
}
void spi1_display2(const char *string) {
    spi_cmd(0xc0);               //sets cursor to second row
    while (*string != '\0'){      //loops through string until null is reached
        spi_data(*string);        //writes each character to oled
        string = string + 1; //iterates to next char in string
    }
}

void spi_cmd(unsigned int data) {
    // if (~(SPI1->SR & SPI_SR_TXE)){
    //     nano_wait(500);
    // }
    while(!(SPI1->SR & SPI_SR_TXE)){}
    SPI1->DR = data;
}
void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
}
