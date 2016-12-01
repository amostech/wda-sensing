#include <Wire.h>
#include <math.h>

#include <FXOS8700CQ.h>

// Public Methods //////////////////////////////////////////////////////////////

FXOS8700CQ::FXOS8700CQ(byte addr)
{
	address = addr;
	accelFSR = AFS_2g;     // Set the scale below either 2, 4 or 8
	accelODR = AODR_200HZ; // In hybrid mode, accel/mag data sample rates are half of this value
	magOSR = MOSR_7;     // Choose magnetometer oversample rate
}

// Writes a register
void FXOS8700CQ::writeReg(byte reg, byte value)
{
	Wire1.beginTransmission(address);
	Wire1.write(reg);
	Wire1.write(value);
	Wire1.endTransmission();
}

// Reads a register
byte FXOS8700CQ::readReg(byte reg)
{
	byte value;

	Wire1.beginTransmission(address);
	Wire1.write(reg);
	Wire1.endTransmission(false);
	Wire1.requestFrom(address, (uint8_t)1);
	value = Wire1.read();
	Wire1.endTransmission();

	return value;
}

void FXOS8700CQ::readRegs(byte reg, uint8_t count, byte dest[])
{
	uint8_t i = 0;

	Wire1.beginTransmission(address);   // Initialize the Tx buffer
	Wire1.write(reg);            	   // Put slave register address in Tx buffer
	Wire1.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
	Wire1.requestFrom(address, count);  // Read bytes from slave register address 

	while (Wire1.available()) {
		dest[i++] = Wire1.read();   // Put read results in the Rx buffer
	}
}

// Read the accelerometer data
void FXOS8700CQ::readAccelData()
{
	uint8_t rawData[6];  // x/y/z accel register data stored here

	readRegs(FXOS8700CQ_OUT_X_MSB, 6, &rawData[0]);  // Read the six raw data registers into data array
	accelData.x = ((int16_t) rawData[0] << 8 | rawData[1]) >> 2;
	accelData.y = ((int16_t) rawData[2] << 8 | rawData[3]) >> 2;
	accelData.z = ((int16_t) rawData[4] << 8 | rawData[5]) >> 2;
}

// Read the magnometer data
void FXOS8700CQ::readMagData()
{
	uint8_t rawData[6];  // x/y/z accel register data stored here

	readRegs(FXOS8700CQ_M_OUT_X_MSB, 6, &rawData[0]);  // Read the six raw data registers into data array
	magData.x = ((int16_t) rawData[0] << 8 | rawData[1]);
	magData.y = ((int16_t) rawData[2] << 8 | rawData[3]);
	magData.z = ((int16_t) rawData[4] << 8 | rawData[5]);
}

// Read the temperature data
void FXOS8700CQ::readTempData()
{
	tempData = readReg(FXOS8700CQ_TEMP);
}

// Put the FXOS8700CQ into standby mode.
// It must be in standby for modifying most registers
void FXOS8700CQ::standby()
{
	byte c = readReg(FXOS8700CQ_CTRL_REG1);
	writeReg(FXOS8700CQ_CTRL_REG1, c & ~(0x01));
}

// Put the FXOS8700CQ into active mode.
// Needs to be in this mode to output data.
void FXOS8700CQ::active()
{
	byte c = readReg(FXOS8700CQ_CTRL_REG1);
	writeReg(FXOS8700CQ_CTRL_REG1, c | 0x01);
}

void FXOS8700CQ::init()
{
	standby();  // Must be in standby to change registers
	delay(10);
	
	// Configure the accelerometer
	writeReg(FXOS8700CQ_XYZ_DATA_CFG, accelFSR);  // Choose the full scale range to 2, 4, or 8 g.
	delay(10);
	//writeReg(FXOS8700CQ_CTRL_REG1, readReg(FXOS8700CQ_CTRL_REG1) & ~(0x38)); // Clear the 3 data rate bits 5:3
	if (accelODR <= 7) 
		writeReg(FXOS8700CQ_CTRL_REG1, readReg(FXOS8700CQ_CTRL_REG1) | (accelODR << 3));
	delay(10);
	//writeReg(FXOS8700CQ_CTRL_REG2, readReg(FXOS8700CQ_CTRL_REG2) & ~(0x03)); // clear bits 0 and 1
	//writeReg(FXOS8700CQ_CTRL_REG2, readReg(FXOS8700CQ_CTRL_REG2) |  (0x02)); // select normal(00) or high resolution (10) mode
	
	// Configure the magnetometer
	writeReg(FXOS8700CQ_M_CTRL_REG1, 0x80 | magOSR << 2 | 0x01); // Set auto-calibration, set oversampling, enable hybrid mode 
	delay(10);
		                                     
	// Configure interrupts 1 and 2
	//writeReg(CTRL_REG3, readReg(CTRL_REG3) & ~(0x02)); // clear bits 0, 1 
	//writeReg(CTRL_REG3, readReg(CTRL_REG3) |  (0x02)); // select ACTIVE HIGH, push-pull interrupts    
	//writeReg(CTRL_REG4, readReg(CTRL_REG4) & ~(0x1D)); // clear bits 0, 3, and 4
	//writeReg(CTRL_REG4, readReg(CTRL_REG4) |  (0x1D)); // DRDY, Freefall/Motion, P/L and tap ints enabled  
	//writeReg(CTRL_REG5, 0x01);  // DRDY on INT1, P/L and taps on INT2

	active();  // Set to active to start reading
	delay(10);
}

// Get accelerometer resolution
float FXOS8700CQ::getAres(void)
{
	switch (accelFSR)
	{
		// Possible accelerometer scales (and their register bit settings) are:
		// 2 gs (00), 4 gs (01), 8 gs (10). 
		case AFS_2g:
			return 2.0/8192.0;
		break;
		case AFS_4g:
			return 4.0/8192.0;
		break;
		case AFS_8g:
			return 8.0/8192.0;
		break;
	}

	return 0.0;
}

// Get magnometer resolution
float FXOS8700CQ::getMres(void)
{
	return 10./32768.;
}
// Private Methods //////////////////////////////////////////////////////////////
