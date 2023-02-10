#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48

ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);

float voltage = 0;


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

void setup() {
  Wire.begin();
  Serial.begin(9600);
  adc.init();

  Serial.println(thermocoupleTemp());
}

void loop() {
  /*
 // while(Serial.available()){
    float voltage = 4;

    Serial.print("Voltage: ");
    Serial.println(voltage);
    voltage *= 1000;

    bool negativeVoltage = false;
    if (voltage < 0){
      negativeVoltage = true;
      voltage = -voltage;
    }
    int height = (chartHeight - 1)/2;
    int divisor = 4;
    while( !( (KThermocoupleChart[height][0] <= voltage) and (KThermocoupleChart[height + 1][0] >= voltage) ) ){
      Serial.print("height: ");
        Serial.print(height);
        Serial.print("  divisor: ");
        Serial.print(divisor);
        Serial.print(" Chart Value: ");
        Serial.println(KThermocoupleChart[height][0]);
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
      Serial.print("width: ");
        Serial.print(width);
        Serial.print("  divisor: ");
        Serial.print(divisor);
        Serial.print(" Chart Value: ");
        Serial.println(KThermocoupleChart[height][width]);
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

        
    Serial.print("Found Height : ");
    Serial.print(height);
    Serial.print("  Found width : ");
    Serial.print(width);
    Serial.print(" Chart Value: ");
    Serial.print(KThermocoupleChart[height][width]);


    Serial.print(" Temperature:  ");
    float temperature = height * 10 + width + (voltage - KThermocoupleChart[height][width])/41;;
    if(negativeVoltage)
      temperature = -temperature;
    Serial.println(temperature);

    return;
 // }
*/

Serial.println(thermocoupleTemp());
delay(1000);
}


float thermocoupleTemp(){
  adc.setVoltageRange_mV(ADS1115_RANGE_0256);
  voltage = readChannel(ADS1115_COMP_0_GND) * 1000;

  Serial.print("Voltage: ");
  Serial.println(voltage);

  bool negativeVoltage = false;
  if (voltage < 0){
    negativeVoltage = true;
    voltage = -voltage;
  }
  int height = (chartHeight - 1)/2;
  int divisor = 4;
  while( !( (KThermocoupleChart[height][0] <= voltage) and (KThermocoupleChart[height + 1][0] >= voltage) ) ){
    Serial.print("height: ");
      Serial.print(height);
      Serial.print("  divisor: ");
      Serial.print(divisor);
      Serial.print(" Chart Value: ");
      Serial.println(KThermocoupleChart[height][0]);
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
    Serial.print("width: ");
      Serial.print(width);
      Serial.print("  divisor: ");
      Serial.print(divisor);
      Serial.print(" Chart Value: ");
      Serial.println(KThermocoupleChart[height][width]);
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

      
  Serial.print("Found Height : ");
  Serial.print(height);
  Serial.print("  Found width : ");
  Serial.print(width);
  Serial.print(" Chart Value: ");
  Serial.print(KThermocoupleChart[height][width]);


  Serial.print(" Temperature:  ");
  float temperature = height * 10 + width + (voltage - KThermocoupleChart[height][width])/41;;
  if(negativeVoltage)
    temperature = -temperature;
  Serial.println(temperature);

  return temperature;
}

float readChannel(ADS1115_MUX channel) {
  float voltage = 0.0;
  adc.setCompareChannels(channel);
  adc.startSingleMeasurement();
  while(adc.isBusy()){}
  voltage = adc.getResult_mV(); // alternative: getResult_V for Volt
  return voltage;
}