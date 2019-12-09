#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include <pthread.h>

#define INTERVAL 		30000

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#define LED_MAJOR_NUMBER 501
#define LED_MINOR_NUMBER 100
#define LED_DEV_PATH_NAME "/dev/led_ioctl"

#define BUTTON_MAJOR_NUMBER 502
#define BUTTON_MINOR_NUMBER 101
#define BUTTON_DEV_PATH_NAME "/dev/button_ioctl"

#define GAS_MAJOR_NUMBER 503
#define GAS_MINOR_NUMBER 102
#define GAS_DEV_PATH_NAME "/dev/gas_ioctl"

#define MOTOR_MAJOR_NUMBER 504
#define MOTOR_MINOR_NUMBER 103
#define MOTOR_DEV_PATH_NAME "/dev/motor_ioctl"

#define SOUND_MAJOR_NUMBER 505
#define SOUND_MINOR_NUMBER 104
#define SOUND_DEV_PATH_NAME "/dev/sound_ioctl"

#define LED_IOCTL_MAGIC_NUMBER 'j'
#define BUTTON_IOCTL_MAGIC_NUMBER 'b'
#define GAS_IOCTL_MAGIC_NUMBER 'g'
#define MOTOR_IOCTL_MAGIC_NUMBER 'm'
#define SOUND_IOCTL_MAGIC_NUMBER 's'

/* LED */
#define IOCTL_CMD_ON _IOWR(LED_IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_OFF _IOWR(LED_IOCTL_MAGIC_NUMBER, 1, int)
#define IOCTL_CMD_BRIGHT _IOWR(LED_IOCTL_MAGIC_NUMBER, 2, int)

/* BUTTON */
#define B_IOCTL_CMD_GET_STATUS _IOWR(BUTTON_IOCTL_MAGIC_NUMBER, 0, int)

/* GAS */
#define G_IOCTL_CMD_GET_STATUS _IOWR(GAS_IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_TEST _IOWR(GAS_IOCTL_MAGIC_NUMBER, 1, int)
#define G_IOCTL_SET_FREQUENCY _IOWR(GAS_IOCTL_MAGIC_NUMBER, 2, int)

/* MOTOR */
#define IOCTL_CMD_MOVE_FORWARD _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_MOVE_BACKWARD _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 1, int)
#define IOCTL_CMD_STOP_WINDING _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 2, int)

/* Sound */
#define S_IOCTL_CMD_GET_STATUS _IOWR(SOUND_IOCTL_MAGIC_NUMBER, 0, int)
#define S_IOCTL_SET_FREQUENCY _IOWR(SOUND_IOCTL_MAGIC_NUMBER, 1, int)

void init_dev(dev_t * dev, int * fd, int MAJOR_NUMBER, int MINOR_NUMBER, char * dev_path)
{
    *dev = makedev(MAJOR_NUMBER, MINOR_NUMBER);
    mknod(dev_path, S_IFCHR|0666, *dev);
    *fd = open(dev_path, O_RDWR);

    if(*fd < 0){
		printf("fail to open %s\n", dev_path);	
	}
}

int main()
{
	int i = 0, j = 0;
	dev_t led_dev, button_dev, gas_dev, motor_dev, sound_dev;
	int led_fd , button_fd, gas_fd, motor_fd, sound_fd;

	/* main code */
	
	init_dev(&led_dev, &led_fd, LED_MAJOR_NUMBER, LED_MINOR_NUMBER, LED_DEV_PATH_NAME);
	
	while(1){
		//for(i = 0; i<256; i++){
			//ioctl(led_fd, IOCTL_CMD_BRIGHT, i);
			//usleep(10000);
		//}
		for(i = 255; i >=0 ; i--){
			ioctl(led_fd, IOCTL_CMD_BRIGHT, i);
			usleep(10);
		}
	}
	
	close(led_dev);

	return 0;
}

