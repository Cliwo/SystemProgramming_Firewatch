#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include <asm/mach/map.h>
#include <asm/io.h>
#include <asm/uaccess.h>

// GPIO FSEL Types
#define INPUT  0
#define OUTPUT 1
#define ALT0   4
#define ALT1   5
#define ALT2   6
#define ALT3   7
#define ALT4   3
#define ALT5   2

#define PWM_CLK_PASSWORD 0x5a000000
#define PWM_MASH 9
#define PWM_KILL 5
#define PWM_ENAB 4
#define PWM_SRC 0

// PWM Constants
#define PLL_FREQUENCY 500000000 // default PLLD value is 500 [MHz]
#define CM_FREQUENCY 25000000   // max pwm clk is 25 [MHz]
#define PLL_CLOCK_DIVISOR (PLL_FREQUENCY / CM_FREQUENCY)


// These #define values are specific to the BCM2835, taken from "BCM2835 ARM Peripherals"
//#define BCM2835_PERI_BASE        0x20000000
// Updated to BCM2836 for Raspberry Pi 2.0 Fall 2015 dmh
#define BCM2835_PERI_BASE        0x3F000000

#define GPIO_BASE               (BCM2835_PERI_BASE + 0x200000)
#define SPI0_BASE			    (BCM2835_PERI_BASE + 0x204000)
#define PWM_BASE			    (BCM2835_PERI_BASE + 0x20c000)
#define CM_PWM_BASE             (BCM2835_PERI_BASE + 0x101000)

volatile unsigned int *gpio; //pointer to base of gpio
volatile unsigned int *spi;  //pointer to base of spi registers
volatile unsigned int *pwm;
volatile unsigned int *cm_pwm;

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

typedef struct
{
    unsigned FSEL20      : 3;
    unsigned FSEL21      : 3;
    unsigned FSEL22      : 3;
    unsigned FSEL23      : 3;
    unsigned FSEL24      : 3;
    unsigned FSEL25      : 3;
    unsigned FSEL26      : 3;
    unsigned FSEL27      : 3;
    unsigned FSEL28      : 3;
    unsigned FSEL29      : 3;
    unsigned             : 2;
}gpfsel2bits;
#define GPFSEL2bits (* (volatile gpfsel2bits*) (gpio + 2))   
#define GPFSEL2 (* (volatile unsigned int *) (gpio + 2))                        

typedef struct
{
    unsigned FSEL30      : 3;
    unsigned FSEL31      : 3;
    unsigned FSEL32      : 3;
    unsigned FSEL33      : 3;
    unsigned FSEL34      : 3;
    unsigned FSEL35      : 3;
    unsigned FSEL36      : 3;
    unsigned FSEL37      : 3;
    unsigned FSEL38      : 3;
    unsigned FSEL39      : 3;
    unsigned             : 2;
}gpfsel3bits;
#define GPFSEL3bits (* (volatile gpfsel3bits*) (gpio + 3))   
#define GPFSEL3 (* (volatile unsigned int *) (gpio + 3))                        


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

typedef struct
{
    unsigned LEV32       : 1;
    unsigned LEV33       : 1;
    unsigned LEV34       : 1;
    unsigned LEV35       : 1;
    unsigned LEV36       : 1;
    unsigned LEV37       : 1;
    unsigned LEV38       : 1;
    unsigned LEV39       : 1;
    unsigned LEV40       : 1;
    unsigned LEV41       : 1;
    unsigned LEV42       : 1;
    unsigned LEV43       : 1;
    unsigned LEV44       : 1;
    unsigned LEV45       : 1;
    unsigned LEV46       : 1;
    unsigned LEV47       : 1;
    unsigned LEV48       : 1;
    unsigned LEV49       : 1;
    unsigned LEV50       : 1;
    unsigned LEV51       : 1;
    unsigned LEV52       : 1;
    unsigned LEV53       : 1;
    unsigned             : 10;
}gplev1bits;
#define GPLEV1bits (* (volatile gplev1bits*) (gpio + 14))   
#define GPLEV1 (* (volatile unsigned int *) (gpio + 14)) 


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


/////////////////////////////////////////////////////////////////////
// PWM Registers
/////////////////////////////////////////////////////////////////////

