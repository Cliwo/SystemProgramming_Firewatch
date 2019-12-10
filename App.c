#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>

#define INTERVAL 		30000

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

/* Dirty */


// MAX_PINS:
//    This is more than the number of Pi pins because we can actually softPwm.
//    Once upon a time I let pins on gpio expanders be softPwm'd, but it's really
//    really not a good thing.
#define PWM_RANGE   100
#define MAX_PINS    32
#define GPIO_18_IN_PIN_NUM 18

#define IOCTL_MAGIC_NUMBER      'j'
#define IOCTL_CMD_DIGITAL_WRITE_HIGH _IOWR(IOCTL_MAGIC_NUMBER, 4, int)
#define IOCTL_CMD_DIGITAL_WRITE_LOW _IOWR(IOCTL_MAGIC_NUMBER, 5, int)
#define IOCTL_CMD_PIN_MODE_HIGH _IOWR(IOCTL_MAGIC_NUMBER, 6, int)
#define IOCTL_CMD_PIN_MODE_LOW _IOWR(IOCTL_MAGIC_NUMBER, 7, int)

// The PWM Frequency is derived from the "pulse time" below. Essentially,
//    the frequency is a function of the range and this pulse time.
//    The total period will be range * pulse time in µS, so a pulse time
//    of 100 and a range of 100 gives a period of 100 * 100 = 10,000 µS
//    which is a frequency of 100Hz.
//
//    It's possible to get a higher frequency by lowering the pulse time,
//    however CPU uage will skyrocket as wiringPi uses a hard-loop to time
//    periods under 100µS - this is because the Linux timer calls are just
//    not accurate at all, and have an overhead.
//
//    Another way to increase the frequency is to reduce the range - however
//    that reduces the overall output accuracy...

#define    PULSE_TIME    100

static volatile int marks         [MAX_PINS] ;
static volatile int range         [MAX_PINS] ;
static volatile pthread_t threads [MAX_PINS] ;
static volatile int newPin = -1 ;

static int led_fd_SOFT;

void digitalWriteHIGH()
{
    ioctl(led_fd_SOFT, IOCTL_CMD_DIGITAL_WRITE_HIGH, GPIO_18_IN_PIN_NUM);
}

void digitalWriteLOW()
{
    ioctl(led_fd_SOFT, IOCTL_CMD_DIGITAL_WRITE_LOW, GPIO_18_IN_PIN_NUM);
}

void pinModeHIGH()
{
    ioctl(led_fd_SOFT, IOCTL_CMD_PIN_MODE_HIGH, GPIO_18_IN_PIN_NUM);
}

void pinModeLOW()
{
    ioctl(led_fd_SOFT, IOCTL_CMD_PIN_MODE_LOW, GPIO_18_IN_PIN_NUM);
}

/*
* piHiPri:
*    Attempt to set a high priority schedulling for the running program
*********************************************************************************
*/
int piHiPri (const int pri)
{
  struct sched_param sched ;

  memset (&sched, 0, sizeof(sched)) ;

  if (pri > sched_get_priority_max (SCHED_RR))
    sched.sched_priority = sched_get_priority_max (SCHED_RR) ;
  else
    sched.sched_priority = pri ;

  return sched_setscheduler (0, SCHED_RR, &sched) ;
}

/*
* softPwmThread:
*    Thread to do the actual PWM output
*********************************************************************************
*/

static void * softPwmThread (void *arg)
{
  int pin, mark, space ;
  struct sched_param param ;

  param.sched_priority = sched_get_priority_max (SCHED_RR) ; //Round Robin
  pthread_setschedparam (pthread_self (), SCHED_RR, &param) ;

  pin = *((int *)arg) ;
  free (arg) ;

  pin    = newPin ;
  newPin = -1 ;

  piHiPri (90) ;

  for (;;)
  {
    mark  = marks [pin] ;
    space = range [pin] - mark ;

    if (mark != 0)
      digitalWriteHIGH();
    usleep (mark * 100) ;

    if (space != 0)
      digitalWriteLOW() ;
    usleep (space * 100) ;
  }

  return NULL ;
}


