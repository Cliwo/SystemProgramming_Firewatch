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

#define LED_MAJOR_NUMBER 501
#define LED_DEV_NAME "led_ioctl"

#define GPIO_BASE_ADDR 0x3F200000
#define GPFSEL1 0x04
#define GPSET0 0x1C
#define GPCLR0 0x28

#define IOCTL_MAGIC_NUMBER 'l'
#define IOCTL_CMD_ON _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_OFF _IOWR(IOCTL_MAGIC_NUMBER, 1, int)
#define IOCTL_CMD_SET_BRIGHTNESS _IOWR(IOCTL_MAGIC_NUMBER, 2, int)


static void __iomem *gpio_base;
volatile unsigned int *gpsel1;
volatile unsigned int *gpset1;
volatile unsigned int *gpclr1;

int led_state;

int led_open(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "LED driver open!!\n");
	
	gpio_base = ioremap(GPIO_BASE_ADDR, 0x60);
	gpsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
	gpset1 = (volatile unsigned int *)(gpio_base + GPSET0);
	gpclr1 = (volatile unsigned int *)(gpio_base +GPCLR0);
	
	*gpsel1 |= (1<<24); //LED set direction out
	return 0;	
}

int led_release(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "LED driver closed!!\n");
	iounmap((void*)gpio_base);
	return 0;
}

long led_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	int kbuf = -1;
	int i = 0;
	
	switch(cmd) {
		case IOCTL_CMD_ON:
			printk(KERN_ALERT "LED on!!\n");
			*gpset1 |= (1<<18);
			break;
		case IOCTL_CMD_OFF:
			printk(KERN_ALERT "LED off!!\n");
			*gpclr1 |= (1<<18);
			break;
		case IOCTL_CMD_SET_BRIGHTNESS:
			/* TO BE IMPLEMENTED */
			break;
		}
	return 0;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_release,
	.unlocked_ioctl = led_ioctl
};

int __init led_init(void){
	if(register_chrdev(LED_MAJOR_NUMBER, LED_DEV_NAME, &led_fops) <0)
		printk(KERN_ALERT "LED driver init fail\n");
	else
	{
		*gpclr1 |= (1<<18);
		led_state = 0;
		printk(KERN_ALERT "LED driver init success\n");
	}
	return 0;
}

void __exit led_exit(void){
	unregister_chrdev(LED_MAJOR_NUMBER, LED_DEV_NAME);
	printk(KERN_ALERT "LED driver exit done\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FourthTeam");
MODULE_DESCRIPTION("description");

