#include <MD_DS3231.h>    //RTC lib
#include <SD.h>           //SD kortin lib
#include <SPI.h>
#include <SFE_BMP180.h>   //paine ja lämpötila anturin lib
#include <Wire.h>         //sama

#include <DHT.h>          //kosteusanturi
#define DHTPIN 2          //missä pinnissä anturi on
#define DHTTYPE DHT22     //definataan mitä piiriä käytetään
DHT dht(DHTPIN, DHTTYPE); //laitetaan toimimaan

SFE_BMP180 pressure;      //

#define ALTITUDE 37     //laita tähän nykyinen korkeus metreissä

String dataString;       //luodaan stringi datalle

File dataFile;

uint32_t location = 0;   //int datan paikalle, mitä ei olla lähetetty

void setup() {

Serial.begin(9600);    //baudrate

}


void loop() {
 
 
 String dataString = "";  //tyhjennetään stringi

 Aika();
 dataString +=";";
 TempPres();
 dataString += ";";
 Humidity();
 dataPrint();

 //Datanlähetys:
 Handshake();
 if (Handshake() == 1){
  SendLoop();
 }

 return;

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

  return;
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

  return;
  
  }

void Humidity(){
  float h = dht.readHumidity();   //lukee kosteuden
  dataString += h;     //tarvitseeko kokeilla uudestaan, jos tulee nan?
  return;
    
}


void dataPrint(){ //printtaa SD kortille dataStringin
   dataFile = SD.open("test.txt", FILE_WRITE);  
   dataFile.println(dataString);
   dataFile.close();
   return;
}

int Handshake(){   //tarkistaa, että BT yhteys on ok ja palauttaa 1, jos on
  char rec = 0;
  char ykok = "ykok";
  for(int k = 0; k <= 2; k++){    //vertaa vastausta kolmesti
    for(int i = 0; i <= 2; i++){    //huutaa kolmesti
     Serial.print("yk");
     for(int j = 0; j <= 4; j++){   //kuuntelee viidesti
        if(Serial.available() == 0){
         delay(20);
       }
        else{
         i = 3;
         j = 5;
       }
     }
   }
   if(Serial.available() == 0){
     return(0);
   }
   rec = Serial.read();
   if(rec == ykok){
     k = 3; 
   }
  }
  Serial.print("ykok");    //jos pääsee tähän, yhteys on kuosissa
  return(1);
  
}

int SendLoop(){
  dataFile = SD.open("test.txt");
  dataFile.seek(location);
  while (dataFile.peek() != -1){
    SendData();
    if (SendData() == 0){
      dataFile.close();
      return(0);
    }
  }
  dataFile.close();
  return(1);
}
  

int SendData(){    //lähettää seuraavan rivin tallennettua ei-lähetettyä dataa
  char rec = 0;    //jos ok, niin palauttaa 1 ja siirtää lukukohdan seuraavalle riville
  char ok = "ok";
  //dataFile = SD.open("test.txt");   avattu jo SendLoopissa
  dataFile.seek(location);
  while (dataFile.peek() != "/n"){
    Serial.write(dataFile.read());
  }
  Serial.write("/n");
  while (Serial.available() == 0){
  }
  delay(20);
  rec = Serial.read();
  if (rec == ok){
  location = (dataFile.position() + 1); //+1, koska ei missään vaiheessa lukenut rivinvaihtoa
  //dataFile.close();
  return(1);
  }
  //dataFile.close(); sulkeutuu SendLoopissa
  return(0);
}
