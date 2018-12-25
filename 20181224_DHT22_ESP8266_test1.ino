#include "SimpleDHT.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#define DHTPIN D7
#define DHT5V D8
#define DHTGRND D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(DHT5V, OUTPUT);
  digitalWrite(DHT5V, HIGH);
  pinMode(DHTGRND, OUTPUT);
  digitalWrite(DHTGRND, LOW);
  Serial.println("DHT22 test!");

  dht.begin();
}

void loop() {
  delay(6000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("NINCS JEL A SZENZORBÓL!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
}
