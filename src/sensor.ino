#include <tinysnore.h>
#include <TinyWireM.h>
#include <SI7021.h>
#include <RCSwitch.h>
#include <stdlib.h>
#include <string.h>

/**
 * ATTINY PINS   _______
 * 1 -  BAT LVL |       |    VCC     - 8
 * 2 -  OPT RX  |       |  TEMP SCL  - 7
 * 3 -  OPT TX  |       |  RF OUT    - 6
 * 4 -   GND    |_______|  TEMP SDA  - 5
 * 
**/

#define SERIAL_DEBUG 0

#define RF_TX_PIN 1
#define BAT_LVL_PIN 0

#define TEMP_HEADER 0xFD670000
#define HUM_HEADER 0xFAB10000
#define BAT_HEADER 0x180D0000

#if SERIAL_DEBUG
  #include <SoftwareSerial.h>
  SoftwareSerial softSerial(3,4);
#endif

#define SLEEP_TIME_MS (5*60*1000) // 5 minutes

SI7021 sensor;
RCSwitch RF = RCSwitch();

uint8_t to_minifloat(float value);

void setup() {

    sensor.begin();

#if SERIAL_DEBUG    
    softSerial.begin(9600); // ONLY WORKS WITH THIS BAUD RATE
    softSerial.println("--- ATtiny85 SI7021 Temp & Hum ---\n");
#endif

    RF.enableTransmit(RF_TX_PIN);
    pinMode(BAT_LVL_PIN, INPUT);
}

void loop() {

    si7021_env data = sensor.getHumidityAndTemperature();

    float temp = ((float)data.celsiusHundredths)/100;
    float hum = ((float)data.humidityBasisPoints)/100;
    float battery_voltage = (analogRead(BAT_LVL_PIN)*3)/1024;
    uint8_t bat = (uint8_t) map(battery_voltage, 2.2, 3, 0, 100);

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

    uint32_t temperature_rf_data = TEMP_HEADER + to_minifloat(temp);
    uint32_t humidity_rf_data = HUM_HEADER + to_minifloat(hum);
    uint32_t battery_rf_data = BAT_HEADER + bat;
    
    RF.send(temperature_rf_data,32); delay(2000); RF.send(temperature_rf_data,32); delay(2000);
    RF.send(humidity_rf_data,32); delay(2000); RF.send(humidity_rf_data,32); delay(2000);
    RF.send(battery_rf_data,32); delay(2000); RF.send(battery_rf_data,32); delay(2000);

#if SERIAL_DEBUG
    softSerial.print("RF Sent: ");
    softSerial.println(rf_data, HEX);
#endif
    
    snore(SLEEP_TIME_MS); // deep-sleep like - measured 5uA of consumption
}


/**
 * I only managed to send 32bit of data via RF,
 * and I was worried that the spectrum would be to clamsy,
 * so I decided to include a header to each data I send, 
 * so that I could identify the received data and so I could
 * confirm that the data was indeed my own.
 * I read that 3 to 4 bytes was enough to ensure the data was coming
 * from my own device, and since I had only 4 bytes to send, I used
 * 3 of them.
 * The thing is, since there's only 1 byte left, i designed a way to
 * send the data as fixed-point precision, with a resolution of 0.25.
 * I used 6 bits (MSB) to store the integer (only positive here) part, with
 * a range from 0 to 63, and the 2 bits (LSB) to the fractional part,
 * where 00 is .00, 01 is .25, 10 is .50 and 11 is .75. This function here
 * implements that.
 * I also added another function below so you could reference a "decoder",
 * wherever you may use it.
*/
uint8_t to_minifloat(float value){
    
    uint8_t istr[9];
    uint8_t fstr[9];
    char ret_str[9];
    
    uint8_t val_i = (uint8_t) value;    // extract integer part
    uint8_t val_f = (value - val_i)*4;  // extract fractional part, multiplies by 4 to 2bit precision
        
    // turn the integer part in a binary string
    for (int i = 7; i >= 0; i--) {
        istr[i] = (val_i & 0x01) + '0';
        val_i = val_i >> 1;
    }
    // turn the fractional part in a binary string
    for (int i = 7; i >= 0; i--) {
        fstr[i] = (val_f & 0x01) + '0';
        val_f = val_f >> 1;
    }
    
    // combine both strings - with limited bits - to a single string
    sprintf(ret_str,"%s%s",&istr[2],&fstr[6]);
    
    // convert the string into a integer again
    uint8_t ret = (uint8_t) strtol(ret_str,NULL,2);
    
    return ret;
}

float from_minifloat(uint8_t value){
  
  uint8_t bin_string[9];
  uint8_t int_part = 0;
  uint8_t frac_part = 0;
  float ret_data;

  // turn the received value in a binary string
  for (int i = 7; i >= 0; i--) {
      bin_string[i] = (value & 0x01);
      value = value >> 1;
  }

  frac_part = bin_string[7] + 2*bin_string[6];
  
  for(int i = 0; i < 7; i++){
    int_part += bin_string[i] << (5-i);
  }

  ret_data = int_part + ((float)frac_part)/4;

  return ret_data;
}