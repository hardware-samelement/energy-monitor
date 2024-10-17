#include "WiFiType.h"
#include "WiFi.h"

#include "lcd_display.h"
#include <LiquidCrystal_I2C.h>

extern ReadData dataL1;
extern ReadData dataL2;
extern ReadData dataL3;
extern float kwhTotal;
extern String addr;

int page_before = 255;


LiquidCrystal_I2C lcd(0x27, 20, 4);

void lcd_init() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Energy Monitor");
  lcd.setCursor(2, 1);
  lcd.print("Starting....");
  // lcd.begin(16,2);
}

void lcd_show(int page) {
  String temp;

  switch (page) {
    case display_voltage:
      if (page_before != page) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CH1");
        lcd.setCursor(0, 1);
        lcd.print("CH2");
        lcd.setCursor(0, 2);
        lcd.print("CH3");

        lcd.setCursor(15, 0);
        lcd.print("V");
        lcd.setCursor(15, 1);
        lcd.print("V");
        lcd.setCursor(15, 2);
        lcd.print("V");

        lcd.setCursor(0, 3);
        lcd.print("kWh");
      }
      clearValue();

      //1st row
      temp = String(dataL1.Vrms);
      lcd.setCursor(14 - temp.length(), 0);
      lcd.print(temp);

      //2nd row
      temp = String(dataL2.Vrms);
      lcd.setCursor(14 - temp.length(), 1);
      lcd.print(temp);

      //3rd row
      temp = String(dataL3.Vrms);
      lcd.setCursor(14 - temp.length(), 2);
      lcd.print(temp);

      //4th row
      temp = String(kwhTotal);
      lcd.setCursor(16 - temp.length(), 3);
      lcd.print(temp);

      page_before = page;
      break;

    case display_current:

      if (page_before != page) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CH1");
        lcd.setCursor(0, 1);
        lcd.print("CH2");
        lcd.setCursor(0, 2);
        lcd.print("CH3");

        lcd.setCursor(15, 0);
        lcd.print("A");
        lcd.setCursor(15, 1);
        lcd.print("A");
        lcd.setCursor(15, 2);
        lcd.print("A");

        lcd.setCursor(0, 3);
        lcd.print("kWh");
      }

      clearValue();

      //1st row
      temp = String(dataL1.Irms);
      lcd.setCursor(14 - temp.length(), 0);
      lcd.print(temp);

      //2nd row
      lcd.setCursor(0, 1);
      lcd.print("CH2");
      temp = String(dataL2.Irms);
      lcd.setCursor(14 - temp.length(), 1);
      lcd.print(temp);

      //3rd row
      lcd.setCursor(0, 2);
      lcd.print("CH3");
      temp = String(dataL3.Irms);
      lcd.setCursor(14 - temp.length(), 2);
      lcd.print(temp);

      //4th row
      temp = String(kwhTotal);
      lcd.setCursor(16 - temp.length(), 3);
      lcd.print(temp);

      page_before = page;
      break;

    case display_real:
      if (page_before != page) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CH1");
        lcd.setCursor(0, 1);
        lcd.print("CH2");
        lcd.setCursor(0, 2);
        lcd.print("CH3");

        lcd.setCursor(15, 0);
        lcd.print("W");
        lcd.setCursor(15, 1);
        lcd.print("W");
        lcd.setCursor(15, 2);
        lcd.print("W");

        lcd.setCursor(0, 3);
        lcd.print("kWh");
      }

      clearValue();
      //1st row
      temp = String(dataL1.realPower);
      lcd.setCursor(14 - temp.length(), 0);
      lcd.print(temp);

      //2nd row
      temp = String(dataL2.realPower);
      lcd.setCursor(14 - temp.length(), 1);
      lcd.print(temp);

      //3rd row
      temp = String(dataL3.realPower);
      lcd.setCursor(14 - temp.length(), 2);
      lcd.print(temp);

      //4th row
      temp = String(kwhTotal);
      lcd.setCursor(16 - temp.length(), 3);
      lcd.print(temp);

      page_before = page;
      break;

    case display_apparent:
      if (page_before != page) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CH1");
        lcd.setCursor(0, 1);
        lcd.print("CH2");
        lcd.setCursor(0, 2);
        lcd.print("CH3");

        lcd.setCursor(14, 0);
        lcd.print("VA");
        lcd.setCursor(14, 1);
        lcd.print("VA");
        lcd.setCursor(14, 2);
        lcd.print("VA");

        lcd.setCursor(0, 3);
        lcd.print("kWh");
      }

      clearValue();

      //1st row
      temp = String(dataL1.apparentPower);
      lcd.setCursor(13 - temp.length(), 0);
      lcd.print(temp);

      //2nd row
      temp = String(dataL2.apparentPower);
      lcd.setCursor(13 - temp.length(), 1);
      lcd.print(temp);

      //3rd row
      temp = String(dataL3.apparentPower);
      lcd.setCursor(13 - temp.length(), 2);
      lcd.print(temp);

      //4th row
      temp = String(kwhTotal);
      lcd.setCursor(16 - temp.length(), 3);
      lcd.print(temp);

      page_before = page;
      break;

    case display_pf:
      if (page_before != page) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CH1");
        lcd.setCursor(0, 1);
        lcd.print("CH2");
        lcd.setCursor(0, 2);
        lcd.print("CH3");

        lcd.setCursor(14, 0);
        lcd.print("pf");
        lcd.setCursor(14, 1);
        lcd.print("pf");
        lcd.setCursor(14, 2);
        lcd.print("pf");

        lcd.setCursor(0, 3);
        lcd.print("kWh");
      }

      clearValue();

      //1st row
      temp = String(dataL1.powerFactor);
      lcd.setCursor(13 - temp.length(), 0);
      lcd.print(temp);

      //2nd row
      temp = String(dataL2.powerFactor);
      lcd.setCursor(13 - temp.length(), 1);
      lcd.print(temp);

      //3rd row
      temp = String(dataL3.powerFactor);
      lcd.setCursor(13 - temp.length(), 2);
      lcd.print(temp);

      //4th row
      temp = String(kwhTotal);
      lcd.setCursor(16 - temp.length(), 3);
      lcd.print(temp);

      page_before = page;
      break;

    case display_energy:
      if (page_before != page) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CH1");
        lcd.setCursor(0, 1);
        lcd.print("CH2");
        lcd.setCursor(0, 2);
        lcd.print("CH3");

        lcd.setCursor(13, 0);
        lcd.print("kwh");
        lcd.setCursor(13, 1);
        lcd.print("kwh");
        lcd.setCursor(13, 2);
        lcd.print("kwh");

        lcd.setCursor(0, 3);
        lcd.print("total");
      }

      clearValue();

      //1st row
      temp = String(dataL1.kwh);
      lcd.setCursor(12 - temp.length(), 0);
      lcd.print(temp);

      //2nd row
      temp = String(dataL2.kwh);
      lcd.setCursor(12 - temp.length(), 1);
      lcd.print(temp);

      //3rd row
      temp = String(dataL3.kwh);
      lcd.setCursor(12 - temp.length(), 2);
      lcd.print(temp);

      //4th row
      temp = String(kwhTotal);
      lcd.setCursor(16 - temp.length(), 3);
      lcd.print(temp);

      page_before = page;
      break;

    case display_info:
      if (WiFi.status() == WL_CONNECTED) {
        String ssid = WiFi.SSID();
        String ip = WiFi.localIP().toString();

        lcd.clear();
        lcd.setCursor((16 - ssid.length()) / 2, 0);
        lcd.print(ssid);
        lcd.setCursor((16 - ip.length()) / 2, 1);
        lcd.print(ip);
      } else {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("not connected");
      }

      page_before = page;
      break;

    case display_info2:
      if (WiFi.status() == WL_CONNECTED) {

        lcd.clear();
        lcd.setCursor((16 - addr.length()) / 2, 0);
        lcd.print(addr);
      } else {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("not connected");
      }

      page_before = page;
      break;

    case display_apmode:
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("AP Mode");
      break;

    case display_1ch:
      lcd.clear();
      break;
  }
}

void clearValue() {
  lcd.setCursor(4, 0);
  lcd.print("         ");
  lcd.setCursor(4, 1);
  lcd.print("         ");
  lcd.setCursor(4, 2);
  lcd.print("         ");
  lcd.setCursor(4, 3);
  lcd.print("         ");
}