/*
 * softPwmWrite:
 *    Write a PWM value to the given pin
 *********************************************************************************
 */

void softPwmWrite (int pin, int value)
{
  if (pin < MAX_PINS)
  {
    /**/ if (value < 0)
      value = 0 ;
    else if (value > range [pin])
      value = range [pin] ;

    marks [pin] = value ;
  }
}


/*
 * softPwmCreate:
 *    Create a new softPWM thread.
 *********************************************************************************
 */

int softPwmCreate (int pin, int initialValue, int pwmRange)
{
  int res ;
  pthread_t myThread ;
  int *passPin ;

  if (pin >= MAX_PINS)
    return -1 ;

  if (range [pin] != 0)    // Already running on this pin
    return -1 ;

  if (pwmRange <= 0)
    return -1 ;

  passPin = (int*) malloc (sizeof (*passPin)) ;
  if (passPin == NULL)
    return -1 ;

  digitalWriteLOW();
  pinModeHIGH();

  marks [pin] = initialValue ;
  range [pin] = pwmRange ;

  *passPin = pin ;
  newPin   = pin ;
  res      = pthread_create (&myThread, NULL, softPwmThread, (void *)passPin) ;

  while (newPin != -1)
    sleep (1);

  threads [pin] = myThread ;

  return res ;
}


/*
 * softPwmStop:
 *    Stop an existing softPWM thread
 *********************************************************************************
 */

void softPwmStop (int pin)
{
  if (pin < MAX_PINS)
  {
    if (range [pin] != 0)
    {
      pthread_cancel (threads [pin]) ;
      pthread_join   (threads [pin], NULL) ;
      range [pin] = 0 ;
      digitalWriteLOW();
    }
  }
}

void testPWM(void)
{
    int pin = GPIO_18_IN_PIN_NUM; //or 18
    int i = 0;
    softPwmCreate (pin, 0, PWM_RANGE); 
    
    for(i = 0 ; i < 100000000; i++)
    {
        printf("Hi\n");
        softPwmWrite(pin, i % 100);
    }
}


/* Dirty End */


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
#define G_IOCTL_SET_FREQUENCY _IOWR(GAS_IOCTL_MAGIC_NUMBER, 1, int)

/* MOTOR */
#define IOCTL_CMD_MOVE_FORWARD _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_MOVE_BACKWARD _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 1, int)
#define IOCTL_CMD_STOP_WINDING _IOWR(MOTOR_IOCTL_MAGIC_NUMBER, 2, int)

/* Sound */
#define S_IOCTL_CMD_GET_STATUS _IOWR(SOUND_IOCTL_MAGIC_NUMBER, 0, int)
#define S_IOCTL_SET_FREQUENCY _IOWR(SOUND_IOCTL_MAGIC_NUMBER, 1, int)

typedef struct 
{
	int fd1;
	int fd2;
	int fd3;
	int fd4;
}Thread;


void init_dev(dev_t * dev, int * fd, int MAJOR_NUMBER, int MINOR_NUMBER, char * dev_path)
{
    *dev = makedev(MAJOR_NUMBER, MINOR_NUMBER);
    mknod(dev_path, S_IFCHR|0666, *dev);
    *fd = open(dev_path, O_RDWR);

    if(*fd < 0){
		printf("fail to open %s\n", dev_path);	
	}
}

void* control_motor(void* p){
	Thread* temp = (Thread*)p;
	int b_fd = temp->fd1;
	int m_fd = temp->fd2;
	int current_button_value = 0, prev_button_value=0; 
	int counter = 0;
	
	while(1)
	{
		usleep(INTERVAL);
		prev_button_value = current_button_value;
		
		current_button_value = ioctl(b_fd, B_IOCTL_CMD_GET_STATUS);
		
		if (prev_button_value == 0 && current_button_value != 0) {
			printf("%d\n",counter);
			counter += 1;
		}
		if(counter == 2){
			printf("%d\n",counter);
			ioctl(m_fd, IOCTL_CMD_MOVE_FORWARD);
		}
		if(counter == 3){
			printf("%d\n",counter);
			ioctl(m_fd, IOCTL_CMD_STOP_WINDING);
		}
		if(counter == 4){
			printf("%d\n",counter);
			ioctl(m_fd, IOCTL_CMD_MOVE_BACKWARD);
		}
		if(counter == 5){
			printf("%d\n",counter);
			ioctl(m_fd, IOCTL_CMD_STOP_WINDING);
			counter = 0;
		}
	}
}

