#include <SFE_BMP180.h>   //paine ja lämpötila anturin lib
#include <Wire.h>         //sama

SFE_BMP180 pressure;      //

#define ALTITUDE 37     //laita tähän nykyinen korkeus metreissä

void setup() {
 
  

}

void loop() {
  // put your main code here, to run repeatedly:
#include <MD_DS1307.h>    //RTC lib
#include <SD.h>           //SD kortin lib
#include <SPI.h>
#include <SFE_BMP180.h>   //paine ja lämpötila anturin lib
#include <Wire.h>         //sama

SFE_BMP180 pressure;      //

#define ALTITUDE 37     //laita tähän nykyinen korkeus metreissä

void setup() {


}


void loop() {
 

 
 String dataString = "";  //luodaan stringi, mihin laitetaan data

 Aika();


}

void Aika(){            //Ottaa RTC:ltä nykyisen ajan muodossa
  dataString += RTC.h   // hh:mm;dd.mm.yyyy ja lisää dataStringiin
  datastring +=":"
  dataString += RTC.m
  datastring +=";"
  dataString += RTC.dd
  datastring +="."
  dataString += RTC.mm
  datastring +="."
  dataString += RTC.yyyy
}

void dataPrint{ //printtaa SD kortille dataStingin
   SD.open("test.txt", FILE_WRITE);  
   dataFile.println(dataString)
   dataFile.close
}



}
