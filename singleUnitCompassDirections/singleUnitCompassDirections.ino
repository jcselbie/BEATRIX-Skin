#include <Wire.h>
#include <MLX90393.h> //From https://github.com/tedyapo/arduino-MLX90393 by Theodore Yapo

MLX90393 mlx;
MLX90393::txyz data; //Create a structure, called data, of four floats (t, x, y, and z)

int count = 0;
int firstTimeFlag = 0;
int calibrationCountdown = 5;
int overallCount = 0;
int previousTime = 0;
int currentTime = 0;
int softCalibratedFlag = 0;
int hardCalibratedFlag = 0;

struct {

  float average;
  float total;
  float offset;
  float previousVal;
  float soft;
  float hard;

} x, y, z;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  mlx.begin(0, 0); //Assumes I2C jumpers are GND. No DRDY pin used.
  mlx.setOverSampling(0);
  mlx.setDigitalFiltering(0);
}


void loop()
{

  mlx.readData(data); //Read the values from the sensor

  // You want to do a rolling average of each signal, so every millisecond, take the sensor reading and add it to the total
  // Then, after 50 of these, 
  x.total += data.x;
  y.total += data.y;
  z.total += data.z;

  if (count >= 50) {

    x.average = round(x.total / count);
    x.total = 0;
    
    y.average = round(y.total / count);
    y.total = 0;
    
    z.average = round(z.total / count);
    z.total = 0;
    
    if (firstTimeFlag == 0) {

      // If this is the first time the sensor has been read, take these values and use them as offsets
      x.offset = x.average;
      y.offset = y.average;
      z.offset = z.average;

      // Assign previousVal as ZERO for use in tracking the sensor reading changes to determine directional changes
      // (the values will be adjusted to zero anyways when calibration happens)
      x.previousVal = 0;
      y.previousVal = 0;
      z.previousVal = 0;

      firstTimeFlag = 1;

    }

    else {

      // Applies the calculated offset to start resting output of each axis at zero
      x.average -= x.offset;
      y.average -= y.offset;
      z.average -= z.offset;

    }

    // count is reset to zero to set up for next averaging loop
    count = 0;


    // Monitors the Serial port in case of user input from the computer
    if (Serial.available() > 0 ) {

      char input = Serial.read();

      // Calibrates the current values as a "Soft" touch
      if (input == 's') {
        
        if (!hardCalibratedFlag) {
          z.soft = z.average;
          softCalibratedFlag = 1;
        }

        else if (hardCalibratedFlag && z.average < z.hard) {
          z.soft = z.average;
          softCalibratedFlag = 1;
        }

        // Error catching for if the calibrated values are in the wrong order
        else if (hardCalibratedFlag && z.average >= z.hard) {
          //Serial.print("soft is set higher than hard!!");
        }
      }

      // Calibrates the current values as a "Hard" touch
      else if (input == 'h') {

        if (!softCalibratedFlag) {
          z.hard = z.average;
          hardCalibratedFlag = 1;
        }
        else if (softCalibratedFlag && z.average > z.soft) {
          z.hard = z.average;
          hardCalibratedFlag = 1;
        }
        // Error catching for if the calibrated values are in the wrong order
        else if (softCalibratedFlag && z.average <= z.soft) {
          //Serial.print("hard is set lower than soft");
        }
      }
      
      // This will re-zero the sensor in the event that the magnets move out of place/are replaced whilst the program is running
      else if (input == 'r') {

        x.offset += x.average;
        y.offset += y.average;
        z.offset += z.average;

      }

    }

    // The following if-else statment is to output the detected strength of the touch 
    // soft calib, NO hard calib
    if (abs(z.average) > abs(z.soft) && softCalibratedFlag && !hardCalibratedFlag) {

        //Serial.println("You're pressing soft :)");

    }

    // NO soft calib, hard calib
    else if (abs(z.average) > abs(z.hard) && !softCalibratedFlag && hardCalibratedFlag) {

        //Serial.println("You're pressing hard :(");

    }

    // BOTH calib, no input
    else if (abs(z.average) < abs(z.soft) && softCalibratedFlag && hardCalibratedFlag) {

        //continue;

    }

    // BOTH calib, soft input (in between hard and soft)
    else if (abs(z.average) > abs(z.soft) && abs(z.average) < abs(z.hard) && softCalibratedFlag && hardCalibratedFlag) {

      //Serial.println("quite nice actually :)");

    }

    else if (abs(z.average) > abs(z.hard) && softCalibratedFlag && hardCalibratedFlag) {

      //Serial.println("OWWWWW!!!!!!");

    }


    // This if-else statement returns the compass direction of the touch, relative to the central magnet
    if (x.average > 50 && z.average < -100) {

      Serial.println("N"); // NORTH (TOP)
      delay(1000);

    }

    else if (x.average < -50 && z.average < -100) {
      
      Serial.println("S"); // SOUTH (BOTTOM)
      delay(1000);

    }

    else if (y.average > 50 && z.average < -100) {

      Serial.println("W"); // WEST (LEFT)
      delay(1000);

    }

    else if (y.average < -50 && z.average < -100) {

      Serial.println("E"); // EAST (RIGHT)
      delay(1000);

    }

    Serial.print("X-value: ");
    Serial.print(x.average);
    Serial.print(",");
    Serial.print("Y-value: ");
    Serial.print(y.average);
    Serial.print(",");
    Serial.print("Z-value: ");
    Serial.println(z.average);

    // stores the current value as previous for use in the next loop in the event of vertical direction tracking
    x.previousVal = x.average;
    y.previousVal = y.average;
    z.previousVal = z.average;

  }

  delay(1);

  // increments count for use in averaging calculations
  count += 1;

}
