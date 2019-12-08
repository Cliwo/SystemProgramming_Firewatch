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

//Replace _DEVICE_ with your device name ex) LED
//Replace _device_ with yourt device name ex) led

#define _DEVICE__MAJOR_NUMBER 502
#define _DEVICE__DEV_NAME "_device__ioctl"

#define GPIO_BASE_ADDR 0x3F200000

#define IOCTL_MAGIC_NUMBER 'j'
#define IOCTL_CMD_COMMAND _IOWR(IOCTL_MAGIC_NUMBER, 0, int)

int _device__open(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "_DEVICE_ driver open!!\n");
	return 0;	
}

int _device__release(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "_DEVICE_ driver closed!!\n");
	return 0;
}

long _device__ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static struct file_operations _device__fops = {
	.owner = THIS_MODULE,
	.open = _device__open,
	.release = _device__release,
	.unlocked_ioctl = _device__ioctl
};

int __init _device__init(void){
	if(register_chrdev(_DEVICE__MAJOR_NUMBER, _DEVICE__DEV_NAME, &_device__fops) <0)
		printk(KERN_ALERT "_DEVICE_ driver init fail\n");
	else
	{
		printk(KERN_ALERT "_DEVICE_ driver init success\n");
	}
	return 0;
}

void __exit _device__exit(void){
	unregister_chrdev(_DEVICE__MAJOR_NUMBER, _DEVICE__DEV_NAME);
	printk(KERN_ALERT "_DEVICE_ driver exit done\n");
}

module_init(_device__init);
module_exit(_device__exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FourthTeam");
MODULE_DESCRIPTION("description");
