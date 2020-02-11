/**********************************************************************************
*                                                                                 *
*  Quadrino Nano Raw Sensor Values                                                *
*  -----------------------------------------------------------------------------  *
*                                                                                 *
*  INSTRUCTIONS:                                                                  *
*                                                                                 *
*  Type the number of the data you want to read into the serial monitor and       *
*  press enter. Some functions run continiously, press enter or type another      *
*  command to stop reading.                                                       *
*                                                                                 *
*   # | Sensor           | Data                                                   *
*  ---|------------------|------------------------------------------------------  *
*   1 | Venus838FLP      | GPS (Longitude, Latitude, Altitude, Satellite Count)   *
*  ---|------------------|------------------------------------------------------  *
*   2 | MPU9150          | Accelerometer (x,y,z)                                  *
*  ---|------------------|------------------------------------------------------  *
*   3 | MPU9150          | Gyroscope (x,y,z)                                      *
*  ---|------------------|------------------------------------------------------  *
*   4 | MPU9150          | Compass (x,y,z)                                        *
*  ---|------------------|------------------------------------------------------  *
*   5 | MS5611           | Barometer (Pressure & Altitude)                        *
*  ---|------------------|------------------------------------------------------  *
*   6 | MPU9150 & MS5611 | Temperature*                                           *
*  ---|------------------|------------------------------------------------------  *
*                                                                                 *
*  *Note about temperature: While both sensors can read temperature the           *
*   sensors themselves are too close to the processor inside the Quadrino         *
*   Nano resulting in readings that are accurate for the inside of the            *
*   Nano but wrong for the surrounding air.                                       *
*                                                                                 *
*  A note about serial communications:                                            *
*  -----------------------------------------------------------------------------  *
*  The serial communications used in this demonstration can be found in the       *
*  Arduino examples (File/Examples/04.Communication/SerialEvent) and an           *
*  explanation is availble here http://www.arduino.cc/en/Tutorial/SerialEvent     *
*                                                                                 *
**********************************************************************************/

//  Include Header Files
//  -----------------------------------------------------------------------------

#include <Wire.h>           // Include I2C communications
#include "Venus838FLP.h"    // GPS Functions
#include "MS5611.h"         // MS5611 Functions
#include "MPU9150.h"        // MPU9150 Functions


//  Setup Variables
//  -----------------------------------------------------------------------------

MS5611 ms5611;                   // Create MS5611 Object
double referencePressure;        // Variable to hold reference pressure, used by MS5611
String inputString = "";         // Serial Comm: a string to hold incoming data
boolean stringComplete = false;  // Serial Comm: whether the string is complete
int action = 0;                  // Serial Comm: value to hold our currently running action


void setup(){
  Serial.begin(57600);            // Start Serial Port at 57600 baud
  Wire.begin();                   // Initialize the 'Wire' class for the I2C-bus.
  printInstructions();            // Print out the instructions for using the tool
  inputString.reserve(200);       // SerialCommunications: reserve 200 bytes for the inputString:
  GPSModuleInit();                // Venus838FLP: Startup GPS Module
  GPSConfigureDefaults();         // Venus838FLP: Configure Default Values

  // Initialize MS5611 sensor
  while(!ms5611.begin()){
    Serial.println("Could not find a valid MS5611 sensor, check wiring!");
    delay(500);
  }
  referencePressure = ms5611.readPressure();      // MS5611: Get reference pressure for relative altitude
  MPU9150_writeSensor(MPU9150_PWR_MGMT_1, 0);     // MPU9150: Clear the 'sleep' bit to start the sensor.
  MPU9150_setupCompass();                         // MPU9150: Start the compass
}



void loop () {

  //  Serial Communications
  //  -----------------------------------------------------------------------------
  //  SerialEvent() {see bottom of this document} occurs whenever a new data comes
  //  in the hardware serial RX and sets "stringComplete" to true. All data is
  //  stored in "inputString". If we simply read the first character of
  //  "inputString" we will get the value that we typed into the serial monitor.
  //  We then set that as our action and let the switch statement take over.

  if (stringComplete) {
    action = inputString.charAt(0);
    inputString = "";
    stringComplete = false;
  }



  switch(action){
      case 49:
        printGPS();
        delay(1000);
      break;
      case 50:
        printAccelerometer();
        delay(25); // Updates at 40Hz
      break;
      case 51:
        printGyroscope();
        delay(10);
      break;
      case 52:
        printCompass();
        delay(10);
      break;
      case 53:
        printBarometer();
        Serial.println("");
        action = 0;
      break;
      case 54:
        printTemperature();
        Serial.println("");
        action = 0;
      break;
      case 10 || 13:
        Serial.println("");
        action = 0;
      break;
      default:

      break;
    }
}

//  Read and Print lat, lon, altitude and satelite count from Venus838FLP
//  -----------------------------------------------------------------------------

void printGPS(){
  Serial.print("GPS          ");
  VenusReadAndPrint(2000);
}


//  Read and Print the X, Y and Z accelerometer values from MPU9150
//  -----------------------------------------------------------------------------

void printAccelerometer(){
  Serial.print("MPU9150: ACCELEROMETER");
  Serial.print("           X = ");
  Serial.print(MPU9150_readSensor(MPU9150_ACCEL_XOUT_L,MPU9150_ACCEL_XOUT_H));
  Serial.print("           Y = ");
  Serial.print(MPU9150_readSensor(MPU9150_ACCEL_YOUT_L,MPU9150_ACCEL_YOUT_H));
  Serial.print("           Z = ");
  Serial.println(MPU9150_readSensor(MPU9150_ACCEL_ZOUT_L,MPU9150_ACCEL_ZOUT_H));
}


