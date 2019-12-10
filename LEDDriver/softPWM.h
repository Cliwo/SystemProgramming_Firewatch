#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#include <pthread.h>

// MAX_PINS:
//    This is more than the number of Pi pins because we can actually softPwm.
//    Once upon a time I let pins on gpio expanders be softPwm'd, but it's really
//    really not a good thing.
#define PWM_RANGE   100
#define MAX_PINS    32
#define GPIO_18_IN_PIN_NUM 1

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
    
    for(i = 0 ; i < 100; i++)
    {
        printf("Hi");
        softPwmWrite(pin, i);
    }
}
