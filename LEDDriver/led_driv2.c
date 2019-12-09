#include <linux/init.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/uaccess.h> 
#include <linux/slab.h> 
#include <linux/delay.h>

#include "heaven_header.h"

#include <asm/mach/map.h> 
#include <asm/uaccess.h> 

#define LED_MAJOR_NUMBER	501
#define LED_DEV_NAME		"led_ioctl" 

#define GPIO_BASE_ADDRESS	0x3F200000
#define PWM_BASE_ADDR		0x3F20C000
#define CLK_BASE_ADDR       0x3F101000
#define BCM_PASSWORD        0x5A000000

#define GPFSEL1     0x04
#define GPSET0  	0x1C
#define GPCLR0	    0x28
#define PWM_CTL	    0x00
#define PWM_RNG1    0x10
#define PWM_DAT1    0x14
#define CLK_PWM_CTL 0xA0
#define CLK_PWM_DIV 0xA4

#define IOCTL_MAGIC_NUMBER  'j'
#define IOCTL_CMD_SET_LED_ON     _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_BRIGHT    _IOWR(IOCTL_MAGIC_NUMBER, 1, int)

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
    pioInit();
    pwmInit();
    return 0; 
}

int led_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "LED driver close\n"); 
    //iounmap((void *)gpio_base); 
    //iounmap((void *)pwm_base); 
    //iounmap((void *)clk_base); 
    return 0; 
}

long led_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    int i = 0;
    switch (cmd){ 
        case IOCTL_CMD_SET_LED_ON: 
        printk(KERN_ALERT "setting LED ON\n");
        *gpsel1 |= (1 << 24); 
        *gpset0 |= (0x01 << 18);
        break;
    
        case IOCTL_CMD_BRIGHT:
        analogWrite(arg/255);
        printk(KERN_ALERT "%d\n", arg);
        break; 

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
