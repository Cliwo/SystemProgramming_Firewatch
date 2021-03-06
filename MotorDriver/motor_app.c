#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#define INTERVAL 		50000

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
#define IOCTL_CMD_SET_BRIGHTNESS _IOWR(LED_IOCTL_MAGIC_NUMBER, 2, int)

/* BUTTON */
#define B_IOCTL_CMD_GET_STATUS _IOWR(BUTTON_IOCTL_MAGIC_NUMBER, 0, int)

/* GAS */
#define G_IOCTL_CMD_GET_STATUS _IOWR(GAS_IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_TEST _IOWR(GAS_IOCTL_MAGIC_NUMBER, 1, int)

/* MOTOR */
#define IOCTL_CMD_MOVE_FORWARD _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_MOVE_BACKWARD _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 1, int)
#define IOCTL_CMD_STOP_WINDING _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 2, int)

void init_dev(dev_t * dev, int * fd, int MAJOR_NUMBER, int MINOR_NUMBER, char * dev_path)
{
    *dev = makedev(MAJOR_NUMBER, MINOR_NUMBER);
    mknod(dev_path, S_IFCHR|0666, *dev);
    *fd = open(dev_path, O_RDWR);

    if(*fd < 0){
		printf("fail to open %s\n", dev_path);
		
	}
}


void motor(int fd1){
	int i = 0;
	
	int current_button_value = 0, prev_button_value=0; 
	
	while(1){
		//ioctl(fd1, IOCTL_CMD_MOVE_FORWARD);
		//ioctl(fd1, IOCTL_CMD_MOVE_BACKWARD);
		usleep(INTERVAL);
		for(i = 0; i < 100; i++){
			ioctl(fd1, IOCTL_CMD_MOVE_BACKWARD);
		}
		for(i = 0; i < 100; i++){
			ioctl(fd1, IOCTL_CMD_MOVE_FORWARD);
		}
		
		//ioctl(fd1, IOCTL_CMD_STOP_WINDING);
	}
}

void button(int fd1, int fd2){
	
	int current_button_value = 0, prev_button_value=0; 
	int counter = 0;
	int i = 0;
	while(1)
	{
		usleep(INTERVAL);
		prev_button_value = current_button_value;
		
		current_button_value = ioctl(fd1, B_IOCTL_CMD_GET_STATUS);
		
		if (prev_button_value == 0 && current_button_value != 0) {
			printf("%d\n",counter);
			counter += 1;
		}
		if(counter == 2){
				printf("%d\n",counter);
					ioctl(fd2, IOCTL_CMD_MOVE_FORWARD);
		}
		if(counter == 3){
				printf("%d\n",counter);
				ioctl(fd2, IOCTL_CMD_STOP_WINDING);
		}
		if(counter == 4){
				printf("%d\n",counter);
				ioctl(fd2, IOCTL_CMD_MOVE_BACKWARD);
		}
		if(counter == 5){
				printf("%d\n",counter);
				ioctl(fd2, IOCTL_CMD_STOP_WINDING);
				counter = 0;
		}
	}
}

int main()
{
	dev_t led_dev, button_dev, gas_dev, motor_dev, sound_dev;
	int led_fd , button_fd, gas_fd, motor_fd, sound_fd;

	//init_dev(&led_dev, &led_fd, LED_MAJOR_NUMBER, LED_MINOR_NUMBER, LED_DEV_PATH_NAME);
	//init_dev(&button_dev, &button_fd, BUTTON_MAJOR_NUMBER, BUTTON_MINOR_NUMBER, BUTTON_DEV_PATH_NAME);
	//init_dev(&gas_dev, &gas_fd, GAS_MAJOR_NUMBER, GAS_MINOR_NUMBER, GAS_DEV_PATH_NAME);
	init_dev(&motor_dev, &motor_fd, MOTOR_MAJOR_NUMBER, MOTOR_MINOR_NUMBER, MOTOR_DEV_PATH_NAME);
	
	/* main code */
	//test_gas(gas_fd);
	//button(button_fd, motor_fd);
	//close(button_fd);
	//close(led_dev);
	//button(button_fd);
	motor(motor_fd);
	printf("Done\n");
    //close(gas_dev);
    close(motor_dev);
    //close(button_dev);
	return 0;
}
