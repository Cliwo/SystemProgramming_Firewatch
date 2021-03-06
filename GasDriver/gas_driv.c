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

#include "../heaven_header.h"

#define GAS_MAJOR_NUMBER 503
#define GAS_DEV_NAME "gas_ioctl"

#define GPIO_BASE_ADDR 0x3F200000

#define IOCTL_MAGIC_NUMBER 'g'

#define IOCTL_CMD_GET_STATUS _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_SET_FREQUENCY _IOWR(IOCTL_MAGIC_NUMBER, 1, int)


int gas_open(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "GAS driver open!!\n");
	pioInit();
	spiInit(250, 0);
	return 0;	
}

int gas_release(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "GAS driver closed!!\n");
	return 0;
}

long gas_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	int i = 0;
	int result;
	char temp;
	char test_string[6] = {'1', '2', '3', 'a', 'b', 'c'};
	
	switch(cmd) {
		case IOCTL_CMD_GET_STATUS:
			result = spiReceiveDecimal();
			printk("SPI Receive %d", result);
			return result;
		case IOCTL_SET_FREQUENCY :
			printk("SPI Set Clock 250000000/%ld", arg);
			spiSetClock(arg);
		break;
	}

	return 0;
}

static struct file_operations gas_fops = {
	.owner = THIS_MODULE,
	.open = gas_open,
	.release = gas_release,
	.unlocked_ioctl = gas_ioctl
};

int __init gas_init(void){
	if(register_chrdev(GAS_MAJOR_NUMBER, GAS_DEV_NAME, &gas_fops) <0)
		printk(KERN_ALERT "GAS driver init fail\n");
	else
	{
		printk(KERN_ALERT "GAS driver init success\n");
	}
	return 0;
}

void __exit gas_exit(void){
	unregister_chrdev(GAS_MAJOR_NUMBER, GAS_DEV_NAME);
	printk(KERN_ALERT "GAS driver exit done\n");
}

module_init(gas_init);
module_exit(gas_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FourthTeam");
MODULE_DESCRIPTION("description");
