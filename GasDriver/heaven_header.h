#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>

// GPIO FSEL Types
#define INPUT  0
#define OUTPUT 1
#define ALT0   4
#define ALT1   5
#define ALT2   6
#define ALT3   7
#define ALT4   3
#define ALT5   2

// These #define values are specific to the BCM2835, taken from "BCM2835 ARM Peripherals"
//#define BCM2835_PERI_BASE        0x20000000
// Updated to BCM2836 for Raspberry Pi 2.0 Fall 2015 dmh
#define BCM2835_PERI_BASE        0x3F000000

#define GPIO_BASE               (BCM2835_PERI_BASE + 0x200000)
#define SPI0_BASE			    (BCM2835_PERI_BASE + 0x204000)

volatile unsigned int *gpio; //pointer to base of gpio
volatile unsigned int *spi;  //pointer to base of spi registers


// Function Select
#define GPFSEL    ((volatile unsigned int *) (gpio + 0))
typedef struct
{
    unsigned FSEL0      : 3;
    unsigned FSEL1      : 3;
    unsigned FSEL2      : 3;
    unsigned FSEL3      : 3;
    unsigned FSEL4      : 3;
    unsigned FSEL5      : 3;
    unsigned FSEL6      : 3;
    unsigned FSEL7      : 3;
    unsigned FSEL8      : 3;
    unsigned FSEL9      : 3;
    unsigned            : 2;
}gpfsel0bits;
#define GPFSEL0bits (*(volatile gpfsel0bits*) (gpio + 0))   
#define GPFSEL0 (*(volatile unsigned int*) (gpio + 0))

typedef struct
{
    unsigned FSEL10      : 3;
    unsigned FSEL11      : 3;
    unsigned FSEL12      : 3;
    unsigned FSEL13      : 3;
    unsigned FSEL14      : 3;
    unsigned FSEL15      : 3;
    unsigned FSEL16      : 3;
    unsigned FSEL17      : 3;
    unsigned FSEL18      : 3;
    unsigned FSEL19      : 3;
    unsigned             : 2;
}gpfsel1bits;
#define GPFSEL1bits (*(volatile gpfsel1bits*) (gpio + 1))   
#define GPFSEL1 (*(volatile unsigned int*) (gpio + 1))


// Pin Output Select
#define GPSET    ((volatile unsigned int *) (gpio + 7))
typedef struct
{
    unsigned SET0       : 1;
    unsigned SET1       : 1;
    unsigned SET2       : 1;
    unsigned SET3       : 1;
    unsigned SET4       : 1;
    unsigned SET5       : 1;
    unsigned SET6       : 1;
    unsigned SET7       : 1;
    unsigned SET8       : 1;
    unsigned SET9       : 1;
    unsigned SET10      : 1;
    unsigned SET11      : 1;
    unsigned SET12      : 1;
    unsigned SET13      : 1;
    unsigned SET14      : 1;
    unsigned SET15      : 1;
    unsigned SET16      : 1;
    unsigned SET17      : 1;
    unsigned SET18      : 1;
    unsigned SET19      : 1;
    unsigned SET20      : 1;
    unsigned SET21      : 1;
    unsigned SET22      : 1;
    unsigned SET23      : 1;
    unsigned SET24      : 1;
    unsigned SET25      : 1;
    unsigned SET26      : 1;
    unsigned SET27      : 1;
    unsigned SET28      : 1;
    unsigned SET29      : 1;
    unsigned SET30      : 1;
    unsigned SET31      : 1;
}gpset0bits;
#define GPSET0bits (* (volatile gpset0bits*) (gpio + 7))   
#define GPSET0 (* (volatile unsigned int *) (gpio + 7)) 


// Pin Output Clear
#define GPCLR    ((volatile unsigned int *) (gpio + 10))
typedef struct
{
    unsigned CLR0       : 1;
    unsigned CLR1       : 1;
    unsigned CLR2       : 1;
    unsigned CLR3       : 1;
    unsigned CLR4       : 1;
    unsigned CLR5       : 1;
    unsigned CLR6       : 1;
    unsigned CLR7       : 1;
    unsigned CLR8       : 1;
    unsigned CLR9       : 1;
    unsigned CLR10      : 1;
    unsigned CLR11      : 1;
    unsigned CLR12      : 1;
    unsigned CLR13      : 1;
    unsigned CLR14      : 1;
    unsigned CLR15      : 1;
    unsigned CLR16      : 1;
    unsigned CLR17      : 1;
    unsigned CLR18      : 1;
    unsigned CLR19      : 1;
    unsigned CLR20      : 1;
    unsigned CLR21      : 1;
    unsigned CLR22      : 1;
    unsigned CLR23      : 1;
    unsigned CLR24      : 1;
    unsigned CLR25      : 1;
    unsigned CLR26      : 1;
    unsigned CLR27      : 1;
    unsigned CLR28      : 1;
    unsigned CLR29      : 1;
    unsigned CLR30      : 1;
    unsigned CLR31      : 1;
}gpclr0bits;
#define GPCLR0bits (* (volatile gpclr0bits*) (gpio + 10))   
#define GPCLR0 (* (volatile unsigned int *) (gpio + 10)) 


