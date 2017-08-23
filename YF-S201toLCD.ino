/*
 Průtokoměr YF-S201 na LCD
 http://navody.arduino-shop.cz/navody-k-produktum/arduino-prutokomer-1-30-l-min.html

 YF-S201
 * pin D3

 LCD
 * SDA - pin D2
 * SCL - pin D1

 */

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define pinPrutokomer 2
#define pinPreruseni 0  // 0 = digitální pin 2 tj D3
// kalibrační proměnná, u senzoru YF-S201
// je to 4,5 pulzu za vteřinu pro 1 litr za minutu
const float kalibrFaktor = 4.5;

volatile byte pocetPulzu = 0;
float prutok = 0.0;
unsigned int prutokML = 0;
unsigned long soucetML = 0;
unsigned long staryCas = 0;

LiquidCrystal_PCF8574 lcd(0x3F);  // set the LCD address to 0x3F for a 16 chars and 2 line display

void setup() {
  // komunikace po sériové lince rychlostí 115200 baud
  Serial.begin(115200);
  // nastavení směru vstupního pinu
  pinMode(pinPrutokomer, INPUT);
  // nastavení vstupního pinu pro využití přerušení,
  // při detekci přerušení pomocí sestupné hrany (FALLING)
  // bude zavolán podprogram prictiPulz
  attachInterrupt(pinPreruseni, prictiPulz, FALLING);
  
  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Prut");
  lcd.setCursor(0, 1);
  lcd.print("Celk");
}
void loop() {
  // místo pro další příkazy
  lcd.setCursor(5, 0);
  lcd.print(prutokML,1);
  lcd.print("ml/s+      ");
  lcd.setCursor(5, 1);
  lcd.print(soucetML,1);
  lcd.print("ml      ");
  // pokud je rozdíl posledního uloženého času a aktuálního
  // 1 vteřina nebo více, provedeme měření
  if ((millis() - staryCas) > 1000) {
    // vypnutí detekce přerušení po dobu výpočtu a tisku výsledku
    detachInterrupt(pinPreruseni);
    // výpočet průtoku podle počtu pulzů za daný čas
    // se započtením kalibrační konstanty
    prutok = ((1000.0 / (millis() - staryCas)) * pocetPulzu) / kalibrFaktor;
    // výpočet průtoku kapaliny v mililitrech
    prutokML = (prutok / 60) * 1000;
    // přičtení objemu do celkového součtu
    soucetML += prutokML;
    // vytištění všech dostupných informací po sériové lince
    Serial.print("Prutok: ");
    Serial.print(prutok);
    Serial.print(" l/min");
    Serial.print("  Aktualni prutok: ");
    Serial.print(prutokML);
    Serial.print(" ml/sek");
    Serial.print("  Soucet prutoku: ");
    Serial.print(soucetML);
    Serial.println(" ml");
    // nulování počítadla pulzů
    pocetPulzu = 0;
    // uložení aktuálního času pro zahájení dalšího měření
    staryCas = millis();
    // povolení detekce přerušení pro nové měření
    attachInterrupt(pinPreruseni, prictiPulz, FALLING);
  }
}
// podprogram pro obsluhu přerušení
void prictiPulz() {
  // inkrementace čítače pulzů
  pocetPulzu++;
}
