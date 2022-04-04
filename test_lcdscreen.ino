//www.diyusthad.com
#include <LiquidCrystal.h>

//const int rs = D9, en = D10, d4 = D3, d5 = D4, d6 = D5, d7 = D6;
LiquidCrystal lcd(D8, D7, D1, D2, D3, D4);

void setup() {
  lcd.begin(16, 2);
  lcd.print("New Temperature:");
  lcd.setCursor(0,1);
  lcd.print("25 Celsius");
}

void loop() {
}
