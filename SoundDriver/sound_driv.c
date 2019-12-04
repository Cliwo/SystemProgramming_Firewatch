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

#define SOUND_MAJOR_NUMBER 505
#define SOUND_DEV_NAME "sound_ioctl"

#define GPIO_BASE_ADDR 0x3F200000

#define IOCTL_MAGIC_NUMBER 's'
#define IOCTL_CMD_COMMAND _IOWR(IOCTL_MAGIC_NUMBER, 0, int)

int sound_open(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "SOUND driver open!!\n");
	return 0;	
}

int sound_release(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "SOUND driver closed!!\n");
	return 0;
}

long sound_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static struct file_operations sound_fops = {
	.owner = THIS_MODULE,
	.open = sound_open,
	.release = sound_release,
	.unlocked_ioctl = sound_ioctl
};

int __init sound_init(void){
	if(register_chrdev(SOUND_MAJOR_NUMBER, SOUND_DEV_NAME, &sound_fops) <0)
		printk(KERN_ALERT "SOUND driver init fail\n");
	else
	{
		printk(KERN_ALERT "SOUND driver init success\n");
	}
	return 0;
}

void __exit sound_exit(void){
	unregister_chrdev(SOUND_MAJOR_NUMBER, SOUND_DEV_NAME);
	printk(KERN_ALERT "SOUND driver exit done\n");
}

module_init(sound_init);
module_exit(sound_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FourthTeam");
MODULE_DESCRIPTION("description");
