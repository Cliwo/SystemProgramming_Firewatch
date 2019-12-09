#! /bin/bash

echo "##### Refreshing ButtonDriver #####"
cd ./ButtonDriver
sudo rmmod button_driv
sudo insmod button_driv.ko
cd ../

echo "##### Refreshing GasDriver #####"
cd ./GasDriver
sudo rmmod gas_driv
sudo insmod gas_driv.ko
cd ../

echo "##### Refreshing LEDDriver #####"
cd ./LEDDriver
sudo rmmod led_driv
sudo insmod led_driv.ko
cd ../

echo "##### Refreshing MotorDriver #####"
cd ./MotorDriver
sudo rmmod motor_driv
sudo insmod motor_driv.ko
cd ../

echo "##### Refreshing SoundDriver #####"
cd ./SoundDriver
sudo rmmod sound_driv
sudo insmod sound_driv.ko
cd ../

dmesg
