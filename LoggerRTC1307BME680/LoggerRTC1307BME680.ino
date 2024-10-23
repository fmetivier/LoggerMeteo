/*

Nano Logger + BME 680
Measurements : temperature, humidity, pressure, gaz content
Time step: DT
Storage: SD card
Clock : DS1307

last tested:
Author: F. Métivier

CC-By SA 4.0 
*/

#include "LowPower.h"

#include "Wire.h"
#include <RTClib.h> 
#define ADRESSE_I2C_RTC 0x68 // Adresse I2C de l'horloge RTC
RTC_DS1307 rtc;

const int pullupPin = 2; // reçoit le signal 1Hz du DS1307 
const int DT=10; // interval de emsures (s)
volatile int jedors = 0; // compteur de réception du signal 1Hz

/*
 * Carte SD
 */
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10; // CS pin de la carte SD sur Nano
const int SDOFFPIN = 3; // éteindre le lecteur

Sd2Card card;
SdVolume volume;
SdFile root;

//BME680 lib
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#define SEALEVELPRESSURE_HPA (1013.25)
#define ADRESSE_I2C_BME 0x77 // Adresse BME

Adafruit_BME680 bme;
const int BMEPIN = 4;

void setup()
{
  Serial.begin(57600); // pour affichage dans le moniteur série
  delay(10);
  
  Wire.begin();

  // RTC
  Wire.beginTransmission(ADRESSE_I2C_RTC);
  rtc.begin();
  rtc.writeSqwPinMode(DS1307_SquareWave1HZ);
  DateTime now = rtc.now();
  Wire.endTransmission();

  pinMode(pullupPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pullupPin),jeCompte,FALLING);


  // BME
  pinMode(BMEPIN,OUTPUT);
  digitalWrite(BMEPIN,LOW);

  // SD
  pinMode(SDOFFPIN,OUTPUT);
  digitalWrite(SDOFFPIN,HIGH);
  delay(1000);

  if (!SD.begin(chipSelect)){
    Serial.println("problème avec le lecteur SD");
  }
  char buff[] = "démarrage hh:mm:ss DDD, DD MM YYYY";
  Serial.println(now.toString(buff));
  delay(100);

  digitalWrite(SDOFFPIN,LOW);

}

void loop()
{


      LowPower.powerDown(SLEEP_FOREVER,ADC_OFF,BOD_OFF);
      if (jedors % DT == 0) {
        // arrêt des interruption
        detachInterrupt(digitalPinToInterrupt(pullupPin));
        // démarrage série
        Serial.begin(57600);
        delay(100);
        // récupération de la date
        Wire.begin();
        Wire.beginTransmission(ADRESSE_I2C_RTC);
        DateTime now = rtc.now();
        Wire.endTransmission();
        delay(100);

        // récupération des mesures
        digitalWrite(BMEPIN,HIGH);
        delay(10);
        Wire.beginTransmission(ADRESSE_I2C_BME);
        bme.begin();
        bme.performReading();
        Wire.endTransmission();
        digitalWrite(BMEPIN,LOW);

        // récupération voltage des piles    
        float Voltage = analogRead(A0)*5.0/1023.0;
                  
        Serial.print(now.year());
        Serial.print(F(","));
        Serial.print(now.month());
        Serial.print(F(","));
        Serial.print(now.day());
        Serial.print(F(","));
        Serial.print(now.hour());
        Serial.print(F(","));
        Serial.print(now.minute());
        Serial.print(F(","));
        Serial.print(now.second());
        Serial.print(F(","));
        Serial.print(bme.temperature);
        Serial.print(F(","));
        Serial.print(bme.pressure);
        Serial.print(F(","));
        Serial.print(bme.humidity);
        Serial.print(F(","));
        Serial.print(bme.gas_resistance);
        Serial.print(F(","));
        Serial.print(Voltage);
        Serial.print("\n");

        // réveil lecteur SD
        digitalWrite(SDOFFPIN,HIGH);
        delay(10);
        // écriture carte
        if (SD.begin(chipSelect)){
          File dataFile = SD.open("DATALOG.TXT", FILE_WRITE);
        
          if (dataFile) {
          // if the file is available, write to it:
            dataFile.print(now.year());
            dataFile.print(F(","));
            dataFile.print(now.month());
            dataFile.print(F(","));
            dataFile.print(now.day());
            dataFile.print(F(","));
            dataFile.print(now.hour());
            dataFile.print(F(","));
            dataFile.print(now.minute());
            dataFile.print(F(","));
            dataFile.print(now.second());
            dataFile.print(F(","));
            dataFile.print(bme.temperature);
            dataFile.print(F(","));
            dataFile.print(bme.pressure);
            dataFile.print(F(","));
            dataFile.print(bme.humidity);
            dataFile.print(F(","));
            dataFile.print(bme.gas_resistance);
            dataFile.print(F(","));
            dataFile.print(Voltage);
            dataFile.print("\n");   
            dataFile.close();  
        } else {
          Serial.println("Pas de fichier ?");  
        }
        } else {
          Serial.println("Pas de carte ?");
        }
        
        delay(10);
        //mise en sommeil lecteur SD
        digitalWrite(SDOFFPIN,LOW);

        // arrêt Série
        Serial.end();
        // redémarrage interruptions
        attachInterrupt(digitalPinToInterrupt(pullupPin),jeCompte,FALLING);  
      }

} //end void

void jeCompte(){
  jedors++; 
}
