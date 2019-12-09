#! /bin/bash

echo "##### Make ButtonDriver #####"
cd ./ButtonDriver
sudo make
cd ../

echo "##### Make GasDriver #####"
cd ./GasDriver
sudo make
cd ../

echo "##### Make LEDDriver #####"
cd ./LEDDriver
sudo make
cd ../

echo "##### Make MotorDriver #####"
cd ./MotorDriver
sudo make
cd ../

echo "##### Make SoundDriver #####"
cd ./SoundDriver
sudo make
cd ../
