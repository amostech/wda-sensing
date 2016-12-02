
#define SerialDebug false  // Set to true to get Serial output for debugging

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling

//MPU9250 myIMU;

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
long int ti;

float gMagneticOffset = 0;
float gMAD = 0;

float gXmag = 0;
float gYmag = 0;
float gZmag = 0;

float THRESHOLD_1 = 99;
float THRESHOLD_2 = 99;

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

  //SerialUSB.print("Arr before sort: ");
  //printArray(arr,AVERAGING_BUFFER_SIZE);
  quickSortIterative(arr,0,AVERAGING_BUFFER_SIZE-1);
  //SerialUSB.print("Arr after sort: ");
  //printArray(arr,AVERAGING_BUFFER_SIZE);

  *median = arr[AVERAGING_BUFFER_SIZE/2];
  //SerialUSB.print("Median: ");
  //SerialUSB.println(*median,DEC);
  
  //Now create the Absolute Deviations Array
  for(i=0; i < AVERAGING_BUFFER_SIZE; i++)
     deviations[i] = abs(arr[i] - *median);

  //SerialUSB.print("Deviations Before Sort: ");
  //printArray(deviations,AVERAGING_BUFFER_SIZE);

  quickSortIterative(deviations,0,AVERAGING_BUFFER_SIZE-1);

  //SerialUSB.print("Deviations After Sort: ");
  //printArray(deviations,AVERAGING_BUFFER_SIZE);
  
  *mad = deviations[AVERAGING_BUFFER_SIZE/2]*1.4826; //ASSUME NORMAL DISTRIBUTION

}

void calculateMagneticEnvironmentalAverage() {
    
    int i = 0;
    int count = 0;
    float median = 0;
    float mad = 0;
    
    float avg_buff[AVERAGING_BUFFER_SIZE] = {0};

    uint8_t ST1;

    SerialUSB.print("Wait... Calculating Offsets.");
    SerialUSB.println("");
    for( i = 0; i < AVERAGING_BUFFER_SIZE; i++) {
      
          readSampleFromSerialPort(&gXmag, &gYmag, &gZmag);
          avg_buff[i] = magnitude( gXmag, gYmag, gZmag); 
    }

    if(USE_MAD) {
      
      MedianAD(avg_buff,&gMAD,&median);
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
    SerialUSB.println("Environmental Offset Calculated.");
    SerialUSB.print("Averaging Mode: ");
    if(USE_MAD)
      SerialUSB.println("MEDIAN_ABSOLUTE_DEVIATION");
    else
      SerialUSB.println("SIMPLE_AVERAGE");

    SerialUSB.print("Baseline: ");
    SerialUSB.println(gMagneticOffset,DEC);
    SerialUSB.print("MAD: ");
    SerialUSB.println(gMAD,DEC);

    SerialUSB.print("CALIBRATION_ACK");

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

bool readyToBegin = false; //THRESHOLDS_SET? 

void readSampleFromSerialPort(float* x, float* y, float* z) {

  String incomingLine = "";
  char charArr[1024] = {0};

  while(SerialUSB.available() <= 0) {
      SerialUSB.println("READ_REQUEST");
      delay(50);
  }

  incomingLine = SerialUSB.readStringUntil('\n');
  incomingLine.toCharArray(charArr,1024);
             
  if (sscanf(charArr, "%f,%f,%f", x, y, z) == 3) {
    // do something with r, g, b
    SerialUSB.print("x: ");
    SerialUSB.print(*x,DEC);
    SerialUSB.print(", y: ");
    SerialUSB.print(*y,DEC);
    SerialUSB.print(", z: ");
    SerialUSB.println(*z,DEC);
    }  
}

void setup()
{
  // Wire.begin();
  // TWBR = 12;  // 400 kbit/sec I2C speed
  while(!SerialUSB);
    
    SerialUSB.begin(115200);
    calculateMagneticEnvironmentalAverage();
}

void algorithmUnderTest(String incomingLine) {

  char charArr[1024] = {0};
  float mag = 0;

  incomingLine.toCharArray(charArr,1024);
        
  sscanf(charArr, "%f,%f,%f", &gXmag, &gYmag, &gZmag);
  
  mag = magnitude(gXmag, gYmag, gZmag);
  
  float posThreshold = gMagneticOffset + THRESHOLD_1*gMAD;
  float negThreshold = gMagneticOffset - THRESHOLD_1*gMAD;
    
  if( mag > posThreshold ) {
    if(!forwardFlag){
      countForward++;
      forwardFlag = true;
     }
   } else if ( forwardFlag && /*with sign!*/(posThreshold - mag) > THRESHOLD_2*gMAD ) {
    forwardFlag = false;
   }
}

void loop()
{

  int len = 0;
  String incomingLine = "";
  char charArr[1024] = {0};

  //if(SerialUSB.available()<=0)
  // return;
  
  incomingLine = SerialUSB.readStringUntil('\n');
  //SerialUSB.print(incomingLine);
  if( incomingLine.equals("THRESHOLD_SET") ) {
          incomingLine = SerialUSB.readStringUntil('\n');
          incomingLine.toCharArray(charArr,1024);
          if(sscanf(charArr, "%f,%f", &THRESHOLD_1, &THRESHOLD_2) == 2) {
            SerialUSB.print("THRESHOLD_ACK");
            SerialUSB.print(THRESHOLD_1);
            SerialUSB.print(",");
            SerialUSB.println(THRESHOLD_2);
        } 
  } else if( incomingLine.equals("GET_COUNT") ) {
        SerialUSB.print("COUNT_ACK,");
        SerialUSB.print(THRESHOLD_1);
        SerialUSB.print(",");
        SerialUSB.print(THRESHOLD_2);
        SerialUSB.print(",");
        SerialUSB.println(countForward);

        countForward = 0;

   } else {

      while(SerialUSB.available() <= 0) {
        SerialUSB.println("READ_REQUEST");
        delay(50);
      }
      
      algorithmUnderTest(incomingLine);
   }
}