typedef struct
{
    unsigned PWEN1      :1;
    unsigned MODE1      :1;
    unsigned RPTL1      :1;
    unsigned SBIT1      :1;
    unsigned POLA1      :1;
    unsigned USEF1      :1;
    unsigned CLRF1      :1;
    unsigned MSEN1      :1;
    unsigned PWEN2      :1;
    unsigned MODE2      :1;
    unsigned RPTL2      :1;
    unsigned SBIT2      :1;
    unsigned POLA2      :1;
    unsigned USEF2      :1;
    unsigned            :1;
    unsigned MSEN2      :1;
    unsigned            :16;
} pwm_ctlbits;
#define PWM_CTLbits (* (volatile pwm_ctlbits *) (pwm + 0))
#define PWM_CTL (*(volatile unsigned int *) (pwm + 0))

#define PWM_RNG1 (*(volatile unsigned int *) (pwm + 4))
#define PWM_DAT1 (*(volatile unsigned int *)(pwm + 5))

/////////////////////////////////////////////////////////////////////
// Clock Manager Registers
/////////////////////////////////////////////////////////////////////

typedef struct
{
    unsigned SRC        :4;
    unsigned ENAB       :1;
    unsigned KILL       :1;
    unsigned            :1;
    unsigned BUSY       :1;
    unsigned FLIP       :1;
    unsigned MASH       :2;
    unsigned            :13;
    unsigned PASSWD     :8;
}cm_pwmctl_bits;
#define CM_PWMCTLbits (* (volatile cm_pwmctl_bits *) (cm_pwm + 40))
#define CM_PWMCTL (* (volatile unsigned int*) (cm_pwm + 40))

typedef struct
{
    unsigned DIVF       :12;
    unsigned DIVI       :12;
    unsigned PASSWD     :8;
} cm_pwmdivbits;
#define CM_PWMDIVbits (* (volatile cm_pwmdivbits *) (cm_pwm + 41))
#define CM_PWMDIV (*(volatile unsigned int *)(cm_pwm + 41)) 


// TODO: return error code instead of printing (mem_fd, reg_map)
void pioInit(void) {
	int  mem_fd;
	void *reg_map;

    gpio = ioremap(GPIO_BASE, 0x60);
    spi = ioremap(SPI0_BASE, 0x60);
    pwm = ioremap(PWM_BASE, 0x60);
    cm_pwm = ioremap(CM_PWM_BASE, 0x60);
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

/////////////////////////////////////////////////////////////////////
// PWM Functions
/////////////////////////////////////////////////////////////////////



void pwmInit(void) {
    pinMode(18, ALT5);

    // Configure the clock manager to generate a 25 MHz PWM clock.
    // Documentation on the clock manager is missing in the datasheet
    // but found in "BCM2835 Audio and PWM Clocks" by G.J. van Loo 6 Feb 2013.
    // Maximum operating frequency of PWM clock is 25 MHz.
    // Writes to the clock manager registers require simultaneous writing
    // a "password" of 5A to the top bits to reduce the risk of accidental writes.

    CM_PWMCTL = 0; // Turn off PWM before changing
    CM_PWMCTL =  PWM_CLK_PASSWORD|0x20; // Turn off clock generator
    while(CM_PWMCTLbits.BUSY); // Wait for generator to stop
    CM_PWMCTL = PWM_CLK_PASSWORD|0x206; // Src = unfiltered 500 MHz CLKD
    CM_PWMDIV = PWM_CLK_PASSWORD|(PLL_CLOCK_DIVISOR << 12); // PWM Freq = 25 MHz
    CM_PWMCTL = CM_PWMCTL|PWM_CLK_PASSWORD|0x10;    // Enable PWM clock
    while (!CM_PWMCTLbits.BUSY);    // Wait for generator to start    
    PWM_CTLbits.MSEN1 = 1;  // Channel 1 in mark/space mode
    PWM_CTLbits.PWEN1 = 1;  // Enable pwm
}

/**
 * dut is a value between 0 and 1 
 * freq is pwm frequency in Hz
 */
void setPWM(int freq, int dut) {
    PWM_RNG1 = (int)(CM_FREQUENCY / freq);
    PWM_DAT1 = (int)(dut * (CM_FREQUENCY / freq));
}

void analogWrite(int val) {
	setPWM(78125, val);
}
