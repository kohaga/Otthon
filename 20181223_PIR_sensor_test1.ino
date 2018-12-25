#define sensorPin D8
void setup()
{
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

void loop()
{
  byte state = digitalRead(sensorPin);
  if (state == 1)Serial.println("ITT VAN VALAKI");
  else Serial.println("SENKI");
  delay(500);
}
