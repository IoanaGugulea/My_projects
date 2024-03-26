//biblioteca integrata pentru display digital
#include <TM1637Display.h>

//definesc pinii display-ului
#define CLK 9
#define DIO 10

//creez un obiect din clasa TM1637Display
//constructorul accepta cele doua intrari: CLK si DIO
TM1637Display display(CLK, DIO);

//valorile minime si maxime
int lowerThreshold = 320;
int upperThreshold = 393;

//declararea pinilor Arduino la care sunt conectati pinii senzorului
#define sensorPower 7
#define sensorPin A0

//variabila pentru stocarea nivelului curent al apei
int val = 0;

//declararea pinilor pentru fiecare led
int redLED = 2;
int blueLED = 3;
int greenLED = 4;

//pin pentru buzzer
#define buzzerPin 5


void setup() {
  //initializare comunicare serial pentru depanare
  Serial.begin(9600);

  //se configureaza pinul de putere pentru senzorul de apa
  pinMode(sensorPower, OUTPUT);
  digitalWrite(sensorPower, LOW);

  //configurare LED-uri la OUTPUT
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

   //inițializarea display-ului TM1637
  display.setBrightness(0x0a);  //puteți ajusta luminozitatea (0x00 până la 0x0f)

  //initial toate LED-urile sunt inchise, iar diplay-ul sters
  digitalWrite(redLED, LOW);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  display.clear();
}

void loop() {
  //citirea valorii de la senzorul de apa
  int level = readSensor();

  if (level == 0) {
    Serial.println("Water Level: Empty");
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    display.showNumberDec(0, false);
  } else if (level > 0 && level <= lowerThreshold) {
    Serial.println("Water Level: Low");
    digitalWrite(redLED, HIGH);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    display.showNumberDec(level, false);
  } else if (level > lowerThreshold && level <= upperThreshold) {
    Serial.println("Water Level: Medium");
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, HIGH);
    digitalWrite(greenLED, LOW);
    display.showNumberDec(level, false);
  } else if (level > upperThreshold) {
    Serial.println("Water Level: High");
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, HIGH);
    display.showNumberDec(level, false);
    //emite un sunet de avertizare folosind buzzer-ul pentru 1 secundă
    tone(buzzerPin, 1000, 1000);
  }
  delay(1000);
}

//functie pentru a citi valoarea senzorului de apa
int readSensor() {
  digitalWrite(sensorPower, HIGH);
  delay(10);
  val = analogRead(sensorPin);
  digitalWrite(sensorPower, LOW);
  return val;
}
