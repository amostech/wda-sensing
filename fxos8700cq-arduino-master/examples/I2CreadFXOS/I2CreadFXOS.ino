#include <Wire.h>
#include <FXOS8700CQ.h>
// the byte address is the address of the sensor for I2C
FXOS8700CQ sensor = FXOS8700CQ(0x1F);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initialize the FXOS8700CQ
  sensor.init();
}

void loop() {
  // Query the sensor
  do{
  sensor.readMagData();

  // Print out the data
  
  // Magnometer
  Serial.print("Mag ");
  Serial.print("X: ");
  Serial.print((int)sensor.magData.x);
  Serial.print(" Y: ");
  Serial.print((int)sensor.magData.y);
  Serial.print(" Z: ");
  Serial.println((int)sensor.magData.z);

  delay(100);
}while(1);
}