//  Read and Print the X, Y and Z gyroscope values from MPU9150
//  -----------------------------------------------------------------------------

void printGyroscope(){
  Serial.print("MPU9150: GYROSCOPE");
  Serial.print("           X = ");
  Serial.print(MPU9150_readSensor(MPU9150_GYRO_XOUT_L,MPU9150_GYRO_XOUT_H));
  Serial.print("           Y = ");
  Serial.print(MPU9150_readSensor(MPU9150_GYRO_YOUT_L,MPU9150_GYRO_YOUT_H));
  Serial.print("           Z = ");
  Serial.println(MPU9150_readSensor(MPU9150_GYRO_ZOUT_L,MPU9150_GYRO_ZOUT_H));
}


//  Read and Print the X, Y and Z compass values from MPU9150
//  -----------------------------------------------------------------------------

void printCompass(){
  Serial.print("MPU9150: COMPASS");
  Serial.print("           X = ");
  Serial.print(MPU9150_readSensor(MPU9150_CMPS_XOUT_L,MPU9150_CMPS_XOUT_H));
  Serial.print("           Y = ");
  Serial.print(MPU9150_readSensor(MPU9150_CMPS_YOUT_L,MPU9150_CMPS_YOUT_H));
  Serial.print("           Z = ");
  Serial.println(MPU9150_readSensor(MPU9150_CMPS_ZOUT_L,MPU9150_CMPS_ZOUT_H));
}


//  Read and Print Pressure and Altitude values from MS6511
//  -----------------------------------------------------------------------------

void printBarometer(){
  // MS5611

      // Read raw values
      uint32_t rawPressure = ms5611.readRawPressure();

      // Read true temperature & Pressure
      long realPressure = ms5611.readPressure();

      // Calculate altitude
      float absoluteAltitude = ms5611.getAltitude(realPressure);
      float relativeAltitude = ms5611.getAltitude(realPressure, referencePressure);

      Serial.println("\nPRESSURE & ALTITUDE");



      Serial.print("MS5611\t\t rawPressure = ");
      Serial.print(rawPressure);
      Serial.print("\t\t\t realPressure = ");
      Serial.print(realPressure);
      Serial.println(" Pa");

      Serial.print("MS5611\t\t absoluteAltitude = ");
      Serial.print(absoluteAltitude);
      Serial.print(" m\t\t relativeAltitude = ");
      Serial.print(relativeAltitude);
      Serial.println(" m\n");
}


//  Read & Print the temperature values from the MS5611 & MPU9150
//  -----------------------------------------------------------------------------

void printTemperature(){
  // MS5611
  Serial.println("\nTEMPERATURE");
  // Read raw values
  uint32_t rawTemp = ms5611.readRawTemperature();

  // Read true temperature & Pressure
  double realTemperature = ms5611.readTemperature();

  Serial.print("MS5611\t\t raw = ");
  Serial.print(rawTemp);
  Serial.print("\t\t interpreted = ");
  Serial.print(realTemperature);
  Serial.println(" *C");

  double dT = ( (double) MPU9150_readSensor(MPU9150_TEMP_OUT_L,MPU9150_TEMP_OUT_H));
  Serial.print("MPU9150\t\t raw = ");
  Serial.print((double)dT);
  Serial.print("\t\t interpreted = ");
  Serial.print(((double)dT + 12412.0) / 340.0, 3);
  Serial.println(" *C\n");
}


//  Print Usage instructions for this tool
//  -----------------------------------------------------------------------------

void printInstructions(){
  Serial.println("/**********************************************************************************\n*                                                                                 *\n*  Quadrino Nano Raw Sensor Values                                                *\n*  -----------------------------------------------------------------------------  *\n*                                                                                 *\n*  INSTRUCTIONS:                                                                  *\n*                                                                                 *\n*  Type the number of the data you want to read into the serial monitor and       *\n*  press enter. Some functions run continiously, press enter or type another      *\n*  command to stop reading.                                                       *\n*                                                                                 *\n*   # | Sensor           | Data                                                   *\n*  ---|------------------|------------------------------------------------------  *\n*   1 | Venus838FLP      | GPS (Longitude, Latitude, Altitude, Satellite Count)   *\n*  ---|------------------|------------------------------------------------------  *\n*   2 | MPU9150          | Accelerometer (x,y,z)                                  *\n*  ---|------------------|------------------------------------------------------  *\n*   3 | MPU9150          | Gyroscope (x,y,z)                                      *\n*  ---|------------------|------------------------------------------------------  *\n*   4 | MPU9150          | Compass (x,y,z)                                        *\n*  ---|------------------|------------------------------------------------------  *\n*   5 | MS5611           | Barometer (Pressure & Altitude)                        *\n*  ---|------------------|------------------------------------------------------  *\n*   6 | MPU9150 & MS5611 | Temperature*                                           *\n*  ---|------------------|------------------------------------------------------  *\n*                                                                                 *\n*  *Note about temperature: While both sensors can read temperature the           *\n*   sensors themselves are too close to the processor inside the Quadrino         *\n*   Nano resulting in readings that are accurate for the inside of the            *\n*   Nano but wrong for the surrounding air.                                       *\n*                                                                                 *\n**********************************************************************************/");
}


//  Serial Communications
//  -----------------------------------------------------------------------------
//  SerialEvent occurs whenever a new data comes in the hardware serial RX.
//  This routine is run between each time loop() runs, so using delay inside loop
//  can delay response.  Multiple bytes of data may be available.
//
//  SOURCE: http://www.arduino.cc/en/Tutorial/SerialEvent

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
