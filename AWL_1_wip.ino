#include <MD_DS3231.h>    //RTC lib
#include <SD.h>           //SD kortin lib
#include <SPI.h>
#include <SFE_BMP180.h>   //paine ja lämpötila anturin lib
#include <Wire.h>         //sama

SFE_BMP180 pressure;      //

#define ALTITUDE 37     //laita tähän nykyinen korkeus metreissä

String dataString;       //luodaan stringi datalle

File dataFile;



void setup() {


}


void loop() {
 
 
 String dataString = "";  //tyhjennetään stringi

 Aika();
 dataString +=";";
 TempPres();
 dataString += ";";
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
  char status;
  double T,P,p0,a;
  pressure.begin(); //käynnistää moduulin
  
   for(int i = 0; i <= 2; i++){ //jos ei toimi, kokeilee uudestaan max 3 kertaa
    status = pressure.startTemperature();
  /*aloittaa mittauksen ja antaa arvon, minkä mittaus 
   * kestää ms, tai 0, jos ei toimi
   */ 
    if (status == 0){                   
      if(i == 2){
        dataString +="TEMPERROR;";    //printataan faili
        return;
      }
      else {
      delay(5);
      }
    }
    else {
      break;  
    }
    
  }


  delay(status);    //odottaa mittauksen keston ajan

  status = pressure.getTemperature(T);
  dataString += T;
  dataString +=";";

  //Sitten alkaa paineen mittaus (pitää olla tempin jälkeen)

  for(int i = 0; i <= 2; i++){ //jos ei toimi, kokeilee uudestaan max 3 kertaa
    status = pressure.startPressure(3); 
    if (status == 0){                   
      if(i == 2){
        dataString +="PRESERROR;";    //printataan faili
        return;
      }
      else {
      }
    }
    else {
      break;  
    }
    }

  delay(status);

  status = pressure.getPressure(P,T);
  p0 = pressure.sealevel(P, ALTITUDE);
  dataString += p0;

  return(0);
  
  }


void dataPrint(){ //printtaa SD kortille dataStringin
   dataFile = SD.open("test.txt", FILE_WRITE);  
   dataFile.println(dataString);
   dataFile.close();
   return(0);
}

