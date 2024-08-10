// Accelerometer for testing Magnetometer Saver
// 
// 1. Create file "ACC_###.txt", where ### - next int number.
// 
// 2. Writes file header with info:
//    Acceleration data file.
//    Acceleration units: G
//    Time units: microseconds
//    Accelerometer: LIS3DSH
//    Accelerometer datarate: 100 Hz
//    Accelerometer range: 16G
//    Accelerometer perfomance: High Resolution 12bit
//    X;Y;Z;dt
// 
// 3. As fatst as possible asks data from accel and writes to file
// 
// Data sting example:
//    0.936;0.192;0.012;14596
//      G_x ; G_y ; G_z ; dt
// Where:
//    G_x - acceleration along the Ox axis in units of gravity acceleration (G)
//    G_y - acceleration along the Oy axis in units of gravity acceleration (G)
//    G_z - acceleration along the Oz axis in units of gravity acceleration (G)
//    dt - time step after previous data in microseconds
// 1G = 9.80665 m/s^2

// LIS3DSH connecting
//    VCC -> 3V3
//    GND -> GND
//    SCL -> A5
//    SDA -> A4
//    CS -> VCC (on LIS3DSH)

// SD (Troyka)
//    DI -> D11
//    DO -> D12
//    SCK -> D13
//    CS -> D10
//    V -> 5V
//    G -> GND

// LED -> D2 and GND

// LED signalization
// Blink 1 sec - Logging
// Setup errors:
//    1 blink every 500 ms - LIS3DH initialization failed!
//    2 blinks every 500 ms - SD initialization failed!
//    3 blinks every 500 ms - File creation failed!
//    4 blinks every 500 ms - Writting file header error!
// Loop errors:
//    10 blinks every 500 ms - File broken!
// For more info see Serial on 115200 bod


// SETTINGS
#define DEBUG

#define SD_CHIPSELECT 10
#define LED_PIN 2

#define ACCEL_ADDRESS     0x19
#define ACCEL_DATARATE    LIS3DH_DATARATE_100_HZ        // 1, 10, 25, 50, 100, 200, 400 Hz or POWERDOWN, LOWPOWER_5KHZ, LOWPOWER_1K6HZ
#define ACCEL_RANGE       LIS3DH_RANGE_16_G             // 2, 4, 8 or 16 G
#define ACCEL_PERFOMANCE  LIS3DH_MODE_HIGH_RESOLUTION   // NORMAL (10 bit), LOW_POWER (8 bit), HIGH_RESOLUTION (12 bit)

#ifdef DEBUG
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_INIT(x)     Serial.begin(x)
#else
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT(x)
  #define DEBUG_INIT(x)
#endif

#define FOREVER() for(;;)

#define FILENAME_LENGTH 11 // max = 13
#define FLOAT_PRECISION 3
#define BLINKING_DELAY_MS 1000

// LIBRARIES
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


// OBJECTS
Adafruit_LIS3DH lis = Adafruit_LIS3DH();
String filename;
File dataFile;


void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  DEBUG_INIT(115200);
  accelerometerSetup();
  SDSetup();

  DEBUG_PRINTLN("Logging...");
}

void loop() {

  printAccelToFile();
  blink();

}

void printAccelToFile() {
  static uint32_t iterationLength, timer;
  iterationLength = micros() - timer;
  timer = micros();
  
  lis.read();

  // try to print to file, if NOT succeed - error
  // print returns the number of bytes written. Must be >0
  if(!dataFile ||
    !dataFile.print(lis.x_g, FLOAT_PRECISION) ||
    !dataFile.print(";") ||
    !dataFile.print(lis.y_g, FLOAT_PRECISION) ||
    !dataFile.print(";") ||
    !dataFile.print(lis.z_g, FLOAT_PRECISION) ||
    !dataFile.print(";") ||
    !dataFile.print(iterationLength) ||
    !dataFile.println()) {


    DEBUG_PRINTLN("File broken!");
    errorLoop(10, 100, 100, 500);
  }

    dataFile.flush();

  // DEBUG_PRINTLN(iterationLength);
}



void blink() {
  static bool LEDflag = false;
  static uint16_t tmr;
  
  uint16_t ms = millis();
  if (ms - tmr > BLINKING_DELAY_MS) {
    tmr = ms;
    digitalWrite(LED_PIN, LEDflag);
    LEDflag = !LEDflag;

    DEBUG_PRINTLN("Logging...");
  }
}

void errorLoop(uint8_t count, int time_on_ms, int time_off_ms, int time_between_ms) {
  
  FOREVER() {
    for(uint8_t i = 0; i < count; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(time_on_ms);

      digitalWrite(LED_PIN, LOW);
      delay(time_off_ms);
    }

    delay(time_between_ms - time_off_ms);
  }

}


