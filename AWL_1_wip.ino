#include <MD_DS3231.h>
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 4
#define DHTTYPE 22

DHT dht(DHTPIN, DHTTYPE);

SFE_BMP180 pressure;  //paineanturi


File myFile; //SD kortti, siihen kirjoittaminen yms.
File rdFile;
File opnNext;
char FileName[10];  //Serialista löytyvän filen nimi
char CharData[7];  //Serialista tulevan datan char

char FileNameArray[10]; //tiedoston nimi SD korttiin

#define ALTITUDE 34

//int i=0;

int Alarm;

void setup (){
  
  Serial.begin(9600);
  delay(2000);
  //Serial.println("S");
  
  pinMode(2, INPUT);

  if (pressure.begin())
    Serial.println("P");
  else {
    Serial.println("PFug");
    while(1);
  }
  Serial.print("SD");
  pinMode(10, OUTPUT);

  if (!SD.begin(10)){
    Serial.println("Fug");
  }
  else{
//  Serial.println("done!");
  }
  
//  Serial.print("H");
  
  dht.begin();
  
  Serial.println("d");
  
 /* 
  RTC.h = 20; //uncomment to set time if needed
  RTC.m = 4;
  RTC.s = 30;
  RTC.dd = 16;
  RTC.mm = 10;
  RTC.yyyy = 2017;
  RTC.writeTime();
  */
  //reset alarms:
  RTC.checkAlarm1();
  RTC.checkAlarm2();

}

void loop(){
  char status;
  double T,P,p0,a;
  

  Serial.println();
  Serial.println("Meas");

  RTC.readTime();

  float h = dht.readHumidity();
  
//  Serial.print("Humidity: ");
  Serial.println(h);

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
     // Serial.print("Temperature: ");
      Serial.print(T,2);
      //Serial.println("C");
      
      status = pressure.startPressure(3);
      if (status != 0)
      {

        delay(status);

        status = pressure.getPressure(P,T);

        if (status != 0)
        {
          p0 = pressure.sealevel(P,ALTITUDE);
     //     Serial.print("Pressure: ");
          Serial.print(p0,2);
     //     Serial.println(" mb");
         
        }
      }
    }
  }

 // i++; 
 
 snprintf(FileNameArray, sizeof(FileNameArray), "%02d%02d.txt", RTC.mm, RTC.dd); //tiedoston nimi tulee muotoon mmdd.txt

  myFile = SD.open(FileNameArray, FILE_WRITE);
  if (myFile){
    Serial.print("Writing");
  //  myFile.print(i);    //printtaa mittauskerran
  //  myFile.print(";");

    myFile.print(RTC.h, DEC);
    myFile.print(":");
    myFile.print(RTC.m, DEC);
    
    myFile.print(":");
    myFile.print(RTC.s, DEC);
    myFile.print(";");
    myFile.print(RTC.dd);
    myFile.print(".");
    myFile.print(RTC.mm);
    myFile.print(".");
    myFile.print(RTC.yyyy);
    myFile.print(";");

    myFile.print(T);
    myFile.print(";");
    myFile.print(p0);
    myFile.print(";");
    myFile.print(h);
    myFile.println();
    myFile.close();

    Serial.println("d");
    
   // Serial.print("Measured at ");
   // Serial.print("Measured ");
   // Serial.print(i);
   // Serial.print(" times at ");
  //  Serial.print(RTC.h);
  //  Serial.print(":");
  //  Serial.print(RTC.m);
  //  Serial.print(" o'clock ");
  //  Serial.print(RTC.dd);
  //  Serial.print(".");
  //  Serial.print(RTC.mm);
  //  Serial.print(".");
  //  Serial.println(RTC.yyyy);
  }
  else{
    Serial.println("FWrit");
  }

  //Serial.println("Waiting 2 seconds");
  //delay(2000);
  
  RTC.checkAlarm1();
  
  setalarm();
  
  while (RTC.checkAlarm2() != true){
    SerialData();
  }


}

void setalarm(){

  int Min;

  RTC.readTime();
  Min = RTC.m;
  //Hour = RTC.h;
  Min = Min + 30;
  if (Min >= 60){
    Min = Min - 60;
   // Hour = Hour + 1;
  }
  //if (Hour >= 24){
   // Hour = Hour - 24;  
    
  //}
 
  RTC.m = Min;
  //RTC.h = Hour;

  RTC.writeAlarm2(DS3231_ALM_M);

//  Serial.print("Alarm set to ");
  RTC.readAlarm2();
  //Serial.print(RTC.h);
 // Serial.print(":");
  Serial.println(RTC.m);
  


}

void SerialData(){
  if (Serial.available() != 0){
    delay(300);
    int j = 0;
    for (int a = Serial.available(); a != 0; a--){
     CharData[j] = Serial.read();
     j++;
    } 
   // Serial.println("Serial read");
    //Orders received in format XYYYY
    //where X is order and YYYY is filename without ending
    
    switch (CharData[0]){
     case 'l': //lists all files on SD-card  
       rdFile = SD.open("/");
       rdFile.rewindDirectory();
       while(true){
         opnNext = rdFile.openNextFile();
         if (! opnNext){
           Serial.println("d");
           opnNext.close();
           rdFile.close();
           break;
         }
         if (!opnNext.isDirectory()){
           Serial.print(opnNext.name());
           Serial.print("\n");
         //  Serial.print("\t");
         //  Serial.println(opnNext.size(), DEC);
           opnNext.close();
           //rdFile.close();
         }
       }
       break;
       
     case 's':  //send data from specified file
         getFileName();
         if (SD.exists(FileName)){
          rdFile = SD.open(FileName);
          while (rdFile.peek() !=-1){
            Serial.write(rdFile.read());  
            }
          rdFile.close();
          Serial.println();
         }
        else{
          Serial.println("Fug");
        }
        break;
        
     case 'r':  //remove specified file
         getFileName();
         if (SD.exists(FileName)){
           SD.remove(FileName);
           Serial.println("d");
         }
         else{
           Serial.println("Fug");
         }
         break;
         
     default:
         Serial.println("wut");
         break;
    }
    
  }
  
}

void getFileName(){
  snprintf(FileName, sizeof(FileName), "%c%c%c%c.txt", CharData[1], CharData[2], CharData[3], CharData[4]);
}  

