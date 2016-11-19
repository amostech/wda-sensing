
#include "MPU9250.h"

#define SerialDebug true  // Set to true to get Serial output for debugging

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling

MPU9250 myIMU;

/*WDA Specific Pins*/
#define IMU_PIN_TO_PROC 38
#define CALCULATE_MAGNETIC_ENVIRONMENTAL_STEPS 200
#define THRESHOLD_FROM_AVG 35
#define CONVERT_TO_MICROTESLA 4912./8190. //microTesla

int countForward = 0;
int countBackward = 0;
bool forwardFlag = 0;
bool backwardFlag = 0;

void calculateMagneticEnvironmentalBias() {
    
    int i = 0;
    
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    myIMU.magbias[0] = 0;
    // User environmental x-axis correction in milliGauss TODO axis??
    myIMU.magbias[1] = 0;
    // User environmental x-axis correction in milliGauss
    myIMU.magbias[2] = 0;

    SerialUSB.print("Wait... Calculating Offsets.");
    for( i = 0; i < CALCULATE_MAGNETIC_ENVIRONMENTAL_STEPS; i++) {
      while(!(myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01));
     
        myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
        //myIMU.getMres();
      
        myIMU.magbias[0] += (float) myIMU.magCount[0]*CONVERT_TO_MICROTESLA;
        myIMU.magbias[1] += (float) myIMU.magCount[1]*CONVERT_TO_MICROTESLA;
        myIMU.magbias[2] += (float) myIMU.magCount[2]*CONVERT_TO_MICROTESLA;
       
    }

  myIMU.magbias[0] /= CALCULATE_MAGNETIC_ENVIRONMENTAL_STEPS;
  myIMU.magbias[1] /= CALCULATE_MAGNETIC_ENVIRONMENTAL_STEPS;
  myIMU.magbias[2] /= CALCULATE_MAGNETIC_ENVIRONMENTAL_STEPS;

  SerialUSB.println("Environmental Offsets Calculated: ");
  SerialUSB.println("x: ");
  SerialUSB.println(myIMU.magbias[0],DEC);
  SerialUSB.println("y: ");
  SerialUSB.println(myIMU.magbias[1],DEC);
  SerialUSB.println("z: ");
  SerialUSB.print(myIMU.magbias[2],DEC);
  SerialUSB.println();
}

void setup()
{
   Wire.begin();

  // TWBR = 12;  // 400 kbit/sec I2C speed
  while(!SerialUSB);
    SerialUSB.begin(115200);

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  SerialUSB.print("MPU9250 "); SerialUSB.print("I AM "); SerialUSB.print(c, HEX);
  SerialUSB.print(" I should be "); SerialUSB.println(0x71, HEX);


  if (c == 0x71) // WHO_AM_I should always be 0x68
  {
    SerialUSB.println("MPU9250 is online...");

    // Start by performing self test and reporting values
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    SerialUSB.print("x-axis self test: acceleration trim within : ");
    SerialUSB.print(myIMU.SelfTest[0],1); SerialUSB.println("% of factory value");
    SerialUSB.print("y-axis self test: acceleration trim within : ");
    SerialUSB.print(myIMU.SelfTest[1],1); SerialUSB.println("% of factory value");
    SerialUSB.print("z-axis self test: acceleration trim within : ");
    SerialUSB.print(myIMU.SelfTest[2],1); SerialUSB.println("% of factory value");
    SerialUSB.print("x-axis self test: gyration trim within : ");
    SerialUSB.print(myIMU.SelfTest[3],1); SerialUSB.println("% of factory value");
    SerialUSB.print("y-axis self test: gyration trim within : ");
    SerialUSB.print(myIMU.SelfTest[4],1); SerialUSB.println("% of factory value");
    SerialUSB.print("z-axis self test: gyration trim within : ");
    SerialUSB.print(myIMU.SelfTest[5],1); SerialUSB.println("% of factory value");

    // Calibrate gyro and accelerometers, load biases in bias registers
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

    myIMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    SerialUSB.println("MPU9250 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    SerialUSB.print("AK8963 "); SerialUSB.print("I AM "); SerialUSB.print(d, HEX);
    SerialUSB.print(" I should be "); SerialUSB.println(0x48, HEX);

    // Get magnetometer calibration from AK8963 ROM
    myIMU.initAK8963(myIMU.magCalibration);
    // Initialize device for active mode read of magnetometer
    SerialUSB.println("AK8963 initialized for active data mode....");
    if (SerialDebug)
    {
      //  SerialUSB.println("Calibration values: ");
      SerialUSB.print("X-Axis sensitivity adjustment value ");
      SerialUSB.println(myIMU.magCalibration[0], 2);
      SerialUSB.print("Y-Axis sensitivity adjustment value ");
      SerialUSB.println(myIMU.magCalibration[1], 2);
      SerialUSB.print("Z-Axis sensitivity adjustment value ");
      SerialUSB.println(myIMU.magCalibration[2], 2);
    }

     calculateMagneticEnvironmentalBias();

  } // if (c == 0x71)
  else
  {
    SerialUSB.print("Could not connect to MPU9250: 0x");
    SerialUSB.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }

  //pinMode(IMU_PIN_TO_PROC, INPUT);
  //digitalWrite(IMU_PIN_TO_PROC, LOW);
  //attachInterrupt( digitalPinToInterrupt(IMU_PIN_TO_PROC), InterruptServiceRoutineMagnetometer , CHANGE);
  
}

double magnitude(float x, float y, float z) {
  return sqrt(x*x + y*y + z*z);
}

void loop()
{
  do 
  {  
    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    myIMU.getMres();
    
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0]*CONVERT_TO_MICROTESLA;
    myIMU.my = (float)myIMU.magCount[1]*CONVERT_TO_MICROTESLA;
    myIMU.mz = (float)myIMU.magCount[2]*CONVERT_TO_MICROTESLA;
  } while(!(myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01));

  int posThreshold = myIMU.magbias[0] + THRESHOLD_FROM_AVG;
  int negThreshold = myIMU.magbias[0] - THRESHOLD_FROM_AVG;
  if( myIMU.mx > posThreshold ) {
    if(!forwardFlag){
      countForward++;
      forwardFlag = true;

      if(!SerialDebug) {
        SerialUSB.print("FORWARD_EVENT_DETECTED Current Count: ");
        SerialUSB.println(countForward);
      }
      
    }
    
  }
  else{
    if(forwardFlag){
      forwardFlag = false;
    }
  }

  if( myIMU.mx < negThreshold) {
    if(!backwardFlag){
      countBackward++;
      backwardFlag = true;

      if(!SerialDebug) {
        SerialUSB.print("BACKWARD_EVENT_DETECTED Current Count: ");
        SerialUSB.println(countBackward);
      }
      
    }
   
  }
  else{
    if(backwardFlag){
      backwardFlag = false;
    }
  }

  if(SerialDebug) {
    SerialUSB.print(myIMU.mx);
    SerialUSB.print(",");
    SerialUSB.print(myIMU.my);
    SerialUSB.print(",");
    SerialUSB.print(myIMU.mz);
    //SerialUSB.print(",");
    //SerialUSB.print(magnitude(myIMU.mx, myIMU.my, myIMU.mz));
    SerialUSB.println();
  }
  delay(100);
}
