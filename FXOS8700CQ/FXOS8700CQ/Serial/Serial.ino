#include <Wire.h>
#include <FXOS8700CQ.h>

#define FXOS8700CQ_ADDR 0x1E

FXOS8700CQ sensor = FXOS8700CQ(FXOS8700CQ_ADDR);
//FXOS8700CQ sensor2 = FXOS8700CQ(0x68);

void setup() {
  while(!SerialUSB);
  SerialUSB.begin(9600);
  SerialUSB.println("hi");
  Wire1.begin();
  Wire1.setClock(400000L);
  
  // Initialize the FXOS8700CQ
  //sensor.init();

  byte dataByte;
  dataByte = sensor.readReg(FXOS8700CQ_WHO_AM_I);
  SerialUSB.print("Who Am I: ");
  SerialUSB.println(dataByte);
  /*
  dataByte = 0x00;
  sensor.writeReg(FXOS8700CQ_CTRL_REG1, dataByte);
  delay(10);

  dataByte = 0x1D;
  sensor.writeReg(FXOS8700CQ_M_CTRL_REG1, dataByte);
  delay(10);

  dataByte = 0x20;
  sensor.writeReg(FXOS8700CQ_M_CTRL_REG2, dataByte);
  delay(10);

  dataByte = 0x01;
  sensor.writeReg(FXOS8700CQ_XYZ_DATA_CFG, dataByte);
  delay(10);

  dataByte = 0x0D;
  sensor.writeReg(FXOS8700CQ_CTRL_REG1, dataByte);
  delay(10);
  */
}

void loop() {
  // Query the sensor
  //sensor.readAccelData();
  sensor.readMagData();
  /*
  // Print out the data
  // Accelerometer
  SerialUSB.print("Accel ");
  SerialUSB.print("X: ");
  SerialUSB.print((int)sensor.accelData.x);
  SerialUSB.print(" Y: ");
  SerialUSB.print((int)sensor.accelData.y);
  SerialUSB.print(" Z: ");
  SerialUSB.println((int)sensor.accelData.z);
  */
  // Magnometer
  SerialUSB.print("Mag ");
  SerialUSB.print("X: ");
  SerialUSB.print((int)sensor.magData.x);
  SerialUSB.print(" Y: ");
  SerialUSB.print((int)sensor.magData.y);
  SerialUSB.print(" Z: ");
  SerialUSB.println((int)sensor.magData.z);

  //delay(100);
}
