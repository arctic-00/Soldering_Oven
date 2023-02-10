// Uses an Op-amp to amplify a K-type thermocouple temperature reading.
// Uses PID to control temperature

#include <AutoPID.h>
#include <SPI.h>

//pins
#define POT_PIN A0
#define OUTPUT_PIN 6
#define LED_PIN 7
#define AMP_OUT_PIN A1         // Pin for thermocouple amplifier
#define CJC_Thermister_PIN A2  // Pin for cold junction thermometer


#define TEMP_READ_DELAY 100 // only read from temp sensor every ____ms

//pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define KP 8
#define KI .01
#define KD 0.5

//Pins for RGB Lights
#define redPin 9
#define greenPin 10
#define bluePin 5

// Max difference in temperature where lights will change colour
#define tooHot  15
#define tooCold 20


double temperature, setPoint, outputVal;


//input/output variables passed by reference, so they are updated automatically
AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

unsigned long lastTempUpdate; //tracks clock time of last temp update

//call repeatedly in loop, only updates after a certain time interval
//returns true if update happened
bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    temperature = map(analogRead(CJC_Thermister_PIN), <value at temp 1>, <value at temp 2>, <temp 1>, <temp 2>);  // maps measured values to temperature at amplifier
    temperature += map(analogRead(AMP_OUT_PIN), <value at temp 3>, <value at temp 4>, <temp 3 - Room temperature>, <temp 4 - Room temperature>); // maps measured values to temperature difference between amplifier and the probe
    
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
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);


  Serial.begin(9600);
 

  //if temperature is more than 15 degrees below or above setpoint, OUTPUT will be set to min or max respectively
  myPID.setBangBang(15);
  //set PID update interval to 4000ms
  myPID.setTimeStep(200);

}//void setup


void loop() {
  updateTemperature();
  //setPoint = analogRead(POT_PIN);      
  setPoint = map(analogRead(POT_PIN), 0,  1024 /*<measured value for 240*C on dial>*/, 0, 240);
  analogWrite(LED_PIN, map(setPoint, 0, 1024, 0, 255));

  myPID.run(); //call every loop, updates automatically at certain time interval
  analogWrite(OUTPUT_PIN, outputVal);

  Serial.print("Temp: ");
  Serial.println(temperature);
//  Serial.print("    setPoint: ");
//  Serial.print(setPoint);
//  Serial.print("  Output: ");
//  Serial.println(outputVal);
  //digitalWrite(LED_PIN, myPID.atSetPoint(1)); //light up LED when we're at setpoint +-1 degree

  RGB_Indicator();

  delay(400);

}//void loop




void RGB_Indicator(){
  int red;
  int green;
  int blue;
  
  float tempDifference = setPoint - temperature;
  if (tempDifference < 0){
    red = constrain(-tempDifference*255/tooHot, 0, 255);
    green = 255 - red;
    blue = 0;

  } else{
    red = 0;
    blue = constrain(tempDifference*255/tooCold, 0, 255);
    green = 255 - blue;
  }

  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);

  /*Serial.print("Blue: ");
  Serial.print(blue);
  Serial.print("Green: ");
  Serial.print(green);
  */ 
}
