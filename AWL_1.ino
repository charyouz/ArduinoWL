#include <MD_DS3231.h>    //RTC lib
#include <SD.h>           //SD kortin lib
#include <SPI.h>
#include <SFE_BMP180.h>   //paine ja lämpötila anturin lib
#include <Wire.h>         //sama

SFE_BMP180 pressure;      //

#define ALTITUDE 37;     //laita tähän nykyinen korkeus metreissä

String dataString;       //luodaan stringi datalle

File dataFile;

int i = 0;    //käytetään looppaamaan. koitetaan muistaa nollata

void setup() {


}


void loop() {
 
 
 String dataString = "";  //tyhjennetään stringi

 Aika();

 TempPres();

 dataPrint();

 return(0);

}

void Aika(){            //Ottaa RTC:ltä nykyisen ajan muodossa
  dataString += RTC.h;   // hh:mm;dd.mm.yyyy ja
  dataString +=":";      //lisää dataStringiin
  dataString += RTC.m;
  dataString +=";";
  dataString += RTC.dd;
  dataString +=".";
  dataString += RTC.mm;
  dataString +=".";
  dataString += RTC.yyyy;
  return(0);
}

void TempPres(){
  tempfail:   //tullaan tähän, jos temp mittaus failaa
  char status;
  double T,P,P0,a;
  pressure.begin(); //käynnistää moduulin
  status = pressure.startTemperature();
  /*aloittaa mittauksen ja antaa arvon, minkä mittaus 
   * kestää ms, tai 0, jos ei toimi
   */
  if (status == 0)    //jos ei toimi, kokeilee uudestaan                
  {                   //max 3 kertaa
    for ( i; i <= 2; i++){
      goto tempfail;   //aloittaa alusta
  }
      dataString +="TEMPERROR;";    //printataan faili
      i = 0;   //suht tärkeetä myöhemmin...
  }
  delay(status);    //odottaa mittauksen keston ajan

  status = pressure.getTemperature(T);
  dataString += T;
  dataString +=";";
  
  }


void dataPrint(){ //printtaa SD kortille dataStringin
   dataFile = SD.open("test.txt", FILE_WRITE);  
   dataFile.println(dataString);
   dataFile.close();
   return(0);
}

