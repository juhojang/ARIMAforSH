
long Duration = 0; // 정수

// float Duration = 0; //소수점 2자리

int i=1;

#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
 
File myFile;

BH1750 lightMeter;
 
#define DHTPIN 6        // SDA 핀의 설정
#define DHTTYPE DHT22   // DHT22 (AM2302) 센서종류 설정

#define analogPinForRV    1   //  change to pins you the analog pins are using

#define analogPinForTMP   0

const float zeroWindAdjustment =  .2; // negative numbers yield smaller wind speeds and vice versa.

 

int TMP_Therm_ADunits;  //temp termistor value from wind sensor

float RV_Wind_ADunits;    //RV output from wind sensor 

float RV_Wind_Volts;

unsigned long lastMillis;

int TempCtimes100;

float zeroWind_ADunits;

float zeroWind_volts;

float WindSpeed_MPH;

DHT dht(DHTPIN, DHTTYPE);

void setup(){
  
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  lightMeter.begin();

}

void loop(){ 
  Serial.println("time,air_temp,air_humid,wind,lux,soil_humid");
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float lux = lightMeter.readLightLevel();
  int Soil_moisture = analogRead(A3);
  int sm_percentage=map(Soil_moisture,330,1023,100,0)  ;


  int j; 

  j=0;
  float buffer=0;
  while(j<1200){
      if (millis() - lastMillis > 200){      // read every 200 ms - printing slows this down further

    

    TMP_Therm_ADunits = analogRead(analogPinForTMP);

    RV_Wind_ADunits = analogRead(analogPinForRV);

    RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);

 

    // these are all derived from regressions from raw data as such they depend on a lot of experimental factors

    // such as accuracy of temp sensors, and voltage at the actual wind sensor, (wire losses) which were unaccouted for.

    TempCtimes100 = (0.005 *((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;  

 

    zeroWind_ADunits = -0.0006*((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172;  //  13.0C  553  482.39

 

    zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;  

 

    // This from a regression from data in the form of 

    // Vraw = V0 + b * WindSpeed ^ c

    // V0 is zero wind at a particular temperature

    // The constants b and c were determined by some Excel wrangling with the solver.

    

   WindSpeed_MPH =  pow(((RV_Wind_Volts - zeroWind_volts) /.2300) , 2.7265);   

  }
  if(isnan(WindSpeed_MPH)){
    continue;
  }
  buffer=buffer+(float)WindSpeed_MPH;
  j++;
  if(i==1){
    break;
  }
  delay(500);
  }


  Serial.println("SD card module initialization..");
  if(!SD.begin(8)) {
    while(SD.begin(8)){
      Serial.println("initialization fail");
    }
  }
  Serial.println("initialization success");
 
  myFile = SD.open("TEST.CSV", FILE_WRITE);
  if(!myFile){
    while(myFile){
      myFile = SD.open("TEST.CSV", FILE_WRITE);
    }
  }
    Serial.print(i);
    Serial.print(",");
    Serial.print(t);
    Serial.print(",");
    Serial.print(h);
    Serial.print(",");
    Serial.print(buffer);
    Serial.print(",");
    Serial.print(lux);
    Serial.print(",");
    Serial.println(sm_percentage);
  if (myFile) {
    if(i==1){
      myFile.println("time,air_temp,air_humid,wind,lux,soil_humid");
    }
    Serial.print("SOIL_HUMID.CSV 파일 작성 중..");
    myFile.print(i);
    myFile.print(",");
    myFile.print(t);
    myFile.print(",");
    myFile.print(h);
    myFile.print(",");
    myFile.print(buffer);
    myFile.print(",");
    myFile.print(lux);
    myFile.print(",");
    myFile.println(sm_percentage);
    myFile.close();
    Serial.println("complete");
  } else {
    Serial.println("error opening test.txt");
  }
  i++;
}