void accelerometerSetup(){
  // begining
  if (!lis.begin(ACCEL_ADDRESS)) {
    DEBUG_PRINTLN("LIS3DH initialization failed!");

    errorLoop(1, 100, 100, 500);
  }
  DEBUG_PRINTLN("LIS3DH initialization done.");

  // range
  lis.setRange(ACCEL_RANGE);
  DEBUG_PRINT("Range: ");
  DEBUG_PRINT(2 << lis.getRange());
  DEBUG_PRINTLN("G");

  // perfomance
  lis.setPerformanceMode(ACCEL_PERFOMANCE);
  Serial.print("Performance mode set to: ");
  switch (lis.getPerformanceMode()) {
    case LIS3DH_MODE_NORMAL:          DEBUG_PRINTLN("Normal 10bit"); break;
    case LIS3DH_MODE_LOW_POWER:       DEBUG_PRINTLN("Low Power 8bit"); break;
    case LIS3DH_MODE_HIGH_RESOLUTION: DEBUG_PRINTLN("High Resolution 12bit"); break;
  }
  
  // data rate
  lis.setDataRate(ACCEL_DATARATE);
  DEBUG_PRINT("Data rate: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ:            DEBUG_PRINTLN("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ:           DEBUG_PRINTLN("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ:           DEBUG_PRINTLN("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ:           DEBUG_PRINTLN("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ:          DEBUG_PRINTLN("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ:          DEBUG_PRINTLN("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ:          DEBUG_PRINTLN("400 Hz"); break;
    case LIS3DH_DATARATE_POWERDOWN:       DEBUG_PRINTLN("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ:   DEBUG_PRINTLN("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ:  DEBUG_PRINTLN("16 Khz Low Power"); break;
  }

  DEBUG_PRINTLN("LIS initialization done.");
  DEBUG_PRINTLN();
}


void SDSetup() {

  // SD initialization
  if (!SD.begin(SD_CHIPSELECT)) {
    DEBUG_PRINTLN(F("SD initialization failed!"));

    errorLoop(2, 100, 100, 500);
  }

  DEBUG_PRINTLN(F("SD initialization done."));

  // create new file
  filename.reserve(FILENAME_LENGTH);
  newFileName();

  dataFile = SD.open(filename, FILE_WRITE);
  
  if (!dataFile) {
    DEBUG_PRINT(F("File creation failed! Filename: "));
    DEBUG_PRINTLN(filename);

    errorLoop(3, 100, 100, 500);
  }

  DEBUG_PRINT(F("File created. Filename: "));
  DEBUG_PRINTLN(dataFile.name());

  // write header
  if(!writeHeaderToFile()) {
    DEBUG_PRINTLN(F("Writting file header error!"));

    errorLoop(4, 100, 100, 500);
  }
  dataFile.flush();

  DEBUG_PRINTLN(F("Writting file header done."));
  DEBUG_PRINTLN();
}


void newFileName() {
  int n = 0;
  getFilenameByNumber(n);

  while(SD.exists(filename)) {
    n++;
    getFilenameByNumber(n);
  }
}


void getFilenameByNumber(int n) {
  
  filename = "ACC_";
  
  if(n < 100) filename += '0';
  if(n < 10)  filename += '0';
  
  filename += n;
  filename += ".txt";
}


bool writeHeaderToFile() {

  dataFile.println(F("Acceleration data file."));

  dataFile.println(F("Acceleration units: G"));
  dataFile.println(F("Time units: microseconds"));

  dataFile.println(F("Accelerometer: LIS3DSH"));

  dataFile.print(F("Accelerometer datarate: "));
  switch (ACCEL_DATARATE) {
    case LIS3DH_DATARATE_1_HZ:            dataFile.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ:           dataFile.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ:           dataFile.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ:           dataFile.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ:          dataFile.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ:          dataFile.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ:          dataFile.println("400 Hz"); break;
    case LIS3DH_DATARATE_POWERDOWN:       dataFile.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ:   dataFile.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ:  dataFile.println("16 Khz Low Power"); break;
  }

  dataFile.print(F("Accelerometer range: "));
  dataFile.print(2 << LIS3DH_RANGE_16_G);
  dataFile.println("G");

  dataFile.print(F("Accelerometer perfomance: "));
  switch (ACCEL_PERFOMANCE) {
    case LIS3DH_MODE_NORMAL:          dataFile.println("Normal 10bit"); break;
    case LIS3DH_MODE_LOW_POWER:       dataFile.println("Low Power 8bit"); break;
    case LIS3DH_MODE_HIGH_RESOLUTION: dataFile.println("High Resolution 12bit"); break;
  }

  return dataFile.println("X;Y;Z;dt");
}
















































