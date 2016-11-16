/* MPU9250 Basic Example Code
 by: Kris Winer
 date: April 1, 2014
 license: Beerware - Use this code however you'd like. If you
 find it useful you can buy me a beer some time.
 Modified by Brent Wilkins July 19, 2016

 Demonstrate basic MPU-9250 functionality including parameterizing the register
 addresses, initializing the sensor, getting properly scaled accelerometer,
 gyroscope, and magnetometer data out. Added display functions to allow display
 to on breadboard monitor. Addition of 9 DoF sensor fusion using open source
 Madgwick and Mahony filter algorithms. Sketch runs on the 3.3 V 8 MHz Pro Mini
 and the Teensy 3.1.

 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors are on the EMSENSR-9250 breakout board.

 Hardware setup:
 MPU9250 Breakout --------- Arduino
 VDD ---------------------- 3.3V
 VDDI --------------------- 3.3V
 SDA ----------------------- A4
 SCL ----------------------- A5
 GND ---------------------- GND
 */

#include "quaternionFilters.h"
#include "MPU9250.h"


#ifdef LCD
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Using NOKIA 5110 monochrome 84 x 48 pixel display
// pin 9 - Serial clock out (SCLK)
// pin 8 - Serial data out (DIN)
// pin 7 - Data/Command select (D/C)
// pin 5 - LCD chip select (CS)
// pin 6 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(9, 8, 7, 5, 6);
#endif // LCD

#define AHRS true         // Set to false for basic data read
#define SerialDebug true  // Set to true to get Serial output for debugging

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling

MPU9250 myIMU;


/*WDA Specific Pins*/
#define IMU_PIN_TO_PROC 38

void InterruptServiceRoutineMagnetometer() {
  
  SerialUSB.println("====================================================> Interrupt CALLED!");

  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {  
   
    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    myIMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    myIMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    myIMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    myIMU.magbias[2] = +125.;

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0]*myIMU.mRes*myIMU.magCalibration[0] -
               myIMU.magbias[0];
    myIMU.my = (float)myIMU.magCount[1]*myIMU.mRes*myIMU.magCalibration[1] -
               myIMU.magbias[1];
    myIMU.mz = (float)myIMU.magCount[2]*myIMU.mRes*myIMU.magCalibration[2] -
               myIMU.magbias[2];
  }

  SerialUSB.print("x: ");
  SerialUSB.print(myIMU.mx);
  SerialUSB.print(",y: ");
  SerialUSB.print(myIMU.my);
  SerialUSB.print(",z: ");
  SerialUSB.print(myIMU.mz);
  SerialUSB.println();
}

void setup()
{
   Wire.begin();

  // TWBR = 12;  // 400 kbit/sec I2C speed
  while(!SerialUSB);
    SerialUSB.begin(9600);
  
  // Set up the interrupt pin, its set as active high, push-pull
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);
  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, HIGH);

#ifdef LCD
  display.begin(); // Ini8ialize the display
  display.setContrast(58); // Set the contrast

  // Start device display with ID of sensor
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0); display.print("MPU9250");
  display.setTextSize(1);
  display.setCursor(0, 20); display.print("9-DOF 16-bit");
  display.setCursor(0, 30); display.print("motion sensor");
  display.setCursor(20,40); display.print("60 ug LSB");
  display.display();
  delay(1000);

  // Set up for data display
  display.setTextSize(1); // Set text size to normal, 2 is twice normal etc.
  display.setTextColor(BLACK); // Set pixel color; 1 on the monochrome screen
  display.clearDisplay();   // clears the screen and buffer
#endif // LCD

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  SerialUSB.print("MPU9250 "); SerialUSB.print("I AM "); SerialUSB.print(c, HEX);
  SerialUSB.print(" I should be "); SerialUSB.println(0x71, HEX);

#ifdef LCD
  display.setCursor(20,0); display.print("MPU9250");
  display.setCursor(0,10); display.print("I AM");
  display.setCursor(0,20); display.print(c, HEX);
  display.setCursor(0,30); display.print("I Should Be");
  display.setCursor(0,40); display.print(0x71, HEX);
  display.display();
  delay(1000);
#endif // LCD

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

#ifdef LCD
    display.clearDisplay();

    display.setCursor(0, 0); display.print("MPU9250 bias");
    display.setCursor(0, 8); display.print(" x   y   z  ");

    display.setCursor(0,  16); display.print((int)(1000*accelBias[0]));
    display.setCursor(24, 16); display.print((int)(1000*accelBias[1]));
    display.setCursor(48, 16); display.print((int)(1000*accelBias[2]));
    display.setCursor(72, 16); display.print("mg");

    display.setCursor(0,  24); display.print(myIMU.gyroBias[0], 1);
    display.setCursor(24, 24); display.print(myIMU.gyroBias[1], 1);
    display.setCursor(48, 24); display.print(myIMU.gyroBias[2], 1);
    display.setCursor(66, 24); display.print("o/s");

    display.display();
    delay(1000);
#endif // LCD

    myIMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    SerialUSB.println("MPU9250 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    SerialUSB.print("AK8963 "); SerialUSB.print("I AM "); SerialUSB.print(d, HEX);
    SerialUSB.print(" I should be "); SerialUSB.println(0x48, HEX);

#ifdef LCD
    display.clearDisplay();
    display.setCursor(20,0); display.print("AK8963");
    display.setCursor(0,10); display.print("I AM");
    display.setCursor(0,20); display.print(d, HEX);
    display.setCursor(0,30); display.print("I Should Be");
    display.setCursor(0,40); display.print(0x48, HEX);
    display.display();
    delay(1000);
#endif // LCD

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

#ifdef LCD
    display.clearDisplay();
    display.setCursor(20,0); display.print("AK8963");
    display.setCursor(0,10); display.print("ASAX "); display.setCursor(50,10);
    display.print(myIMU.magCalibration[0], 2);
    display.setCursor(0,20); display.print("ASAY "); display.setCursor(50,20);
    display.print(myIMU.magCalibration[1], 2);
    display.setCursor(0,30); display.print("ASAZ "); display.setCursor(50,30);
    display.print(myIMU.magCalibration[2], 2);
    display.display();
    delay(1000);
#endif // LCD
  } // if (c == 0x71)
  else
  {
    SerialUSB.print("Could not connect to MPU9250: 0x");
    SerialUSB.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }


  pinMode(IMU_PIN_TO_PROC, INPUT);
  attachInterrupt( digitalPinToInterrupt(IMU_PIN_TO_PROC), InterruptServiceRoutineMagnetometer , HIGH);
  
}



void loop()
{
  SerialUSB.println("Going to Sleep...");
  pmc_enable_sleepmode(0);
  SerialUSB.println("I cant appear!!! ------->");
}
