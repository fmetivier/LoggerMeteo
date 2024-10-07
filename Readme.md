# NanoLogW : an Autonomous Weather Logger using Arduino Nano

## Principle
NanoLogW is an autonomous weather data logger working on AA/AAA batteries.
It has the following features:

* the logger is controlled by an Arduino Nano that has been modified in order to lower power consumption, yet keep the instrument simpe to handle and program;
* it records Temperature, Pressure, Humidity and Gass content using an Adafruit BME680 sensor;
* data is stored on a uPesy SD card reader;
* time is recorded by an RTC1307 clock (again from Adafruit)
* it is packed in a Dorfman wheather "abri"

all components are soldered on a Fritzing designed PCB card.
The Logger is easy to build (it is part of an undergraduate student's class), easy to use (just remove the SD card from time to time to load the data that is written on an ascii text file), and is independant of any network and proprietary software. 

With a sampling rate of 15 minutes, it runs more than five monthes on 3 AAA batteries or 11 monthes on 3 AA batteries. It is meant to be depoloyed in places where connection is absent of ectic.



## Includes

* Arduino code
* fritzing diagram


## Cost

|Component | Price (€) |
|---------|-----------|
|Arduino Nano | 29 |
|RTC Clock 1307 | |
|uPesy SD card reader| |
|BME 680 | |
|Battery coupling | |
|PCB Card | |
|SD card| |
|Batteries | |


## Images

![Breadboard](./figures/NanoLoggerBME680_bb.svg)

![PCB card](./figures/NanoLoggerBME680_circuit imprimé.svg)






