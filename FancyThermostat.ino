/*
 * cheap thermostat-like device to learn various components
 * 06/02/2019
 * by gbolatto
 */

/*
 * LCD1602 module:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * LCD VO pin to Potentiometer to control contrast
 * 
 * SRD-05VDC-SL-C 5V Relay:
 * P2N2222A transistor for the relay on digital pin 5
 * 
 * Basic buttons on digital pins 3 and 4
 * 
 * DHT11 Humidity and Temperature sensor module on digital pin 2
 */

#include <LiquidCrystal.h>
#include <dht_nonblocking.h>

// initialize the LiquidCrystal library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// initialize the DHT_nonblocking library on digital pin 2
DHT_nonblocking dht_sensor(2, DHT_TYPE_11);

// relayAC on digital pin 5
int relayAC = 5;

// left decrease temperature button on pin 3
int buttonDecrease = 3;

// right increase temperature button on pin 4
int buttonIncrease = 4;

// convert C to F since the humidity/temp sensor library reports back in C
static float convertCToF(float tempInC) {
  return(tempInC * 9 / 5 + 32);
}

static bool measure_environment(float *temperature, float *humidity) {
  static unsigned long measurement_timestamp = millis();
  // Measure once every 10 seconds
  if(millis() - measurement_timestamp > 9000ul) {
    if(dht_sensor.measure(temperature, humidity) == true) {
      measurement_timestamp = millis();
      return(true);
    }
  }
  return(false);
}

void setup() {
  lcd.begin(16, 2); // set up the LCD's number of columns and rows:
  lcd.setCursor(0,0);
  lcd.print("hello, world!");
  lcd.setCursor(0,1);
  lcd.print("booting up!");

  pinMode(relayAC, HIGH);
  pinMode(buttonDecrease, INPUT_PULLUP);
  pinMode(buttonIncrease, INPUT_PULLUP);
}

// basically defaulting to 70 fahrenheit since i'm an american heathen
float setTemperatureInF = 70;

void loop() {

  float tempInC;
  float humidity;

  if (measure_environment(&tempInC, &humidity) == true) {
    float tempInF = convertCToF(tempInC);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(String(tempInF) + " F");
    lcd.setCursor(0, 1);
    lcd.print(String(humidity) + "% Humidity");

    /*
    TODO: get a running average over the last 1 or so minutes and use that to calculate
    whether to turn on or off. currently an issue with the sensor going back and forth 
    with temperature changes for a few seconds at each temp change which rapidly turns
    the relay on and off.
    get running average or lower polling time of sensor?
    */
    
    // relay on if temp goes above the set temp
    if (tempInF > setTemperatureInF) {
      digitalWrite(relayAC, HIGH);
    }

    // relay off if temp goes below or equal to set temp
    if (tempInF <= setTemperatureInF) {
      digitalWrite(relayAC, LOW);
    }
  }

  // increases the setTemperatureInF by 1 every 250ms (if held down) and prints it to LCD
  if (digitalRead(buttonIncrease) == LOW) {
    setTemperatureInF += 1;
    delay(250);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set temp:");
    lcd.setCursor(0,1);
    lcd.print(setTemperatureInF);
  }

  // decreases the setTemperatureInF by 1 every 250ms (if held down) and prints it to LCD
  if (digitalRead(buttonDecrease) == LOW) {
    setTemperatureInF -= 1;
    delay(250);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set temp:");
    lcd.setCursor(0,1);
    lcd.print(setTemperatureInF);
  }
}

