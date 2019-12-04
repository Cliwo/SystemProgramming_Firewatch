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

#define BUTTON_MAJOR_NUMBER 502
#define BUTTON_DEV_NAME "button_ioctl"

#define GPIO_BASE_ADDR 0x3F200000

#define IOCTL_MAGIC_NUMBER 'j'
#define IOCTL_CMD_GET_STATUS _IOWR(IOCTL_MAGIC_NUMBER, 0, int)

int button_open(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "BUTTON driver open!!\n");
	return 0;	
}

int button_release(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "BUTTON driver closed!!\n");
	return 0;
}

long button_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
		case IOCTL_CMD_GET_STATUS:
			/* TO BE IMPLEMENTED */
			return 0; // return some value
		break;
	}

	return 0;
}

static struct file_operations button_fops = {
	.owner = THIS_MODULE,
	.open = button_open,
	.release = button_release,
	.unlocked_ioctl = button_ioctl
};

int __init button_init(void){
	if(register_chrdev(BUTTON_MAJOR_NUMBER, BUTTON_DEV_NAME, &button_fops) <0)
		printk(KERN_ALERT "BUTTON driver init fail\n");
	else
	{
		printk(KERN_ALERT "BUTTON driver init success\n");
	}
	return 0;
}

void __exit button_exit(void){
	unregister_chrdev(BUTTON_MAJOR_NUMBER, BUTTON_DEV_NAME);
	printk(KERN_ALERT "BUTTON driver exit done\n");
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FourthTeam");
MODULE_DESCRIPTION("description");
