#include <tinysnore.h>
#include <TinyWireM.h>
#include <SI7021.h>

/**
 * ATTINY PINS   _______
 * 1 -  BAT LVL |       |    VCC     - 8
 * 2 -  OPT RX  |       |  TEMP SCL  - 7
 * 3 -  OPT TX  |       |  RF OUT    - 6
 * 4 -   GND    |_______|  TEMP SDA  - 5
 * 
**/

#define SERIAL_DEBUG 0
#define BAT_LVL_PIN 0


#if SERIAL_DEBUG
  #include <SoftwareSerial.h>
  SoftwareSerial softSerial(3,4);
#endif

#define SLEEP_TIME_MS (5*60*1000) // 5 minutes

SI7021 sensor;

void setup() {

    sensor.begin();

#if SERIAL_DEBUG    
    softSerial.begin(9600); // ONLY WORKS WITH THIS BAUD RATE
    softSerial.println("--- ATtiny85 SI7021 Temp & Hum ---\n");
#endif

    pinMode(BAT_LVL_PIN, INPUT);
}

void loop() {

    si7021_env data = sensor.getHumidityAndTemperature();

    float temp = ((float)data.celsiusHundredths)/100;
    float hum = ((float)data.humidityBasisPoints)/100;
    int bat = analogRead(BAT_LVL_PIN);

#if SERIAL_DEBUG    
    softSerial.print("Temperature: ");
    softSerial.print(temp,2);
    softSerial.println(" °C\n");
    
    softSerial.print("Humidity: ");
    softSerial.print(hum,2);
    softSerial.println(" %\n");

    softSerial.print("Battery: ");
    softSerial.print(bat);
    softSerial.println(" ADC\n");
#endif

    snore(SLEEP_TIME_MS); // deep-sleep like - measured 5uA of consumption
}