// Pin Level
#define GPLEV    ((volatile unsigned int *) (gpio + 13))
typedef struct
{
    unsigned LEV0       : 1;
    unsigned LEV1       : 1;
    unsigned LEV2       : 1;
    unsigned LEV3       : 1;
    unsigned LEV4       : 1;
    unsigned LEV5       : 1;
    unsigned LEV6       : 1;
    unsigned LEV7       : 1;
    unsigned LEV8       : 1;
    unsigned LEV9       : 1;
    unsigned LEV10      : 1;
    unsigned LEV11      : 1;
    unsigned LEV12      : 1;
    unsigned LEV13      : 1;
    unsigned LEV14      : 1;
    unsigned LEV15      : 1;
    unsigned LEV16      : 1;
    unsigned LEV17      : 1;
    unsigned LEV18      : 1;
    unsigned LEV19      : 1;
    unsigned LEV20      : 1;
    unsigned LEV21      : 1;
    unsigned LEV22      : 1;
    unsigned LEV23      : 1;
    unsigned LEV24      : 1;
    unsigned LEV25      : 1;
    unsigned LEV26      : 1;
    unsigned LEV27      : 1;
    unsigned LEV28      : 1;
    unsigned LEV29      : 1;
    unsigned LEV30      : 1;
    unsigned LEV31      : 1;
}gplev0bits;
#define GPLEV0bits (* (volatile gplev0bits*) (gpio + 13))   
#define GPLEV0 (* (volatile unsigned int *) (gpio + 13)) 


/////////////////////////////////////////////////////////////////////
// SPI Registers
/////////////////////////////////////////////////////////////////////

typedef struct
{
	unsigned CS 		:2;
	unsigned CPHA		:1;
	unsigned CPOL		:1;
	unsigned CLEAR 		:2;
	unsigned CSPOL		:1;
	unsigned TA 		:1;
	unsigned DMAEN		:1;
	unsigned INTD 		:1;
	unsigned INTR 		:1;
	unsigned ADCS		:1;
	unsigned REN 		:1;
	unsigned LEN 		:1;
	unsigned LMONO 		:1;
	unsigned TE_EN		:1;
	unsigned DONE		:1;
	unsigned RXD		:1;
	unsigned TXD		:1;
	unsigned RXR 		:1;
	unsigned RXF 		:1;
	unsigned CSPOL0 	:1;
	unsigned CSPOL1 	:1;
	unsigned CSPOL2 	:1;
	unsigned DMA_LEN	:1;
	unsigned LEN_LONG	:1;
	unsigned 			:6;
}spi0csbits;
#define SPI0CSbits (* (volatile spi0csbits*) (spi + 0))   
#define SPI0CS (* (volatile unsigned int *) (spi + 0))

#define SPI0FIFO (* (volatile unsigned int *) (spi + 1))
#define SPI0CLK (* (volatile unsigned int *) (spi + 2))
#define SPI0DLEN (* (volatile unsigned int *) (spi + 3))

// TODO: return error code instead of printing (mem_fd, reg_map)
void pioInit(void) {
	int  mem_fd;
	void *reg_map;

    gpio = ioremap(GPIO_BASE, 0x60);
    spi = ioremap(SPI0_BASE, 0x60);
}

void pinMode(int pin, int function) {
    int reg      =  pin/10;
    int offset   = (pin%10)*3;
    GPFSEL[reg] &= ~((0b111 & ~function) << offset);
    GPFSEL[reg] |=  ((0b111 &  function) << offset);
}

void digitalWrite(int pin, int val) {
    int reg = pin / 32;
    int offset = pin % 32;

    if (val) GPSET[reg] = 1 << offset;
    else     GPCLR[reg] = 1 << offset;
}

int digitalRead(int pin) {
    int reg = pin / 32;
    int offset = pin % 32;

    return (GPLEV[reg] >> offset) & 0x00000001;
}


/////////////////////////////////////////////////////////////////////
// SPI Functions
/////////////////////////////////////////////////////////////////////

void spiInit(int freq, int settings) {
    //set GPIO 8 (CE), 9 (MISO), 10 (MOSI), 11 (SCLK) alt fxn 0 (SPI0)
    pinMode(8, ALT0);
    pinMode(9, ALT0);
    pinMode(10, ALT0);
    pinMode(11, ALT0);

    //Note: clock divisor will be rounded to the nearest power of 2
    SPI0CLK = 250000000/freq;   // set SPI clock to 250MHz / freq
    SPI0CS = settings;          
    SPI0CSbits.TA = 1;          // turn SPI on with the "transfer active" bit
}

char spiReceive(void)
{
    return SPI0FIFO;
}
char spiSetClock (int freq)
{
    SPI0CLK = 250000000/freq;
}
char spiSendReceive(char send){
    SPI0FIFO = send;            // send data to slave
    while(!SPI0CSbits.DONE);    // wait until SPI transmission complete
    return SPI0FIFO;            // return received data
}

short spiSendReceive16(short send) {
    short rec;
    SPI0CSbits.TA = 1;          // turn SPI on with the "transfer active" bit
    rec = spiSendReceive((send & 0xFF00) >> 8); // send data MSB first
    rec = (rec << 8) | spiSendReceive(send & 0xFF);
    SPI0CSbits.TA = 0;          // turn off SPI
    return rec;
}

int spiReceiveDecimal(void) {
    int result;
    char buffer[3] = {1}; 
    buffer[1] = (8) << 4; //8 is magic number. please refer to 'http://shaunsbennett.com/piblog/?p=266'
    //original value was channelConfig+analogChannel

    SPI0CSbits.TA = 1;          // turn SPI on with the "transfer active" bit
    buffer[0] = spiSendReceive(buffer[0]);
    buffer[1] = spiSendReceive(buffer[1]);
    buffer[2] = spiSendReceive(buffer[2]);
    SPI0CSbits.TA = 0;          // turn off SPI
    
    result = ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
    
    return result;
}
