#include <Wire.h>
#include <MLX90393.h> //From https://github.com/tedyapo/arduino-MLX90393 by Theodore Yapo
#include <math.h>

MLX90393 mlx1;
MLX90393::txyz data1; //Create a structure, called data, of four floats (t, x, y, and z)

MLX90393 mlx2;
MLX90393::txyz data2;

MLX90393 mlx3;
MLX90393::txyz data3;

int count = 0;
int firstTimeFlag = 0; 

struct {

  float average;
  float total;
  float offset;

} zA, zB, zC;


//A is left, B is center, C is right (when looking head-on at BEATRIX)

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  mlx1.begin(0, 0); //Assumes I2C jumpers are GND. No DRDY pin used.
  mlx1.setOverSampling(0);
  mlx1.setDigitalFiltering(0);
  
  mlx2.begin(0, 1);
  mlx2.setOverSampling(0);
  mlx2.setDigitalFiltering(0);

  mlx3.begin(1, 0);
  mlx3.setOverSampling(0);
  mlx3.setDigitalFiltering(0);

}


void loop()
{
  
  mlx1.readData(data1);
  mlx2.readData(data2);
  mlx3.readData(data3);

  zA.total += data1.z;
  zB.total += data2.z;
  zC.total += data3.z;

  count++;

  if (count >= 50) {

    zA.average = round(zA.total / count);
    zA.total = 0;
    
    zB.average = round(zB.total / count);
    zB.total = 0;
    
    zC.average = round(zC.total / count);
    zC.total = 0;
    
    if (firstTimeFlag == 0) {

      // If this is the first time the sensor has been read, take these values and use them as offsets
      zA.offset = zA.average;
      zB.offset = zB.average;
      zC.offset = zC.average;


      firstTimeFlag = 1;

    }

    else {

      zA.average = zA.average - zA.offset;
      zB.average = zB.average - zB.offset;
      zC.average = zC.average - zC.offset;

    }

    count = 0;

    // If the Left sensor is largest value, and above 200 (indicating touch), then publish 'W' to turn Left
    if (zA.average > zB.average && zA.average > zC.average && zA.average > 200) {

      Serial.println("W");

    }

    else if (zB.average > zA.average && zB.average > zC.average && zB.average > 200) {

      Serial.println("N");

    }

    else if (zC.average > zA.average && zC.average > zB.average && zC.average > 200) {

      Serial.println("E");

    }

  }

}
