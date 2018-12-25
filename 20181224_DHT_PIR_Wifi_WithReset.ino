#include <ConnectThings_ESP8266.h>

//Hőmérő cucc
#include "SimpleDHT.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#define DHTGRND D6
#define DHTPIN D5
#define DHT5V D8
#define PIRPIN D2
#define DHTTYPE DHT22

//WiFi cuccok
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
//#include <WiFiManager.h>

DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temperature;
int pir_count = 0;                       // mozgások számlálója
int counter = 0;                         // ciklusok számlálója
int frissites = 900000;                  // feltöltés gyakorisága
int pir_lepes = 500;                    // mozgásérzékelés gyakorisága
int reset_lepes = 1;                     // újraindítás után
String hely = "nappali";                  // érzékelő helye
byte state = 0;
const char* ssid = "KHG-Wifi";           // WiFi ssid
const char* password = "Danci20151219";  // WiFi password
String GAS_ID = "AKfycbx1kNNgwdB6kugqoyAI_8dAsjV1aqwklpY29l3JK1GG2SGWw84";      // google script azonosito
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;

void setup()
{
      Serial.begin(9600);
      pinMode(DHT5V, OUTPUT);
      digitalWrite(DHT5V, HIGH);
      pinMode(DHTGRND, OUTPUT);
      digitalWrite(DHTGRND, LOW);
      pinMode(PIRPIN,INPUT);
      //WiFiManager wifiManager;
      //wifiManager.autoConnect("AutoConnectAP");
      WiFi.begin(ssid, password);
      Serial.println("");
      while (WiFi.status() != WL_CONNECTED) 
            {
              delay(1000);
              Serial.print(".");
            }
      Serial.println("");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
}

void loop()
{
      Serial.print ("Feltoltesig hátravan: ");
      Serial.print ((frissites/pir_lepes)-(counter/pir_lepes));
      Serial.print (" lepes. Mozgas: ");
      Serial.print (pir_count);
      Serial.println (state);
      // Újraindítás után jegyezzük fel az IP címet
      if (reset_lepes == 1)
          { 
            digitalWrite(DHT5V, HIGH);
            delay (2000);
            humidity = dht.readHumidity();
            temperature = dht.readTemperature();
            Serial.print("\t");
            Serial.print(humidity);
            Serial.print("\t\t");
            Serial.print(temperature);
            Serial.print("\t\t");
            sendData(hely,temperature, humidity, 2); // google spreadsheet script hívása az adatok feltöltéséhez
            reset_lepes = 0;
            Serial.print ("Elso uzenet elkuldve! Reset_lepes erteke: ");
            Serial.println (reset_lepes);
          }
      if (counter > frissites)
        { // Ha elértük a frissítési időt, megmérjük  a hőmérsékletet és feltöltjük az adatokat
              // Hőmérséklet és páratartalom adatok olvasása
              Serial.println ("Homerseklet meres: ");
              humidity = dht.readHumidity();
              temperature = dht.readTemperature();
              counter = 0;
              if (isnan(humidity) || isnan(temperature)) //Ha nem aktív a DHT szenzor állítsuk az értékeket átlagosra
                {
                    humidity = 60;
                    temperature = 18;
                    //return;
                }
              Serial.print("\t");
              Serial.print(humidity);
              Serial.print("\t\t");
              Serial.print(temperature);
              Serial.print("\t\t");
              Serial.println(WiFi.localIP());
              sendData(hely,temperature, humidity, pir_count); // google spreadsheet script hívása az adatok feltöltéséhez
              delay (500);
              pir_count = 0;
              Serial.println ("Mozgások száma reset után: ");
              Serial.print (pir_count); 
        }
      else
        { // Egyébként folyamatosan figyeljük a mozgásokat
              state = digitalRead(PIRPIN);
              if(state == 1) pir_count=1;
              counter = counter + pir_lepes;
              delay(pir_lepes);
        }
}


void sendData(String hol, int mennyi, int mennyi2, int mozog) // Function for Send data into Google Spreadsheet
{
      Serial.print("connecting to ");
      Serial.println(host);
      if (!client.connect(host, httpsPort)) 
          {
            Serial.println("connection failed");
            return;
          }
      String string_par1 =  String(mennyi, DEC);
      String string_par2 =  String(mennyi2, DEC); 
      String string_par3 = String(mozog, DEC);
      String url = "/macros/s/" + GAS_ID + "/exec?hol=" + hol + "&mennyi=" + string_par1 + "&mennyi2=" + string_par2 + "&mozog=" + string_par3;
      Serial.print("requesting URL: ");
      Serial.println(url);

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "User-Agent: BuildFailureDetectorESP8266\r\n" +
      "Connection: close\r\n\r\n");
      Serial.println("request sent");
} 
