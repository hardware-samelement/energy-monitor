#include "EnergyMonitor_global.h"
#include "WiFi.h"
#include "WiFiType.h"

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
  lcd.setCursor(4, 0);
  lcd.print("SAM ELEMENT");
  lcd.setCursor(1, 1);
  lcd.print("Energy Monitoring");
  lcd.setCursor(4, 3);
  lcd.print("Starting.");
  vTaskDelay(1000);
  lcd.print(".");
  vTaskDelay(1000);
  lcd.print(".");
  vTaskDelay(1000);
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

      lcd.setCursor(19, 0);
      lcd.print("V");
      lcd.setCursor(19, 1);
      lcd.print("V");
      lcd.setCursor(19, 2);
      lcd.print("V");

      lcd.setCursor(0, 3);
      lcd.print("kWh");
    }
    clearValue();

    // 1st row
    temp = String(dataL1.Vrms);
    lcd.setCursor(18 - temp.length(), 0);
    lcd.print(temp);

    // 2nd row
    temp = String(dataL2.Vrms);
    lcd.setCursor(18 - temp.length(), 1);
    lcd.print(temp);

    // 3rd row
    temp = String(dataL3.Vrms);
    lcd.setCursor(18 - temp.length(), 2);
    lcd.print(temp);

    // 4th row
    temp = String(kwhTotal);
    lcd.setCursor(20 - temp.length(), 3);
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

      lcd.setCursor(19, 0);
      lcd.print("A");
      lcd.setCursor(19, 1);
      lcd.print("A");
      lcd.setCursor(19, 2);
      lcd.print("A");

      lcd.setCursor(0, 3);
      lcd.print("kWh");
    }

    clearValue();

    // 1st row
    temp = String(dataL1.Irms);
    lcd.setCursor(18 - temp.length(), 0);
    lcd.print(temp);

    // 2nd row
    lcd.setCursor(0, 1);
    lcd.print("CH2");
    temp = String(dataL2.Irms);
    lcd.setCursor(18 - temp.length(), 1);
    lcd.print(temp);

    // 3rd row
    lcd.setCursor(0, 2);
    lcd.print("CH3");
    temp = String(dataL3.Irms);
    lcd.setCursor(18 - temp.length(), 2);
    lcd.print(temp);

    // 4th row
    temp = String(kwhTotal);
    lcd.setCursor(20 - temp.length(), 3);
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

      lcd.setCursor(19, 0);
      lcd.print("W");
      lcd.setCursor(19, 1);
      lcd.print("W");
      lcd.setCursor(19, 2);
      lcd.print("W");

      lcd.setCursor(0, 3);
      lcd.print("kWh");
    }

    clearValue();
    // 1st row
    temp = String(dataL1.realPower);
    lcd.setCursor(18 - temp.length(), 0);
    lcd.print(temp);

    // 2nd row
    temp = String(dataL2.realPower);
    lcd.setCursor(18 - temp.length(), 1);
    lcd.print(temp);

    // 3rd row
    temp = String(dataL3.realPower);
    lcd.setCursor(18 - temp.length(), 2);
    lcd.print(temp);

    // 4th row
    temp = String(kwhTotal);
    lcd.setCursor(20 - temp.length(), 3);
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

      lcd.setCursor(18, 0);
      lcd.print("VA");
      lcd.setCursor(18, 1);
      lcd.print("VA");
      lcd.setCursor(18, 2);
      lcd.print("VA");

      lcd.setCursor(0, 3);
      lcd.print("kWh");
    }

    clearValue();

    // 1st row
    temp = String(dataL1.apparentPower);
    lcd.setCursor(17 - temp.length(), 0);
    lcd.print(temp);

    // 2nd row
    temp = String(dataL2.apparentPower);
    lcd.setCursor(17 - temp.length(), 1);
    lcd.print(temp);

    // 3rd row
    temp = String(dataL3.apparentPower);
    lcd.setCursor(17 - temp.length(), 2);
    lcd.print(temp);

    // 4th row
    temp = String(kwhTotal);
    lcd.setCursor(20 - temp.length(), 3);
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

      lcd.setCursor(18, 0);
      lcd.print("pf");
      lcd.setCursor(18, 1);
      lcd.print("pf");
      lcd.setCursor(18, 2);
      lcd.print("pf");

      lcd.setCursor(0, 3);
      lcd.print("kWh");
    }

    clearValue();

    // 1st row
    temp = String(dataL1.powerFactor);
    lcd.setCursor(17 - temp.length(), 0);
    lcd.print(temp);

    // 2nd row
    temp = String(dataL2.powerFactor);
    lcd.setCursor(17 - temp.length(), 1);
    lcd.print(temp);

    // 3rd row
    temp = String(dataL3.powerFactor);
    lcd.setCursor(17 - temp.length(), 2);
    lcd.print(temp);

    // 4th row
    temp = String(kwhTotal);
    lcd.setCursor(20 - temp.length(), 3);
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

      lcd.setCursor(17, 0);
      lcd.print("kwh");
      lcd.setCursor(17, 1);
      lcd.print("kwh");
      lcd.setCursor(17, 2);
      lcd.print("kwh");

      lcd.setCursor(0, 3);
      lcd.print("total");
    }

    clearValue();

    // 1st row
    temp = String(dataL1.kwh);
    lcd.setCursor(16 - temp.length(), 0);
    lcd.print(temp);

    // 2nd row
    temp = String(dataL2.kwh);
    lcd.setCursor(16 - temp.length(), 1);
    lcd.print(temp);

    // 3rd row
    temp = String(dataL3.kwh);
    lcd.setCursor(16 - temp.length(), 2);
    lcd.print(temp);

    // 4th row
    temp = String(kwhTotal);
    lcd.setCursor(20 - temp.length(), 3);
    lcd.print(temp);

    page_before = page;
    break;

  case display_info:
    if (WiFi.status() == WL_CONNECTED) {
      String ssid = WiFi.SSID();
      String ip = WiFi.localIP().toString();

      lcd.clear();
      lcd.setCursor((20 - ssid.length()) / 2, 0);
      lcd.print(ssid);
      lcd.setCursor((20 - ip.length()) / 2, 1);
      lcd.print(ip);
      lcd.setCursor((20 - addr.length()) / 2, 3);
      lcd.print(addr);
      if (iot_isConnected()) {
        temp = "IoT Connected";
      } else
        temp = "IoT not Connected";
      lcd.setCursor((20 - temp.length()) / 2, 2);
      lcd.print(temp);
    } else {
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("WiFi not Connected");
    }

    page_before = page;
    break;

    page_before = page;
    break;

  case display_apmode:
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("- WiFi Config -");
    lcd.setCursor(0, 1);
    lcd.print("SSID: EnergyMonitor");
    lcd.setCursor(0, 2);
    lcd.print("PASS: abcd1234");
    lcd.setCursor(0, 3);
    lcd.print("Open -> 192.168.1.1");
    break;

  case display_ch1:
    if (page_before != page) {
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("- CH 1 -");
      lcd.setCursor(0, 1);
      lcd.println("V:");
      lcd.setCursor(0, 2);
      lcd.print("A:");
      lcd.setCursor(0, 3);
      lcd.print("W:");
      lcd.setCursor(10, 1);
      lcd.print("VA:");
      lcd.setCursor(10, 2);
      lcd.print("PF:");
      lcd.setCursor(10, 3);
      lcd.print("KWH:");
    }

    clearValueCh();

    // vrms
    temp = String(dataL1.Vrms);
    lcd.setCursor(9 - temp.length(), 1);
    lcd.print(temp);

    // irms
    temp = String(dataL1.Irms);
    lcd.setCursor(9 - temp.length(), 2);
    lcd.print(temp);

    // W
    temp = String(dataL1.realPower);
    lcd.setCursor(9 - temp.length(), 3);
    lcd.print(temp);

    // VA
    temp = String(dataL1.apparentPower);
    lcd.setCursor(20 - temp.length(), 1);
    lcd.print(temp);

    // pf
    temp = String(dataL1.powerFactor);
    lcd.setCursor(20 - temp.length(), 2);
    lcd.print(temp);

    // kwh
    temp = String(dataL1.kwh);
    lcd.setCursor(20 - temp.length(), 3);
    lcd.print(temp);

    break;

  case display_ch2:
    if (page_before != page) {
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("- CH 2 -");
      lcd.setCursor(0, 1);
      lcd.println("V:");
      lcd.setCursor(0, 2);
      lcd.print("A:");
      lcd.setCursor(0, 3);
      lcd.print("W:");
      lcd.setCursor(10, 1);
      lcd.print("VA:");
      lcd.setCursor(10, 2);
      lcd.print("PF:");
      lcd.setCursor(10, 3);
      lcd.print("KWH:");
    }

    clearValueCh();

    // vrms
    temp = String(dataL2.Vrms);
    lcd.setCursor(9 - temp.length(), 1);
    lcd.print(temp);

    // irms
    temp = String(dataL2.Irms);
    lcd.setCursor(9 - temp.length(), 2);
    lcd.print(temp);

    // W
    temp = String(dataL2.realPower);
    lcd.setCursor(9 - temp.length(), 3);
    lcd.print(temp);

    // VA
    temp = String(dataL2.apparentPower);
    lcd.setCursor(20 - temp.length(), 1);
    lcd.print(temp);

    // pf
    temp = String(dataL2.powerFactor);
    lcd.setCursor(20 - temp.length(), 2);
    lcd.print(temp);

    // kwh
    temp = String(dataL2.kwh);
    lcd.setCursor(20 - temp.length(), 3);
    lcd.print(temp);

    break;

  case display_ch3:
    if (page_before != page) {
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("- CH 3 -");
      lcd.setCursor(0, 1);
      lcd.println("V:");
      lcd.setCursor(0, 2);
      lcd.print("A:");
      lcd.setCursor(0, 3);
      lcd.print("W:");
      lcd.setCursor(10, 1);
      lcd.print("VA:");
      lcd.setCursor(10, 2);
      lcd.print("PF:");
      lcd.setCursor(10, 3);
      lcd.print("KWH:");
    }

    clearValueCh();

    // vrms
    temp = String(dataL3.Vrms);
    lcd.setCursor(9 - temp.length(), 1);
    lcd.print(temp);

    // irms
    temp = String(dataL3.Irms);
    lcd.setCursor(9 - temp.length(), 2);
    lcd.print(temp);

    // W
    temp = String(dataL3.realPower);
    lcd.setCursor(9 - temp.length(), 3);
    lcd.print(temp);

    // VA
    temp = String(dataL3.apparentPower);
    lcd.setCursor(20 - temp.length(), 1);
    lcd.print(temp);

    // pf
    temp = String(dataL3.powerFactor);
    lcd.setCursor(20 - temp.length(), 2);
    lcd.print(temp);

    // kwh
    temp = String(dataL3.kwh);
    lcd.setCursor(20 - temp.length(), 3);
    lcd.print(temp);

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
  lcd.setCursor(5, 3);
  lcd.print("        ");
}

void clearValueCh() {
  lcd.setCursor(2, 1);
  lcd.print("        ");
  lcd.setCursor(2, 2);
  lcd.print("        ");
  lcd.setCursor(2, 3);
  lcd.print("        ");
  lcd.setCursor(13, 1);
  lcd.print("       ");
  lcd.setCursor(13, 2);
  lcd.print("       ");
  lcd.setCursor(14, 3);
  lcd.print("      ");
}