void* adc(void* p){
	Thread* temp = (Thread*)p;
	int g_fd = temp->fd1;
	int s_fd = temp->fd2;
	int gl_fd = temp->fd3;
	int sl_fd = temp->fd4;
	long gas_value, sound_value;
	
	ioctl(g_fd, G_IOCTL_SET_FREQUENCY, 250);
	ioctl(s_fd, S_IOCTL_SET_FREQUENCY, 250);
	
	while(1)
	{	
		usleep(10000);
		gas_value = ioctl(g_fd, G_IOCTL_CMD_GET_STATUS, NULL);
		sound_value = ioctl(s_fd, S_IOCTL_CMD_GET_STATUS, NULL);
		
		printf("SPI0 %d\n", (int)gas_value);
		printf("SPI1 %d\n", (int)sound_value);
		if(gas_value > 500){
			ioctl(gl_fd, IOCTL_CMD_ON, 18);
		}
		else if(gas_value <= 500){
			ioctl(gl_fd, IOCTL_CMD_OFF, 18);
		}
		if(sound_value > 20){
			ioctl(sl_fd, IOCTL_CMD_ON, 12);
		}
		else if(sound_value <= 20){
			ioctl(sl_fd, IOCTL_CMD_OFF, 12);
		}
	}
}
	
int main()
{	
	int i = 0;
	pthread_t threads[2];
	dev_t gled_dev, button_dev, gas_dev, motor_dev, sound_dev, sled_dev;
	int gled_fd , button_fd, gas_fd, motor_fd, sound_fd, sled_fd;

	/* main code */
	init_dev(&button_dev, &button_fd, BUTTON_MAJOR_NUMBER, BUTTON_MINOR_NUMBER, BUTTON_DEV_PATH_NAME);
	init_dev(&motor_dev, &motor_fd, MOTOR_MAJOR_NUMBER, MOTOR_MINOR_NUMBER, MOTOR_DEV_PATH_NAME);
	init_dev(&sound_dev, &sound_fd, SOUND_MAJOR_NUMBER, SOUND_MINOR_NUMBER, SOUND_DEV_PATH_NAME);
	init_dev(&gled_dev, &gled_fd, LED_MAJOR_NUMBER, LED_MINOR_NUMBER, LED_DEV_PATH_NAME);
	init_dev(&sled_dev, &sled_fd, LED_MAJOR_NUMBER, LED_MINOR_NUMBER, LED_DEV_PATH_NAME);
	init_dev(&gas_dev, &gas_fd, GAS_MAJOR_NUMBER, GAS_MINOR_NUMBER, GAS_DEV_PATH_NAME);

	led_fd_SOFT = gled_fd;
	testPWM();
	return 0;

	Thread th1;
	th1.fd1 = button_fd;
	th1.fd2 = motor_fd;
	
	Thread th2;
	th2.fd1 = gas_fd;
	th2.fd2 = sound_fd;
	th2.fd3 = gled_fd;
	th2.fd4 = sled_fd;
	
	pthread_create(&threads[0], NULL, control_motor, &th1);
	pthread_create(&threads[1], NULL, adc, &th2);
	
	for(i = 0; i < 2; i++){
		pthread_join(threads[i], NULL);
	}
	pthread_exit(NULL);

	close(button_fd);
	close(motor_dev);
	close(sound_dev);
	close(gas_dev);
	close(gled_dev);
	close(sled_dev);

	return 0;
}

