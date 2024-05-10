#include <tinysnore.h>
#include <TinyWireM.h>
#include <SI7021.h>

#define SERIAL_DEBUG 0

#if SERIAL_DEBUG
  #include <SoftwareSerial.h>
  SoftwareSerial softSerial(3,4);
#endif

#define SLEEP_TIME_MS (5*60*1000) // 5 minutes

SI7021 sensor;

void setup() {

    sensor.begin();

#if SERIAL_DEBUG    
    softSerial.begin(9600);
    softSerial.println("--- ATtiny85 SI7021 Temp & Hum ---\n");
#endif
}

void loop() {

    si7021_env data = sensor.getHumidityAndTemperature();

    float temp = ((float)data.celsiusHundredths)/100;
    float hum = ((float)data.humidityBasisPoints)/100;

#if SERIAL_DEBUG    
    softSerial.print("Temperature: ");
    softSerial.print(temp,2);
    softSerial.println(" °C\n");
    
    softSerial.print("Humidity: ");
    softSerial.print(hum,2);
    softSerial.println(" %\n");
#endif

    snore(SLEEP_TIME_MS); // deep-sleep like - measured 5uA of consumption
}