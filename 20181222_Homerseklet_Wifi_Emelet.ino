#include <ConnectThings_ESP8266.h>

//Hőmérő cucc
#include "SimpleDHT.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT22

//WiFi cuccok
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>

DHT dht(DHTPIN, DHTTYPE);

int pir_count = 0;
const char* ssid = "KHG-Wifi";           // WiFi ssd
const char* password = "Danci20151219";  // WiFi password
String GAS_ID = "AKfycbx1kNNgwdB6kugqoyAI_8dAsjV1aqwklpY29l3JK1GG2SGWw84";      // google script azonosito
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;

void setup()
{
      Serial.begin(9600);
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
      delay(10000);
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();
      Serial.println("\t");
      Serial.print(humidity, 1);
      Serial.print("\t\t");
      Serial.print(temperature, 1);
      sendData("emelet",temperature, humidity, pir_count);
      delay(1800000);  //15 perc várakozás 900000
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
      while (client.connected()) 
          {
            String line = client.readStringUntil('\n');
            if (line == "\r") 
                {
                    Serial.println("headers received");
                    break;
                }
          }
    String line = client.readStringUntil('\n');
  
    if (line.startsWith("{\"state\":\"success\"")) 
      {
          Serial.println("esp8266/Arduino CI successfull!");
      } else 
      {
          Serial.println("esp8266/Arduino CI has failed");
      }
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    Serial.println("closing connection");
} 
