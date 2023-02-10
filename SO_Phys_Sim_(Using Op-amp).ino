// Uses formula to simulate energy required to reach a given temperature
// Constants A and B need to be measured for different ovens used


//pins
#define TempPin A1
#define AmbTempPin A2
#define POT_PIN A3 
#define setTempButPin 3  
#define OUTPUT_PIN 6


#define chartHeight 34
#define chartWidth 11
int KThermocoupleChart[chartHeight][chartWidth] = {
    0,   39,   79,  119,  158,  198,  238,  277,  317,  357,  397, 
  397,  437,  477,  517,  557,  597,  637,  677,  718,  758,  798, 
  798,  838,  879,  919,  960, 1000, 1041, 1081, 1122, 1163, 1203, 
 1203, 1244, 1285, 1326, 1366, 1407, 1448, 1489, 1530, 1571, 1612, 
 1612, 1653, 1694, 1735, 1776, 1817, 1858, 1899, 1941, 1982, 2023, 
 2023, 2064, 2106, 2147, 2188, 2230, 2271, 2312, 2354, 2395, 2436, 
 2436, 2478, 2519, 2561, 2602, 2644, 2685, 2727, 2768, 2810, 2851, 
 2851, 2893, 2934, 2976, 3017, 3059, 3100, 3142, 3184, 3225, 3267, 
 3267, 3308, 3350, 3391, 3433, 3474, 3516, 3557, 3599, 3640, 3682, 
 3682, 3723, 3765, 3806, 3848, 3889, 3931, 3972, 4013, 4055, 4096, 
 4096, 4138, 4179, 4220, 4262, 4303, 4344, 4385, 4427, 4468, 4509, 
 4509, 4550, 4591, 4633, 4674, 4715, 4756, 4797, 4838, 4879, 4920, 
 4920, 4961, 5002, 5043, 5084, 5124, 5165, 5206, 5247, 5288, 5328, 
 5328, 5369, 5410, 5450, 5491, 5532, 5572, 5613, 5653, 5694, 5735, 
 5735, 5775, 5815, 5856, 5896, 5937, 5977, 6017, 6058, 6098, 6138, 
 6138, 6179, 6219, 6259, 6299, 6339, 6380, 6420, 6460, 6500, 6540, 
 6540, 6580, 6620, 6660, 6701, 6741, 6781, 6821, 6861, 6901, 6941, 
 6941, 6981, 7021, 7060, 7100, 7140, 7180, 7220, 7260, 7300, 7340, 
 7340, 7380, 7420, 7460, 7500, 7540, 7579, 7619, 7659, 7699, 7739, 
 7739, 7779, 7819, 7859, 7899, 7939, 7979, 8019, 8059, 8099, 8138, 
 8138, 8178, 8218, 8258, 8298, 8338, 8378, 8418, 8458, 8499, 8539, 
 8539, 8579, 8619, 8659, 8699, 8739, 8779, 8819, 8860, 8900, 8940, 
 8940, 8980, 9020, 9061, 9101, 9141, 9181, 9222, 9262, 9302, 9343, 
 9343, 9383, 9423, 9464, 9504, 9545, 9585, 9626, 9666, 9707, 9747, 
 9747, 9788, 9828, 9869, 9909, 9950, 9991, 10031, 10072, 10113, 10153, 
 10153, 10194, 10235, 10276, 10316, 10357, 10398, 10439, 10480, 10520, 10561,
 10561, 10602, 10643, 10684, 10725, 10766, 10807, 10848, 10889, 10930, 10971,
 10971, 11012, 11053, 11094, 11135, 11176, 11217, 11259, 11300, 11341, 11382,
 11382, 11423, 11465, 11506, 11547, 11588, 11630, 11671, 11712, 11753, 11795,
 11795, 11836, 11877, 11919, 11960, 12001, 12043, 12084, 12126, 12167, 12209,
 12209, 12250, 12291, 12333, 12374, 12416, 12457, 12499, 12540, 12582, 12624,
 12624, 12665, 12707, 12748, 12790, 12831, 12873, 12915, 12956, 12998, 13040,
 13040, 13081, 13123, 13165, 13206, 13248, 13290, 13331, 13373, 13415, 13457,
 13457, 13498, 13540, 13582, 13624, 13665, 13707, 13749, 13791, 13833, 13874
};

unsigned long lastTime = 0;
float iterationTime = 12;   //Change to unsigned int if time for void loop to iterate can be higher than 255ms
int setTemp = 0;
byte Power = 0;               // PWM power
long unconstrainedPower = 0;   // Power required to reach temperature in one iteration (in milliJoules)
const float A = 0.00247569836;  //Heat Loss Constant
const float B = 680;  //Specific Heat Constant
long heatEnergyLeft = 0; // Total energy required to reach set temp (Not including energy to make up for heat loss)    Can be negative to show oven has excess energy
float heatEnergyLost = 0; //Energy lost in single iteration

float initialTemp = 25;
float temp = 25;  // Add function


void setup() {
  
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(setTempButPin, INPUT_PULLUP);
  Serial.begin(115200);
  
  if (temp < 40){         // In case oven was reset while hot
    Serial.print(" ambTemp: ");
    Serial.println(ambTemp());
  } else {
    Serial.println("Unknown Ambient Temperature");
  }
}


