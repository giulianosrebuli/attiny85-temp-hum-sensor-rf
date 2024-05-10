# ATtiny85 Temperature sensor with SI7021

Low Power SI7021 Temperature and Humidity Sensor for ATtiny85 sent over RF 433Mhz

I wanted for a while to have a temperature sensor to integrate into my house smart gadgets, but i didn't want to add another Wi-Fi device for two main reasons: I wanted to minimize the devices that connects to my router and I wanted a small battery powered device that could run for years without me worrying. 
I have installed a Home Assistant instance with a RF receiver i made with Tasmota, and then i can filter data in software vi HA.
So I managed to have the ATtiny that is small and doesn't consume lots of power, since it will be sleeping most of the time.

I am including all the libraries that i used, and already added the same folders zipped so you can add straigth to your Arduino IDE. 

Links to original repositories from where i got the libraries:

* [TinyWireM](https://github.com/adafruit/TinyWireM)
* [TinySnore](https://github.com/connornishijima/TinySnore/tree/master)
* [SI7021](https://github.com/LowPowerLab/SI7021/tree/master)

**Notes**
- I had to make a slight modification in the SI7021 lib so it would be compatible with the TinyWireM;
- The SoftwareSerial lib is optional if you want to add serial printing to your project while testing. Be aware that this lib and it's usage take a lot of memory, so be cautious. If you want to now more about SoftwareSerial, I recomend visiting [this](https://docs.arduino.cc/learn/built-in-libraries/software-serial/) and [this](https://thewanderingengineer.com/2013/05/05/software-serial-on-the-attiny85/) link

Feel free to use as you please, and improvements are welcome.

This inicial code is just a initial desing, as the intention is to add RF support and it is not done yet. A hardware desing board will be included as well.
I will update the repo as soon it is ready.