import time
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(18, GPIO.OUT)

p = GPIO.PWM(18, 50)
p.start(0)

p.stop()

exit()

while 1:
	for dc in range(0, 101000, 5):
		p.ChangeDutyCycle(dc)
		time.sleep(0.05)

