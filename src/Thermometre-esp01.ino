/* Zone commune pour tous les objets, ne rien effacer mais adapter */

#include <string>

#define DEBUG   //If you comment this line, the serial lines are defined as blank.

const char*       PROTOVOX_HARDWARE_NAME =      "ESP01S-TEMP-01";      //identification du hardware sur le réseau
const char*       PROTOVOX_TOPIC_PATH =         "home/living";       // tous les topics de ce capteur sont réunis dans la même hierarchie : State , lastrun, update, etc.
const char*       PROTOVOX_HUMAN_NAME =         "Température salon";
const char*       PROTOVOX_VERSION =            "0.1.0";



int               CONNECT_PIN = 0; // ESP01 : pas assez de pin pour blinking pendant la connection



const char*       TOPIC_TEMP =           "/temp";            // le topic principal celui qui dit si la chaudière chauffe


// LA condition ci dessous ne fonctionne pas faire à la main

//#if defined(ESP8266) || defined(ESP32)
//int               veille =                       analogRead(D0);      // attention valable uniquement sur WEMOS D1 MINI
//#else
int               veille =                       1;                   // veille non autorisée
//#endif

// la librairie UP-RISE
#include <protovox.h>
Protovox  myProtovox;



// Temps de veille profonde (deep sleep) entre 2 mesures.
// La veille profonde est indispensable et nécessite de relier physiquement les broches RST et DO
#define durationSleep 1800 // secondes

/* FIN de zone commune*/
/* Zone spécifique aux capteurs */

// librairies pour la gestion du capteur de Température pour exemple d'application
#include <OneWire.h>
#include <DallasTemperature.h>
// Sensor input pin
#define DATA_PIN 2
// How many bits to use for temperature values: 9, 10, 11 or 12
//9 bit	0.5 degrees time 	93.75 mSec
//10 bit	0.25 degrees time	187.5 mSec
//11 bit	0.125 degrees time	375 mSec
//12 bit	0.0625 degrees time	750 mSec
#define SENSOR_RESOLUTION 11
// Index of sensors connected to data pin, default: 0
#define SENSOR_INDEX 0

OneWire oneWire(DATA_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorDeviceAddress;

/* FIN de zone spécifique */



void setup() {




    DBEGIN(115200);
    DPRINTLN();
    DPRINTLN();
    DPRINTLN("------  |||   START   |||   ------");
    DPRINTLN();
    DPRINTLN("VERSION SENSOR :"); DPRINTLN(PROTOVOX_VERSION);



  // gestion de la connection au wifi et MQTT
  myProtovox.connect();
  // pas de lecture Batterie sur ESP01S
  //int bat = myProtovox.getBatteryCapacity();

  /*
  (bat);
  DPRINTLN("% de batterie");
  */
  sensors.begin();
  sensors.getAddress(sensorDeviceAddress, 0);
  sensors.setResolution(sensorDeviceAddress, SENSOR_RESOLUTION);
  sensors.requestTemperatures();
  // Measurement may take up to 750ms

  float temperatureInCelsius = sensors.getTempCByIndex(SENSOR_INDEX);

  if(mqtt.publish(myProtovox.concatenate(PROTOVOX_TOPIC_PATH,TOPIC_TEMP,""), String(temperatureInCelsius).c_str(), true)){   // && mqtt.publish("/maison/temperature/bat", String(bat).c_str())){

    DPRINTLN("Message sent :");

    DPRINT("Temperature: ");
    DPRINT(temperatureInCelsius, 2);
    DPRINT("°C");
    /*DPRINT("(Batterie restante : ");
    DPRINT(bat);
    DPRINTLN("%)");*/

    myProtovox.sleep(durationSleep);
  }
}

void loop() {
  //DPRINTLN("en attente de mise à jour ...");
  mqtt.loop();
}
