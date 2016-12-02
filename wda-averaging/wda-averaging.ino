
#include "MPU9250.h"

#define SerialDebug true  // Set to true to get Serial output for debugging

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling

MPU9250 myIMU;

/*WDA Specific Pins*/
#define IMU_PIN_TO_PROC 38
#define AVERAGING_BUFFER_SIZE 100
#define USE_MAD true

#define THRESHOLD_FROM_AVG 35
#define CONVERT_TO_MICROTESLA 4912./8190. //microTesla


int countForward = 0;
int countBackward = 0;
bool forwardFlag = 0;
bool backwardFlag = 0;

float gMagneticOffset = 0;

float magnitude(float x, float y, float z) {
  return sqrt(x*x + y*y + z*z);
}

void printArray(float a[], int l){
  int i = 0;
  SerialUSB.print("[ ");
  for(i = 0; i < l; i++) {
    SerialUSB.print(a[i]);
    SerialUSB.print(" ");
  }
  SerialUSB.println(" ]");
}


void MedianAD(float arr[], float* mad /*where to store mad*/, float* median /*where to store the median*/) {
  
  int i = 0;
  float deviations[AVERAGING_BUFFER_SIZE] = {0};

  SerialUSB.print("Arr before sort: ");
  printArray(arr,AVERAGING_BUFFER_SIZE);
  quickSortIterative(arr,0,AVERAGING_BUFFER_SIZE-1);
  SerialUSB.print("Arr after sort: ");
  printArray(arr,AVERAGING_BUFFER_SIZE);

  *median = arr[AVERAGING_BUFFER_SIZE/2];
  SerialUSB.print("Median: ");
  SerialUSB.println(*median,DEC);
  
  //Now create the Absolute Deviations Array
  for(i=0; i < AVERAGING_BUFFER_SIZE; i++)
     deviations[i] = abs(arr[i] - *median);

  SerialUSB.print("Deviations Before Sort: ");
  printArray(deviations,AVERAGING_BUFFER_SIZE);

  quickSortIterative(deviations,0,AVERAGING_BUFFER_SIZE-1);

  SerialUSB.print("Deviations After Sort: ");
  printArray(deviations,AVERAGING_BUFFER_SIZE);
  
  *mad = deviations[AVERAGING_BUFFER_SIZE/2]*1.4826; //ASSUME NORMAL DISTRIBUTION

}

void calculateMagneticEnvironmentalAverage() {
    
    int i = 0;
    int count = 0;
    float median = 0;
    float mad = 0;


    float avg_buff[AVERAGING_BUFFER_SIZE] = {0};
    
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    myIMU.magbias[0] = 0;
    // User environmental x-axis correction in milliGauss TODO axis??
    myIMU.magbias[1] = 0;
    // User environmental x-axis correction in milliGauss
    myIMU.magbias[2] = 0;

    SerialUSB.print("Wait... Calculating Offsets.");
    SerialUSB.println("");
    for( i = 0; i < AVERAGING_BUFFER_SIZE; i++) {
      while(!(myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01));
     
        myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
        
        avg_buff[i] = magnitude((float) myIMU.magCount[0], (float) myIMU.magCount[1], (float) myIMU.magCount[2]);
       
    }

    if(USE_MAD) {
      
      MedianAD(avg_buff,&mad,&median);

      SerialUSB.print("Calculated MAD: ");
      SerialUSB.println(mad,DEC);
      
      for( i = 0; i < AVERAGING_BUFFER_SIZE; i++) {

          if( avg_buff[i] >= (median-mad) && avg_buff[i] <= (median+mad) ) {
            gMagneticOffset += avg_buff[i];
            count++;
          }
      }
    } else {
      //SIMPLE AVERAGING!!!!
      for( i = 0; i < AVERAGING_BUFFER_SIZE; i++) {
            gMagneticOffset += avg_buff[i];
            count++;
      }
    }

    gMagneticOffset /= count;
    SerialUSB.print("Environmental Offset Calculated.");
    SerialUSB.print("Averaging Mode: ");
    if(USE_MAD)
      SerialUSB.println("MEDIAN_ABSOLUTE_DEVIATION");
    else
      SerialUSB.println("SIMPLE_AVERAGE");

    SerialUSB.print("Offset: ");
    SerialUSB.println(gMagneticOffset,DEC);

}

void swap ( float* a, float* b )
{
    float t = *a;
    *a = *b;
    *b = t;
}
 
/* This function is same in both iterative and recursive*/
int partition (float arr[], int l, int h)
{
    float x = arr[h];
    int i = (l - 1);
 
    for (int j = l; j <= h- 1; j++)
    {
        if (arr[j] <= x)
        {
            i++;
            swap (&arr[i], &arr[j]);
        }
    }
    swap (&arr[i + 1], &arr[h]);
    return (i + 1);
}
 
/* A[] --> Array to be sorted, 
   l  --> Starting index, 
   h  --> Ending index */
void quickSortIterative (float arr[], int l, int h)
{
    // Create an auxiliary stack
    float stack[ h - l + 1 ];
 
    // initialize top of stack
    int top = -1;
 
    // push initial values of l and h to stack
    stack[ ++top ] = l;
    stack[ ++top ] = h;
 
    // Keep popping from stack while is not empty
    while ( top >= 0 )
    {
        // Pop h and l
        h = stack[ top-- ];
        l = stack[ top-- ];
 
        // Set pivot element at its correct position
        // in sorted array
        int p = partition( arr, l, h );
 
        // If there are elements on left side of pivot,
        // then push left side to stack
        if ( p-1 > l )
        {
            stack[ ++top ] = l;
            stack[ ++top ] = p - 1;
        }
 
        // If there are elements on right side of pivot,
        // then push right side to stack
        if ( p+1 < h )
        {
            stack[ ++top ] = p + 1;
            stack[ ++top ] = h;
        }
    }
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

  } // if (c == 0x71)
  else
  {
    SerialUSB.print("Could not connect to MPU9250: 0x");
    SerialUSB.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }
  
}

void loop()
{

  calculateMagneticEnvironmentalAverage();
  
  delay(100);
}
