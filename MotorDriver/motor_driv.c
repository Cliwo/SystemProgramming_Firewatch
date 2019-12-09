#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include "heaven_header.h"

#include <asm/mach/map.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define MOTOR_MAJOR_NUMBER 504
#define MOTOR_DEV_NAME "motor_ioctl"

#define GPIO_BASE_ADDR 0x3F200000

#define IOCTL_MAGIC_NUMBER 'm'

#define MOTOR_STATUS_HOLT 0
#define MOTOR_STATUS_IS_WINDING 1
#define IOCTL_CMD_MOVE_FORWARD _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_MOVE_BACKWARD _IOWR(IOCTL_MAGIC_NUMBER, 1, int)
#define IOCTL_CMD_STOP_WINDING _IOWR(IOCTL_MAGIC_NUMBER, 2, int)

int motor_open(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "MOTOR driver open!!\n");
	pioInit();
	//pwmInit();
	return 0;	
}

int motor_release(struct inode *inode, struct file *flip){
	printk(KERN_ALERT "MOTOR driver closed!!\n");
	return 0;
}

void setsteps(int w1, int w2, int w3, int w4)
{
    pinMode(13,OUTPUT);
    digitalWrite(13,w1);
    pinMode(19,OUTPUT);
    digitalWrite(19,w2);
    pinMode(26,OUTPUT);
    digitalWrite(26,w3);
    pinMode(21,OUTPUT);
    digitalWrite(21,w4);
}

 void forward (int del,int steps)
{
    int i;
    for(i=0;i<=steps;i++)
    {
	setsteps(1,1,0,0);
	ndelay(del);
	setsteps(0,1,1,0);
	ndelay(del);
	setsteps(0,0,1,1);
	ndelay(del);
	setsteps(1,0,0,1);
	ndelay(del);
    }
}

void backward (int del,int steps)
{
      int k;
      for(k=0;k<=steps;k++)
      {
	  setsteps(1,0,0,1);
	  ndelay(del);
	  setsteps(0,0,1,1);
          ndelay(del);
	  setsteps(0,1,1,0);
          ndelay(del);
          setsteps(1,1,0,0);
          ndelay(del);
      }
}
      
void stop(void){
	setsteps(0,0,0,0);
	pinMode(13,INPUT);
         pinMode(19,INPUT);
         pinMode(26,INPUT);
         pinMode(21,INPUT);
}


long motor_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
  int kbuf = 0;
	switch(cmd) {
		case IOCTL_CMD_MOVE_FORWARD:
		  printk(KERN_ALERT "move forward!!\n");
		  forward(5000000, 1);
		break;
		case IOCTL_CMD_MOVE_BACKWARD:
		    printk(KERN_ALERT "move backward!!\n");
		    backward(5000000, 1);
		break;
		case IOCTL_CMD_STOP_WINDING:
		  stop();
		break;
	}
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
