#include <linux/init.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/uaccess.h> 
#include <linux/slab.h> 
#include <linux/delay.h>

#include <asm/mach/map.h> 
#include <asm/uaccess.h> 

#define LED_MAJOR_NUMBER    501
#define LED_DEV_NAME        "led_ioctl" 

#define GPIO_BASE_ADDRESS   0x3F200000
#define PWM_BASE_ADDR       0x3F20C000
#define CLK_BASE_ADDR       0x3F101000
#define BCM_PASSWORD        0x5A000000

#define GPFSEL1     0x04
#define GPSET0      0x1C
#define GPCLR0      0x28
#define PWM_CTL     0x00
#define PWM_RNG1    0x10
#define PWM_DAT1    0x14
#define CLK_PWM_CTL 0xA0
#define CLK_PWM_DIV 0xA4

#define IOCTL_MAGIC_NUMBER      'j'
#define IOCTL_CMD_SET_LED_ON    _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_SET_LED_OFF   _IOWR(IOCTL_MAGIC_NUMBER, 1, int)
#define IOCTL_CMD_BRIGHT        _IOWR(IOCTL_MAGIC_NUMBER, 2, int)

#define IOCTL_CMD_DIGITAL_WRITE_HIGH _IOWR(IOCTL_MAGIC_NUMBER, 4, int)
#define IOCTL_CMD_DIGITAL_WRITE_LOW _IOWR(IOCTL_MAGIC_NUMBER, 5, int)
#define IOCTL_CMD_PIN_MODE_HIGH _IOWR(IOCTL_MAGIC_NUMBER, 6, int)
#define IOCTL_CMD_PIN_MODE_LOW _IOWR(IOCTL_MAGIC_NUMBER, 7, int)

static void __iomem * gpio_base;
static void __iomem * pwm_base;
static void __iomem * clk_base;
volatile unsigned int * gpsel1; 
volatile unsigned int * gpset0; 
volatile unsigned int * gpclr0;
volatile unsigned int * pwm_ctl;
volatile unsigned int * pwm_rng1;
volatile unsigned int * pwm_dat1;
volatile unsigned int * clk_ctl;
volatile unsigned int * clk_div;

int led_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "LED driver open\n"); 
    
    volatile unsigned int * pwm;
    volatile unsigned int * clk;
    
    gpio_base = ioremap(GPIO_BASE_ADDRESS, 0xFF);
    pwm_base = ioremap(PWM_BASE_ADDR, 0xFF);
    clk_base = ioremap(CLK_BASE_ADDR, 0xFF);

    gpsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
    gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
    gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
    pwm_rng1 = (volatile unsigned int *)(pwm_base + PWM_RNG1);
    pwm_dat1 = (volatile unsigned int *)(pwm_base + PWM_DAT1);
    pwm_ctl = (volatile unsigned int *)(pwm_base + PWM_CTL);
    clk_ctl = (volatile unsigned int *)(clk_base + CLK_PWM_CTL);
    clk_div = (volatile unsigned int *)(clk_base + CLK_PWM_DIV);
    /*
    pwm = (volatile unsigned int *)pwm_base; 
    clk = (volatile unsigned int *)clk_base; 
    
    * gpsel1 &= ~(0x07 << 24);
    * gpsel1 |= (1 << 25); 
    
    int pwm_ctrl = *pwm_ctl;
    *pwm_ctl = 0;                                 //  store PWM control and stop PWM

    *clk_ctl = BCM_PASSWORD | (0x01 << 5);    //  stop PWM clock
   
    int idiv = (int) (19200000.0f / 16000.0f);          //  Oscilloscope to 16kHz

    *clk_div = BCM_PASSWORD | (idiv << 12);   //  integer part of divisior register
    *clk_div = BCM_PASSWORD | (0x11);         //  set source to oscilloscope & enable PWM CLK
    
    *pwm_ctl = pwm_ctrl;                          //  restore PWM control and enable PWM
    */
    *gpsel1 |= (1 << 24); //gpio 18 output
    *gpsel1 |= (1 << 6); //gpio 12 output

    return 0; 
}

int led_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "LED driver close\n"); 
    iounmap((void *)gpio_base); 
    iounmap((void *)pwm_base); 
    iounmap((void *)clk_base); 
    return 0; 
}

long led_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    int i = 0;
    int kbuf = -1;

    switch (cmd){ 
        case IOCTL_CMD_SET_LED_ON:
        printk(KERN_ALERT "setting LED%d ON\n", arg);
        *gpset0 |= (1 << arg);
        break;

        case IOCTL_CMD_SET_LED_OFF:
        printk(KERN_ALERT "setting LED%d OFF\n", arg);
        *gpclr0 |= (1 << arg);
        //*gpclr0 |= (1 << 12);
        break;
        
        case IOCTL_CMD_BRIGHT:
        * pwm_ctl |= (1 << 0);                              // Channel 1 Enable 1: Channel is enabled
        * pwm_ctl |= (1 << 7);                              // Channel 1 M/S Enable 1: M/S transmission is used.
        * pwm_rng1 = 320;  
        * pwm_dat1 = 100;
        *pwm_dat1 = arg;
        printk(KERN_ALERT "%d\n", arg);
        break; 
        
        case IOCTL_CMD_DIGITAL_WRITE_HIGH:
        printk(KERN_ALERT "Write High\n");
        digitalWrite(arg, 1);
            
        case IOCTL_CMD_DIGITAL_WRITE_LOW:
        printk(KERN_ALERT "Write Low\n");
        digitalWrite(arg, 0);
            
        case IOCTL_CMD_PIN_MODE_HIGH:
        printk(KERN_ALERT "Pin MODE High\n");
        pinMode(arg, 1);

        case IOCTL_CMD_PIN_MODE_LOW:
        printk(KERN_ALERT "Pin MODE Low\n");
        pinMode(arg, 0);
        
        default : 
        printk(KERN_ALERT "ioctl : command error\n");
    }
    
    return 0;
}

static struct file_operations led_fops = { 
    .owner = THIS_MODULE, 
    .open = led_open, 
    .release = led_release, 
    .unlocked_ioctl = led_ioctl
}; 

int __init led_init (void) { 
    if(register_chrdev(LED_MAJOR_NUMBER, LED_DEV_NAME, &led_fops) < 0)
        printk(KERN_ALERT "LED driver initalization failed\n"); 
    else 
        printk(KERN_ALERT "LED driver initalization succeed\n");

    return 0; 
}

void __exit led_exit(void){ 
    unregister_chrdev(LED_MAJOR_NUMBER, LED_DEV_NAME); 
    printk(KERN_ALERT "LED driver exit"); 
}

module_init(led_init); 
module_exit(led_exit);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("brightjun"); 
MODULE_DESCRIPTION("foo");
