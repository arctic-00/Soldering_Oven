//   Controls a Toaster Oven to be used for reflow soldering.



#include <AutoPID.h>
#include <SPI.h>


// 0: MAX6675   1: MAX31855
#define TEMP_SENSOR 0

// #define USE_SERIAL
// #define USE_RGB_INDICATOR
#define USE_OLED

// Pins
#define POT_PIN A0   // Used for temperature control
#define OUTPUT_PIN 6
#define LED_PIN 7
// RGB LED pins
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 5
// Thermocouple software SPI
#define MAXDO   12
#define MAXCS   11
#define MAXCLK  13

// Set to 0 to disable
#define POT_ROLLING_AVERAGE 0.75

#define TEMP_READ_DELAY 500 // Can only read digital temp sensor every ____

// PID settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define BANG_BANG 90    // If temperature is more than __ degrees below or above setpoint, OUTPUT will be set to min or max respectively
#define KP 5
#define KI .03
#define KD 1


// Max difference in temperature where lights will change colour
#define TOO_HOT  15
#define TOO_COLD 20








double temperature, setPoint, prevSetPoint, outputVal;


#if TEMP_SENSOR == 0
  #include <max6675.h>
  MAX6675 thermocouple(MAXCLK, MAXCS, MAXDO);
#elif TEMP_SENSOR == 1
  #include "Adafruit_MAX31855.h"
  Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
#endif

#if defined(USE_SERIAL)
  #define SBegin(a) (Serial.begin(a))
  #define SPrint(a) (Serial.print(a))
#else
  #define SBegin(a)
  #define SPrint(a)
#endif

#if defined(USE_OLED)
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64

  #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
  #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

//input/output variables passed by reference, so they are updated automatically
AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

unsigned long lastTempUpdate; // Tracks clock time of last temp update. Call repeatedly in loop, only updates after a certain time interval.  
bool updateTemperature() {    // Returns true if update happened
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    temperature = thermocouple.readCelsius();
    lastTempUpdate = millis();
    return true;
  }
  return false;
}//void updateTemperature



void setup() {
  pinMode(POT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  //RGB Lights
  #if defined(USE_RGB_INDICATOR)
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
  #endif
  
  SBegin(9600);
  SPrint("\nStarting...\n");
  // wait for MAX chip to stabilize
  delay(500);
  
  // If using MAX31855
  #if TEMP_SENSOR == 1
    SPrint("Initializing sensor...");
    thermocouple.begin();
    if (!thermocouple.begin()) {
      SPrint("ERROR.\n");
      while (1) delay(10);
    }
    SPrint("DONE.\n");
  #endif

  #ifdef USE_OLED
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      SPrint(F("SSD1306 allocation failed\n"));
      for(;;); // Don't proceed, loop forever
    }
  #endif


  // If temperature is more than __ degrees below or above setpoint, OUTPUT will be set to min or max respectively
  myPID.setBangBang(BANG_BANG);
  // Set PID update interval to 200ms
  myPID.setTimeStep(200);

}//void setup


void loop() {
  updateTemperature();
  SPrint("Temp: ");
  SPrint(temperature);
  
  // Use rolling average for noisy pots
  setPoint = map(analogRead(POT_PIN), 0,  1024, 300, 0);
  setPoint = (setPoint + POT_ROLLING_AVERAGE * prevSetPoint) / (POT_ROLLING_AVERAGE + 1);

  analogWrite(LED_PIN, map(setPoint, 0, 1024, 0, 255));
  SPrint("\tSetPoint: ");
  SPrint(int(setPoint));
  myPID.run(); //call every loop, updates automatically at certain time interval
  analogWrite(OUTPUT_PIN, outputVal);
  SPrint("\tOutput: ");
  SPrint(int(outputVal));
  // digitalWrite(LED_PIN, myPID.atSetPoint(1)); //light up LED when we're at setpoint +-1 degree

  #if defined(USE_RGB_INDICATOR)
    RGB_Indicator();
  #endif
  
  updateOLED();
  prevSetPoint = setPoint;
  SPrint("\n");
  delay(400);

}//void loop



#ifdef USE_RGB_INDICATOR
  void RGB_Indicator(){
    int red;
    int green;
    int blue;
    
    float tempDifference = setPoint - temperature;
    if (tempDifference < 0){
      red = constrain(-tempDifference*255/TOO_HOT, 0, 255);
      green = 255 - red;
      blue = 0;

    } else{
      red = 0;
      blue = constrain(tempDifference*255/TOO_COLD, 0, 255);
      green = 255 - blue;
    }

    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);

    SPrint("\t\tRed: ");
    SPrint(red);
    SPrint(" \tBlue: ");
    SPrint(blue);
    SPrint(" \tGreen: ");
    SPrint(green);
  }
#endif

#ifdef USE_OLED
  void updateOLED() {
    display.clearDisplay();

    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    if (temperature >= 100) {
      display.print(F("Temp:"));
      display.println(temperature, 1);
    } else {
      display.print(F("Temp:"));
      display.println(temperature, 2);
    }
    
    display.print(F("Set: "));
    display.println(setPoint, 0);
    display.print(F("Out: "));
    display.println(outputVal, 0);
    display.display();      // Show initial text
  }
#else
  void updateOLED() {}
#endif
