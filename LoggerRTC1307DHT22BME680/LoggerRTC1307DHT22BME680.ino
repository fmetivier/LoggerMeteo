/*
 * Nano Logger
 * 
 * RTC 1307
 * SD uPesy
 * DHT 11
 * 
 * Testé le 05/07
 * Pour gagner un peu de temps et d'énergie desactiver les sorties série avant utilisation en autonomie
 * 
 * F. Métivier
 * CC BY-SA 4.0
 */
#include "LowPower.h"

#include "Wire.h"
#include <RTClib.h> 
#define ADRESSE_I2C_RTC 0x68 // Adresse I2C de l'horloge RTC
RTC_DS1307 rtc;

const int pullupPin = 2;
const int DT=5;
volatile int jedors = 0;

/*
 * Carte SD
 */
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10; // CS pin de la carte SD sur Nano
const int SDOFFPIN = 3;

Sd2Card card;
SdVolume volume;
SdFile root;

#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN,DHTTYPE);

#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#define SEALEVELPRESSURE_HPA (1013.25)
#define ADRESSE_I2C_BME 0x76 // BME card addres

Adafruit_BME680 bme;


void setup()
{

  
  Serial.begin(57600); // pour affichage dans le moniteur série
  delay(10);
  
  Wire.begin();


  Wire.beginTransmission(ADRESSE_I2C_RTC);
  rtc.begin();
  rtc.writeSqwPinMode(DS1307_SquareWave1HZ);
  DateTime now = rtc.now();
  Wire.endTransmission();
  delay(100);


  Wire.beginTransmission(ADRESSE_I2C_BME);
    bme.begin();
    Serial.println("BME ok");
  Wire.endTransmission();
  delay(100);

  pinMode(pullupPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pullupPin),jeCompte,FALLING);

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
        Serial.println("Réveil !");
        
        // récupération de la date
        Wire.begin();
        
        Wire.beginTransmission(ADRESSE_I2C_RTC);
        DateTime now = rtc.now();
        String sentence = String(now.year())+";"+String(now.month())+";"+String(now.day())+";"+String(now.hour()) + ";" + String(now.minute())+ ";" + String(now.second()) + ";";
        Wire.endTransmission();
        delay(100);

        dht.begin();
        delay(100);
        sentence += String(dht.readTemperature()) + ";" + String(dht.readHumidity()) + ";";

        Wire.beginTransmission(ADRESSE_I2C_BME);
        bme.begin();
        delay(100);
        bme.performReading();
        sentence += String(bme.temperature) + ";" + String(bme.pressure) + ";" + String(bme.humidity) + ";" + String(bme.gas_resistance);
        Wire.endTransmission();
        
        Serial.println(sentence);

        // réveil lecteur SD
        digitalWrite(SDOFFPIN,HIGH);
        delay(100);
        // écriture carte
        if (SD.begin(chipSelect)){
          File dataFile = SD.open("DATALOG.TXT", FILE_WRITE);
        
          if (dataFile) {
          // if the file is available, write to it:
          dataFile.print(sentence+"\n");
          dataFile.close();
        } else {
          Serial.println("Pb de fichier ?");  
        }
        } else {
          Serial.println("Pb de lecteur ?");
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
