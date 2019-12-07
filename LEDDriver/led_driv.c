#include <linux/init.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/uaccess.h> 
#include <linux/slab.h> 

#include <asm/mach/map.h> 
#include <asm/uaccess.h> 

#define LED_MAJOR_NUMBER	501
#define LED_DEV_NAME		"led_dev" 

#define GPIO_BASE_ADDRESS	0x3F200000
#define PWM_BASE_ADDR		0x3F20C000
#define CLK_BASE_ADDR       0x3F101000
#define BCM_PASSWORD        0x5A000000

#define GPFSEL1     0x04
#define GPSET0  	0x1C
#define GPCLR0	    0x28
#define PWM_CTL	    0x00
#define PWM_RNG1    0x10
#define CLK_PWM_CTL 0xA0
#define CLK_PWM_DIV 0xA4

#define IOCTL_MAGIC_NUMBER  'r'
#define IOCTL_CMD_SET_LED_ON     _IO(IOCTL_MAGIC_NUMBER, 0)
#define IOCTL_CMD_SET_LED_OFF    _IO(IOCTL_MAGIC_NUMBER, 1)

static void __iomem * gpio_base;
static void __iomem * pwm_base;
volatile unsigned int * gpsel1; 
volatile unsigned int * gpset0; 
volatile unsigned int * gpclr0;
volatile unsigned int * pwm;
volatile unsigned int * clk;
volatile unsigned int * pwm_ctrl;
volatile unsigned int * pwm_rng1;

int led_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "LED driver open\n"); 

    gpio_base = ioremap(GPIO_BASE_ADDRESS, 0xFF);
    pwm_base = ioremap(PWM_BASE_ADDR, 0xFF);
    gpsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
    gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
    gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
    pwm_rng1 = (volatile unsigned int *)(pwm_base + PWM_RNG1);
    
    * gpsel1 |= (1<<24); 
    * pwm_ctrl |= (1 << 0);     // Channel 1 Enable 1: Channel is enabled
    * pwm_ctrl |= (1 << 7);     // Channel 1 M/S Enable 1: M/S transmission is used.
    * pwm_rng1 = 320;           // 

	int pwm_ctrl = pwm[PWM_CTL/4];
	pwm[PWM_CTL/4] = 0;

	clk[CLK_PWM_CTL/4] = BCM_PASSWORD | (0x01 << 5);
		
	int idiv = (int) (19200000.0f / 16000.0f);
    
    clk[CLK_PWM_DIV/4] = BCM_PASSWORD | (idiv << 12);
	clk[CLK_PWM_DIV/4] = BCM_PASSWORD | (0x11);

	pwm[PWM_CTL/4] = pwm_ctrl;	

    return 0; 
}

int led_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "LED driver close\n"); 
    iounmap((void *)gpio_base); 
    return 0; 
}

long led_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    switch (cmd){ 
        case IOCTL_CMD_SET_LED_ON: 
        printk(KERN_ALERT "setting LED ON\n");
        *gpset0 |= (0x01 << 18); 
        break; 
    
        case IOCTL_CMD_SET_LED_OFF: 
        printk(KERN_ALERT "setting LED OFF\n");
        *gpclr0 |= (0x01 << 18); 
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
