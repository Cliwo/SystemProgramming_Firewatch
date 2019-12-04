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

#define MOTOR_MAJOR_NUMBER 504
#define MOTOR_DEV_NAME "motor_ioctl"

#define GPIO_BASE_ADDR 0x3F200000

#define IOCTL_MAGIC_NUMBER 'j'
#define IOCTL_CMD_COMMAND _IOWR(IOCTL_MAGIC_NUMBER, 0, int)

int motor_open(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "MOTOR driver open!!\n");
	return 0;	
}

int motor_release(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "MOTOR driver closed!!\n");
	return 0;
}

long motor_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static struct file_operations motor_fops = {
	.owner = THIS_MODULE,
	.open = motor_open,
	.release = motor_release,
	.unlocked_ioctl = motor_ioctl
};

int __init motor_init(void){
	if(register_chrdev(MOTOR_MAJOR_NUMBER, MOTOR_DEV_NAME, &motor_fops) <0)
		printk(KERN_ALERT "MOTOR driver init fail\n");
	else
	{
		printk(KERN_ALERT "MOTOR driver init success\n");
	}
	return 0;
}

void __exit motor_exit(void){
	unregister_chrdev(MOTOR_MAJOR_NUMBER, MOTOR_DEV_NAME);
	printk(KERN_ALERT "MOTOR driver exit done\n");
}

module_init(motor_init);
module_exit(motor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FourthTeam");
MODULE_DESCRIPTION("description");
