// I2C interface by default
//
#include "Wire.h"
#include "SparkFunIMU.h"
#include "SparkFunLSM303C.h"
#include "LSM303CTypes.h"

// #define DEBUG 1 in SparkFunLSM303C.h turns on debugging statements.
// Redefine to 0 to turn them off.

LSM303C myIMU;

int interruptPin = 58;
int carCount = 0;
long int ti;

#define AVERAGING_BUFFER_SIZE 1000
#define THRESHOLD 7

int gMagOffsetX = 0;
int gMagOffsetY = 0;
int gMagOffsetZ = 0;
int gMAD = 0;

#define PLOT false

float magnitude(float x, float y, float z) {
  return sqrt(x*x + y*y + z*z);
}

/*
* MAD
*/

void printArray(int a[], int l){
  int i = 0;
  SerialUSB.print("[ ");
  for(i = 0; i < l; i++) {
    SerialUSB.print(a[i]);
    SerialUSB.print(" ");
  }
  SerialUSB.println(" ]");
}

void MedianAD(int arr[], int* mad /*where to store mad*/, int* median /*where to store the median*/) {
  
  int i = 0;
  int deviations[AVERAGING_BUFFER_SIZE] = {0};

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
    int median = 0;
    int mad = 0;
    
    //float avg_buff_x[AVERAGING_BUFFER_SIZE] = {0};
    //float avg_buff_y[AVERAGING_BUFFER_SIZE] = {0};
    int avg_buff_z[AVERAGING_BUFFER_SIZE] = {0};

    uint8_t ST1;

    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    //myIMU.magbias[0] = 0;
    // User environmental x-axis correction in milliGauss TODO axis??
    //myIMU.magbias[1] = 0;
    // User environmental x-axis correction in milliGauss
    //myIMU.magbias[2] = 0;

    SerialUSB.print("Wait... Calculating Offsets.");
    SerialUSB.println("");
    for( i = 0; i < AVERAGING_BUFFER_SIZE; i++){
      AxesRaw_t axes;
      myIMU.MAG_GetMagRaw(axes);
      avg_buff_z[i] = axes.yAxis;
    }
        
      
      MedianAD(avg_buff_z,&gMAD,&median);
      for( i = 0; i < AVERAGING_BUFFER_SIZE; i++) {

          if( avg_buff_z[i] >= (median-mad) && avg_buff_z[i] <= (median+mad) ) {
            gMagOffsetZ += avg_buff_z[i];
            count++;
          }
      }
      


    gMagOffsetZ /= count;
    SerialUSB.println("Environmental Offset Calculated.");
    SerialUSB.print("Averaging Mode: ");
    SerialUSB.println("MEDIAN_ABSOLUTE_DEVIATION");

    SerialUSB.print("Baseline: ");
    SerialUSB.println(gMagOffsetZ,DEC);
    SerialUSB.print("MAD: ");
    SerialUSB.println(gMAD,DEC);
}

void swap ( int* a, int* b )
{
    int t = *a;
    *a = *b;
    *b = t;
}
 
/* This function is same in both iterative and recursive*/
int partition (int arr[], int l, int h)
{
    int x = arr[h];
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
void quickSortIterative (int arr[], int l, int h)
{
    // Create an auxiliary stack
    int stack[ h - l + 1 ];
 
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

/*
* END MAD
*/

void interrupt(){
  
  uint8_t intSource;
  myIMU.readMagInterruptSource(intSource);
  
  if(!(intSource&0x48)) {
    return;
  }

  if(ti != millis()){
    carCount++;
    ti = millis();
    SerialUSB.print(" count ");
    SerialUSB.println(carCount,DEC);
  }
  
}

void setup()
{
  
  int i;
  while(!SerialUSB);
  SerialUSB.begin(115200);
  if (myIMU.begin() != IMU_SUCCESS)
  {
    SerialUSB.println("Failed setup.");
    while(1);
  }

  myIMU.interruptMagDisable();
  calculateMagneticEnvironmentalAverage();

  int th;
  if (gMagOffsetZ < 0){
    th = (gMagOffsetZ - THRESHOLD*gMAD);
  }
  else {
    th = (gMagOffsetZ + THRESHOLD*gMAD);
  }

  int16_t th_int = (int16_t)(th);
  if(th_int < 0){
    th_int *= -1;
  }
 // uint16_t iMagOffsetZ = (uint16_t)th_int;
  uint16_t iMagOffsetZ =  (uint16_t)th_int;//= (th_int >= 0) ? ((uint16_t)th_int) : ((uint16_t)(-1*th_int));
  
  SerialUSB.print("Float Offset: ");
  SerialUSB.println( gMagOffsetZ, DEC );
  SerialUSB.print("Float adjusted offset: ");
  SerialUSB.println( th, DEC );
  SerialUSB.print("th_int: ");
  SerialUSB.println( th_int, DEC );
  SerialUSB.print("uint16 offset: ");
  SerialUSB.println( iMagOffsetZ, DEC );
  SerialUSB.print("MAG_INT_THS_L: ");
  
  SerialUSB.println( (iMagOffsetZ & 0xFF), HEX);
  SerialUSB.print("MAG_INT_THS_H: ");
  SerialUSB.println(   (iMagOffsetZ >> 8 ) & 0x7F  , HEX);
  
  if(!PLOT) {
    myIMU.interruptMagEnable(iMagOffsetZ);
    pinMode(interruptPin,INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt, LOW);
  } else {
    //save initial time for plots
    ti = millis();
  }
  
}

void loop()
{

  if(!PLOT) {
    
    SerialUSB.println("Entering sleep mode...");
    pmc_enable_waitmode();
 
  } else {
    
    float x=0,y=0,z=0;

    SerialUSB.print(millis()-ti,DEC);
    SerialUSB.print(",");
    x = myIMU.readMagX();
    if ( !isnan(x) )
    {
      SerialUSB.print(x, 4);
      SerialUSB.print(",");
      SerialUSB.print((y=myIMU.readMagY()), 4);
      SerialUSB.print(",");
      SerialUSB.print((z=myIMU.readMagZ()), 4);
      SerialUSB.print(",");
      SerialUSB.println(magnitude(x,y,z) );
    }
  
    delay(10);
  }
  
}