void loop() {
  if (!digitalRead(setTempButPin)) {       // If temperature is chosen
    setTemp = map(analogRead(POT_PIN), 0,  650 /*<measured value for 240*C on dial>*/, 0, 240); //when pot is connected to 3.3v, 743 when connected to 5v
    Serial.print(" setTemp: ");
    Serial.print(setTemp);
    initialTemp = thermocoupleTemp() + ambTemp(); // Adds thermocouple and thermister values
    Serial.print(" initialTemp: ");
    Serial.println(initialTemp);
    heatEnergyLeft = B * (setTemp - initialTemp);

  }

  Serial.print(" heatEnergyLeft: ");
  Serial.print(heatEnergyLeft);

  temp = thermocoupleTemp() + ambTemp(); // Adds thermocouple and thermister values

  Serial.print(" setTemp: ");
  Serial.print(setTemp);

  Serial.print(" thermocoupleTemp: ");
  Serial.print(thermocoupleTemp());

  Serial.print(" temp: ");
  Serial.print(temp);


  iterationTime = millis() - lastTime;    // Might need to change units to seconds otherwise energy units are in milliJoules, Should check for 'lastTime' flipping (Resetting back to zero)
  lastTime = millis();

  Serial.print(" iterationTime: ");
  Serial.print(iterationTime);


  heatEnergyLost = A * (temp - ambTemp()) * (iterationTime);   // Must go before any change to power variable, requires fairly stable iterationTime, in milliJoules
  unconstrainedPower = (heatEnergyLost + B * (setTemp - temp) * 1000) / iterationTime;  // " * 1000" To convert joules to milliJoules for oncoming division by ms

  Serial.print(" heatEnergyLost: ");
  Serial.print(heatEnergyLost);
  Serial.print(" unconstrainedPower: ");
  Serial.print(unconstrainedPower);


  // Ensures Oven doesn't produce more energy than necessary to
  // reach required temperature in single iteration if at full power
  if ((heatEnergyLeft >= 1200 * 0.001 * iterationTime  - heatEnergyLost) && unconstrainedPower > 0){    
    Serial.print("  1  ");
    Power = 255;
    heatEnergyLeft -= (1200 * 0.001 * iterationTime - heatEnergyLost);    // Find energy put towards increasing oven temp (By removing energy lost to counter heat loss) and minus it from total energy required to reach required temp
  } else if((1200 > unconstrainedPower) && (unconstrainedPower > 0) ) {  // If oven can reach temp in one iteration
    Serial.print("  2  ");
    Power = byte((unconstrainedPower * 255) / 1200);   // Converting watts to byte
    heatEnergyLeft = 0;
  } else {                                    // If the oven needs more than one iteration to cool down
    Serial.print("  3  ");
    Power = 0;
    heatEnergyLeft += heatEnergyLost;          // Heat loss reduces excess energy (Which is written as a negative value)
  }
  
  analogWrite(OUTPUT_PIN, Power);
  Serial.print("  Power: ");
  Serial.println(Power);
}

float thermocoupleTemp(){
  int voltage = map(analogRead(TempPin), 0, 1024 /* value out */, 0, 9766 /* thermocouple reading in Microvolts */);

  // Serial.print("Voltage: ");
  // Serial.println(voltage);

  bool negativeVoltage = false;
  if (voltage < 0){   // The chart has the same values for negative temperatures but in order for them to be compared they first need to be made positive
    negativeVoltage = true;
    voltage = -voltage;
  }
  int height = (chartHeight - 1)/2;
  int divisor = 4;
  while( !( (KThermocoupleChart[height][0] <= voltage) and (KThermocoupleChart[height + 1][0] >= voltage) ) ){
    // Serial.print("height: ");
    //   Serial.print(height);
    //   Serial.print("  divisor: ");
    //   Serial.print(divisor);
    //   Serial.print(" Chart Value: ");
    //   Serial.println(KThermocoupleChart[height][0]);
    if (KThermocoupleChart[height][0] < voltage){
      height += chartHeight / divisor;
    } else {
      height -= chartHeight / divisor;
    }

    if((divisor * 2) < chartHeight){
    divisor *= 2;
    } else {
      divisor = chartHeight - 1;
    }
  }

  
  int width = (chartWidth - 1) / 2;
  divisor = 4;
  while( !( (KThermocoupleChart[height][width] <= voltage) and (KThermocoupleChart[height][width + 1] > voltage) ) ){
    // Serial.print("width: ");
    //   Serial.print(width);
    //   Serial.print("  divisor: ");
    //   Serial.print(divisor);
    //   Serial.print(" Chart Value: ");
    //   Serial.println(KThermocoupleChart[height][width]);
    if (KThermocoupleChart[height][width] < voltage){
      width += chartWidth / divisor;
    } else {
      width -= chartWidth / divisor;
    }

    if((divisor * 2) < chartWidth){
    divisor *= 2;
    } else {
      divisor = chartWidth - 1;
    }
  }

      
  // Serial.print("Found Height : ");
  // Serial.print(height);
  // Serial.print("  Found width : ");
  // Serial.print(width);
  // Serial.print(" Chart Value: ");
  // Serial.print(KThermocoupleChart[height][width]);


  // Serial.print(" Temperature:  ");
  float temperature = height * 10 + width + (voltage - KThermocoupleChart[height][width])/41;;
  if(negativeVoltage)
    temperature = -temperature;
  // Serial.println(temperature);

  return temperature;
}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float ambTemp() {
  return mapfloat(analogRead(AmbTempPin), 585 /*value at temp 3*/, 482 /*value at temp 4*/, 17.2 /*temp 3*/, 35 /*temp 4*/);  // maps measured values to temperature
}
