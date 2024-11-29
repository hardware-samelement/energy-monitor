
#include "EnergyMonitor_global.h"

WiFiMulti wifiMulti;
FileRW fileRW;
// Adafruit_SSD1306 display(128, 32, &Wire, -1);
ModbusRTU mb;
ModbusIP mbIp;
EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;

JsonDocument configJson;
String config;

const String json_filename = "/config.json";

float kwhTotal;

int baudrate[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

ModbusConfig mbConf;

QueueHandle_t calcKwh1_queue;
QueueHandle_t calcKwh2_queue;
QueueHandle_t calcKwh3_queue;
QueueHandle_t kwhReset_queue;
QueueHandle_t display_queue;

TaskHandle_t task1_handle = NULL;

ReadData dataL1;
ReadData dataL2;
ReadData dataL3;

int display_page = 0;

String SSID;
String PASS;

bool singleChannel_mode = false;

SensorCalibration cal;

void task_readAll(void *pvParameter);
void task_readVI1(void *pvParameter);
void task_readVI2(void *pvParameter);
void task_readVI3(void *pvParameter);
void task_calcKwh(void *pvParameter);
void task_button(void *pvParameter);
void task_display(void *pvParameter);
void task_modbus(void *pvParameter);
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void task_wifi(void *pvParameter);
void configInit();
void configJsonSet();
void buttonShortPressed();
void buttonLongPressed();
void kwhReset(uint8_t ch);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(15, INPUT);

  pinMode(PIN_LED_I1, OUTPUT);
  pinMode(PIN_LED_I2, OUTPUT);
  pinMode(PIN_LED_I3, OUTPUT);

  digitalWrite(PIN_LED_I1, HIGH);
  digitalWrite(PIN_LED_I2, HIGH);
  digitalWrite(PIN_LED_I3, HIGH);

  configInit();

  calcKwh1_queue = xQueueCreate(10, sizeof(float));
  calcKwh2_queue = xQueueCreate(10, sizeof(float));
  calcKwh3_queue = xQueueCreate(10, sizeof(float));
  kwhReset_queue = xQueueCreate(3, sizeof(uint8_t));
  display_queue = xQueueCreate(10, sizeof(int));
  xTaskCreatePinnedToCore(task_readAll, "readVI all line", 1024 * 8, NULL, 5, NULL, 1);
  // xTaskCreate(task_readVI1, "readVI line 1", 1024 * 8, NULL, 5, NULL);
  // xTaskCreate(task_readVI2, "readVI line 2", 1024 * 8, NULL, 5, NULL);
  // xTaskCreate(task_readVI3, "readVI line 3", 1024 * 8, NULL, 5, NULL);
  xTaskCreatePinnedToCore(task_calcKwh, "calculate kwh", 1024 * 4, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(task_button, "button", 1024 * 3, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(task_display, "display lcd", 1024 * 4, NULL, 6, NULL, 0);
  xTaskCreatePinnedToCore(task_modbus, "modbus", 1024 * 3, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(task_wifi, "wifi", 1024 * 8, NULL, 3, &task1_handle, 0);

  Serial.printf("connecting back to %s : %s\n", SSID, PASS);
}

void loop() {
  // put your main code here, to run repeatedly:
  vTaskDelay(100);
}

void task_readAll(void *pvParameter) {
  bool read1_skip = false;
  unsigned long int timer_skip1;
  unsigned long int timer_skip2;
  bool read2_skip = false;

  emon1.voltage(PIN_V1, cal.cal_v1, 1.7); // Voltage: input pin, calibration, phase_shift
  emon1.current(PIN_CT1, cal.cal_i1);     // Current: input pin, calibration.
  emon2.voltage(PIN_V2, cal.cal_v2, 1.7); // Voltage: input pin, calibration, phase_shift
  emon2.current(PIN_CT2, cal.cal_i2);     // Current: input pin, calibration.
  emon3.voltage(PIN_V3, cal.cal_v3, 1.7); // Voltage: input pin, calibration, phase_shift
  emon3.current(PIN_CT3, cal.cal_i3);     // Current: input pin, calibration.
  vTaskDelay(1);

  for (int i = 0; i < 10; i++) {
    emon1.calcVI(20, 2000);
    vTaskDelay(1);
    emon2.calcVI(20, 2000);
    vTaskDelay(1);
    emon3.calcVI(20, 2000);
    vTaskDelay(1);
  }

  while (1) {
    emon1.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
    dataL1.realPower = emon1.realPower;
    xQueueSend(calcKwh1_queue, &dataL1.realPower, 0);
    dataL1.apparentPower = emon1.apparentPower;
    dataL1.powerFactor = emon1.powerFactor;
    dataL1.Vrms = emon1.Vrms;
    dataL1.Irms = emon1.Irms;

    // Serial.printf("Vrms : %f;   Irms : %f;    real : %f;   apparent : %f;   pf : %f; \r\n", dataL1.Vrms, dataL1.Irms, dataL1.realPower, dataL1.apparentPower, dataL1.powerFactor);
    vTaskDelay(10);

    if (!read1_skip) {
      emon2.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
      dataL2.realPower = emon2.realPower;
      xQueueSend(calcKwh2_queue, &dataL2.realPower, 0);
      dataL2.apparentPower = emon2.apparentPower;
      dataL2.powerFactor = emon2.powerFactor;
      dataL2.Vrms = emon2.Vrms;
      dataL2.Irms = emon2.Irms;
      vTaskDelay(10);
      // Serial.printf("Vrms : %f;   Irms : %f;    real : %f;   apparent : %f;   pf : %f; \r\n", dataL2.Vrms, dataL2.Irms, dataL2.realPower, dataL2.apparentPower, dataL2.powerFactor);
    }

    if (dataL2.Vrms == 0 && !read1_skip) {
      Serial.println("ch2 skipped");
      read1_skip = true;
      timer_skip1 = millis();
    }
    if (millis() - timer_skip1 > 1000) {
      read1_skip = false;
    }

    if (!read2_skip) {
      emon3.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
      dataL3.realPower = emon3.realPower;
      xQueueSend(calcKwh3_queue, &dataL3.realPower, 0);
      dataL3.apparentPower = emon3.apparentPower;
      dataL3.powerFactor = emon3.powerFactor;
      dataL3.Vrms = emon3.Vrms;
      dataL3.Irms = emon3.Irms;

      // Serial.printf("Vrms : %f;   Irms : %f;    real : %f;   apparent : %f;   pf : %f; \r\n", dataL3.Vrms, dataL3.Irms, dataL3.realPower, dataL3.apparentPower, dataL3.powerFactor);
      vTaskDelay(10);
    }
    if (dataL3.Vrms == 0 && !read2_skip) {
      Serial.println("ch3 skipped");
      read2_skip = true;
      timer_skip2 = millis();
    }
    if (millis() - timer_skip2 > 1000) {
      read2_skip = false;
    }

    if (dataL1.Irms == 0) {
      digitalWrite(PIN_LED_I1, HIGH);
    } else
      digitalWrite(PIN_LED_I1, LOW);
    if (dataL2.Irms == 0) {
      digitalWrite(PIN_LED_I2, HIGH);
    } else
      digitalWrite(PIN_LED_I2, LOW);
    if (dataL3.Irms == 0) {
      digitalWrite(PIN_LED_I3, HIGH);
    } else
      digitalWrite(PIN_LED_I3, LOW);

    if (dataL2.Vrms == 0 && dataL3.Vrms == 0) {
      singleChannel_mode = true;
    } else
      singleChannel_mode = false;

    // if (dataL1.Vrms == 0) {
    //   vTaskDelay(1000);
    // }
    vTaskDelay(1);
  }
}

void task_readVI1(void *pvParameter) {
  emon1.voltage(PIN_V1, cal.cal_v1, 1.7); // Voltage: input pin, calibration, phase_shift
  emon1.current(PIN_CT1, cal.cal_i1);     // Current: input pin, calibration.

  for (int i = 0; i < 10; i++) {
    emon1.calcVI(10, 2000);
  }
  vTaskDelay(1);

  while (1) {
    emon1.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
    dataL1.realPower = emon1.realPower;
    xQueueSend(calcKwh1_queue, &dataL1.realPower, 10);
    dataL1.apparentPower = emon1.apparentPower;
    dataL1.powerFactor = emon1.powerFactor;
    dataL1.Vrms = emon1.Vrms;
    dataL1.Irms = emon1.Irms;

    Serial.printf("Vrms : %f;   Irms : %f;    real : %f;   apparent : %f;   pf : %f; \r\n", dataL1.Vrms, dataL1.Irms, dataL1.realPower, dataL1.apparentPower, dataL1.powerFactor);
    vTaskDelay(10);

    if (dataL1.Vrms == 0) {
      vTaskDelay(1000);
    }

    if (dataL1.Irms == 0) {
      digitalWrite(PIN_LED_I1, HIGH);
    } else
      digitalWrite(PIN_LED_I1, LOW);
    yield();
  }
}

void task_readVI2(void *pvParameter) {

  emon2.voltage(PIN_V2, cal.cal_v2, 1.7); // Voltage: input pin, calibration, phase_shift
  emon2.current(PIN_CT2, cal.cal_i2);     // Current: input pin, calibration.

  for (int i = 0; i < 10; i++) {
    emon2.calcVI(10, 2000);
  }
  vTaskDelay(1);

  while (1) {
    emon2.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
    dataL2.realPower = emon2.realPower;
    xQueueSend(calcKwh2_queue, &dataL2.realPower, 10);
    dataL2.apparentPower = emon2.apparentPower;
    dataL2.powerFactor = emon2.powerFactor;
    dataL2.Vrms = emon2.Vrms;
    dataL2.Irms = emon2.Irms;

    // Serial.printf("Vrms : %f;   Irms : %f;    real : %f;   apparent : %f;   pf : %f; \r\n", dataL2.Vrms, dataL2.Irms, dataL2.realPower, dataL2.apparentPower, dataL2.powerFactor);
    vTaskDelay(10);
    if (dataL2.Vrms == 0) {
      vTaskDelay(1000);
    }

    if (dataL1.Irms == 0) {
      digitalWrite(PIN_LED_I2, HIGH);
    } else
      digitalWrite(PIN_LED_I2, LOW);
    yield();
  }
}

void task_readVI3(void *pvParameter) {

  emon3.voltage(PIN_V3, 402.6, 1.7); // Voltage: input pin, calibration, phase_shift
  emon3.current(PIN_CT3, 18.6);      // Current: input pin, calibration.

  for (int i = 0; i < 10; i++) {
    emon3.calcVI(20, 2000);
  }

  while (1) {
    emon3.calcVI(20, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
    dataL3.realPower = emon2.realPower;
    xQueueSend(calcKwh3_queue, &dataL2.realPower, portMAX_DELAY);
    dataL3.apparentPower = emon3.apparentPower;
    dataL3.powerFactor = emon3.powerFactor;
    dataL3.Vrms = emon3.Vrms;
    dataL3.Irms = emon3.Irms;

    // Serial.printf("Vrms : %f;   Irms : %f;    real : %f;   apparent : %f;   pf : %f; \r\n", dataL3.Vrms, dataL3.Irms, dataL3.realPower, dataL3.apparentPower, dataL3.powerFactor);
    vTaskDelay(1);
    if (dataL3.Vrms == 0) {
      vTaskDelay(1000);
    }
    if (dataL1.Irms == 0) {
      digitalWrite(PIN_LED_I3, HIGH);
    } else
      digitalWrite(PIN_LED_I3, LOW);

    yield();
  }
}

void task_calcKwh(void *pvParameter) {
  float realPower;
  float kwh1, mwh1, kwh2, mwh2, kwh3, mwh3;
  unsigned long int lastmillis1, lastmillis2, lastmillis3;
  int tes;
  uint8_t resetCh;

  kwh1 = dataL1.kwh;
  kwh2 = dataL2.kwh;
  kwh3 = dataL3.kwh;

  while (1) {
    if (xQueueReceive(calcKwh1_queue, &realPower, 10)) {
      mwh1 = mwh1 + realPower * (millis() - lastmillis1) / 3600.0;
      // Serial.print("calc kwh1 : ");
      // Serial.println(mwh1);
      if (mwh1 / 10000.0 >= 1) {
        kwh1 = kwh1 + (mwh1 / 1000000.0);
        mwh1 = fmod(mwh1, 10000);

        Serial.printf("kwh1 : %f\n", kwh1);

        configJson["kwh1"] = kwh1;
        serializeJson(configJson, config);
        fileRW.writeFile(SPIFFS, "/config.json", config);
        dataL1.kwh = kwh1;

        kwhTotal = dataL1.kwh + dataL2.kwh + dataL3.kwh;
      }
      lastmillis1 = millis();
    }

    if (xQueueReceive(calcKwh2_queue, &realPower, 10)) {
      mwh2 = mwh2 + realPower * (millis() - lastmillis2) / 3600.0;
      if (mwh2 / 10000.0 >= 1) {
        kwh2 = kwh2 + (mwh2 / 1000000.0);
        mwh2 = fmod(mwh2, 10000);

        Serial.printf("kwh2 : %f\n", kwh2);

        configJson["kwh2"] = kwh2;
        serializeJson(configJson, config);
        fileRW.writeFile(SPIFFS, "/config.json", config);
        dataL2.kwh = kwh2;

        kwhTotal = dataL1.kwh + dataL2.kwh + dataL3.kwh;
      }
      lastmillis2 = millis();
    }

    if (xQueueReceive(calcKwh3_queue, &realPower, 10)) {
      mwh3 = mwh3 + realPower * (millis() - lastmillis3) / 3600.0;
      if (mwh3 / 10000.0 >= 1) {
        kwh3 = kwh3 + (mwh3 / 1000000.0);
        mwh3 = fmod(mwh3, 10000);

        Serial.printf("kwh3 : %f\n", kwh3);

        configJson["kwh3"] = kwh3;
        serializeJson(configJson, config);
        fileRW.writeFile(SPIFFS, "/config.json", config);
        dataL3.kwh = kwh3;

        kwhTotal = dataL1.kwh + dataL2.kwh + dataL3.kwh;
      }
      lastmillis3 = millis();
    }

    if (xQueueReceive(kwhReset_queue, &resetCh, 10)) {
      switch (resetCh) {
      case 1:
        kwh1 = 0;
        mwh1 = 0;
        break;
      case 2:
        kwh2 = 0;
        mwh2 = 0;
        break;
      case 3:
        kwh3 = 0;
        mwh3 = 0;
        break;
      }

      kwhTotal = dataL1.kwh + dataL2.kwh + dataL3.kwh;
    }

    vTaskDelay(1);
    yield();
  }
}

// void task_display2(void *pvParameter) {
//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   display.clearDisplay();

//   while (1) {
//     display.clearDisplay();
//     display.setTextColor(1);
//     display.setCursor(6, 5);
//     display.print("V");
//     display.setCursor(18, 5);
//     display.print(dataL1.Vrms);
//     display.setCursor(6, 20);
//     display.print("I");
//     display.setCursor(18, 20);
//     display.print(dataL1.Irms);
//     display.setCursor(69, 5);
//     display.print("W");
//     display.setCursor(85, 5);
//     display.print(dataL1.realPower);
//     display.setCursor(64, 20);
//     display.print("kWh");
//     display.setCursor(84, 20);
//     display.print(dataL1.kwh);
//     display.drawLine(60, 4, 60, 27, 1);
//     display.display();
//     vTaskDelay(1000);
//   }
// }

void task_button(void *pvParameter) {
  bool butt1_pressed = false;
  bool butt1_longpressed = false;
  unsigned long int butt1_timer;
  pinMode(PIN_BTN1, INPUT_PULLUP);

  while (1) {
    if (digitalRead(PIN_BTN1) == LOW) {
      if (!butt1_pressed) {
        butt1_pressed = true;
        butt1_timer = millis();
      }

      if (millis() - butt1_timer > 5000 && !butt1_longpressed) {
        butt1_longpressed = true;
        buttonLongPressed();
      }

    } else { // button released
      if (butt1_pressed) {
        if (butt1_longpressed) {
          butt1_longpressed = false;
        } else { // short press detected
          buttonShortPressed();
          vTaskDelay(200);
        }
        vTaskDelay(100);
        butt1_pressed = false;
      }
    }
    vTaskDelay(10);
    taskYIELD();
  }
}

void task_display(void *pvParameter) {
  int displayed = 0;
  unsigned long int display_timer = 0;

  lcd_init();
  while (1) {
    if (xQueueReceive(display_queue, &displayed, 0)) {
      lcd_show(displayed);
    }

    if (millis() - display_timer > 1000) {
      lcd_show(displayed);
      display_timer = millis();
    }

    vTaskDelay(10);
    taskYIELD();
  }
}

void task_modbus(void *pvParameter) {
  Serial2.begin(baudrate[mbConf.baudrate], serialConfig(mbConf.dataLength, mbConf.parity, mbConf.stopBit));
  mb.begin(&Serial2, PIN_EN_RS485);

  mb.server(mbConf.address);

  for (int i = 0; i < 0x2D; i++) {
    mb.addIreg(i);
  }

  // modbus setting
  for (int i = 0; i < 8; i++) {
    mb.addHreg(0x2000 + i);
  }

  // device calibration
  for (int i = 0; i < 0x0C; i++) {
    mb.addHreg(0x2100 + i);
  }

  mb.Hreg(0x2000, mbConf.baudrate);
  mb.Hreg(0x2001, mbConf.dataLength);
  mb.Hreg(0x2002, mbConf.parity);
  mb.Hreg(0x2003, mbConf.stopBit);
  mb.Hreg(0x2004, mbConf.address);

  mb.onSetHreg(0x2000, cb_baudrate);
  mb.onSetHreg(0x2001, cb_dataLength);
  mb.onSetHreg(0x2002, cb_parity);
  mb.onSetHreg(0x2003, cb_stopbit);
  mb.onSetHreg(0x2004, cb_address);

  mb.onSetHreg(0x2100, cb_cal_v1, 2);
  mb.onSetHreg(0x2102, cb_cal_v2, 2);
  mb.onSetHreg(0x2104, cb_cal_v3, 2);
  mb.onSetHreg(0x2106, cb_cal_i1, 2);
  mb.onSetHreg(0x2108, cb_cal_i2, 2);
  mb.onSetHreg(0x210A, cb_cal_i3, 2);
  // mb.onSetHreg(1, cb1);

  while (1) {
    mb.Ireg(0x00, (int)(dataL1.Vrms * 100) >> 16);
    mb.Ireg(0x01, dataL1.Vrms * 100);
    mb.Ireg(0x02, (int)(dataL2.Vrms * 100) >> 16);
    mb.Ireg(0x03, dataL2.Vrms * 100);
    mb.Ireg(0x04, (int)(dataL3.Vrms * 100) >> 16);
    mb.Ireg(0x05, dataL3.Vrms * 100);
    vTaskDelay(10);

    mb.Ireg(0x06, (int)(dataL1.Irms * 100) >> 16);
    mb.Ireg(0x07, dataL1.Irms * 100);
    mb.Ireg(0x08, (int)(dataL2.Irms * 100) >> 16);
    mb.Ireg(0x09, dataL2.Irms * 100);
    mb.Ireg(0x0A, (int)(dataL3.Irms * 100) >> 16);
    mb.Ireg(0x0B, dataL3.Irms * 100);
    vTaskDelay(10);

    mb.Ireg(0x0C, (int)(dataL1.realPower * 100) >> 16);
    mb.Ireg(0x0D, dataL1.realPower * 100);
    mb.Ireg(0x0E, (int)(dataL2.realPower * 100) >> 16);
    mb.Ireg(0x0F, dataL2.realPower * 100);
    mb.Ireg(0x10, (int)(dataL3.realPower * 100) >> 16);
    mb.Ireg(0x11, dataL3.realPower * 100);
    vTaskDelay(10);

    mb.Ireg(0x12, (int)(dataL1.apparentPower * 100) >> 16);
    mb.Ireg(0x13, dataL1.apparentPower * 100);
    mb.Ireg(0x14, (int)(dataL2.apparentPower * 100) >> 16);
    mb.Ireg(0x15, dataL2.apparentPower * 100);
    mb.Ireg(0x16, (int)(dataL3.apparentPower * 100) >> 16);
    mb.Ireg(0x17, dataL3.apparentPower * 100);
    vTaskDelay(10);

    mb.Ireg(0x18, (int)(dataL1.kwh * 100) >> 16);
    mb.Ireg(0x19, dataL1.kwh * 100);
    mb.Ireg(0x1A, (int)(dataL2.kwh * 100) >> 16);
    mb.Ireg(0x1B, dataL2.kwh * 100);
    mb.Ireg(0x1C, (int)(dataL3.kwh * 100) >> 16);
    mb.Ireg(0x1D, dataL3.kwh * 100);
    vTaskDelay(10);

    mb.Ireg(0x1E, (int)(kwhTotal * 100) >> 16);
    mb.Ireg(0x1F, (kwhTotal * 100));

    mb.Ireg(0x20, (int)(dataL1.powerFactor * 100) >> 16);
    mb.Ireg(0x21, dataL1.powerFactor * 100);
    mb.Ireg(0x22, (int)(dataL2.powerFactor * 100) >> 16);
    mb.Ireg(0x23, dataL2.powerFactor * 100);
    mb.Ireg(0x24, (int)(dataL3.powerFactor * 100) >> 16);
    mb.Ireg(0x25, dataL3.powerFactor * 100);
    vTaskDelay(10);
    // mb.Ireg(0x2E, )

    mb.task();
    vTaskDelay(100);
    yield();
  }
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to AP successfully!");
  Serial.print("WiFi connected to: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("pass: ");
  Serial.println(WiFi.psk());
  Serial.print("mode: ");
  Serial.println(WiFi.getMode());

  configJson["wfSsid"] = WiFi.SSID();
  configJson["wfPass"] = WiFi.psk();

  serializeJson(configJson, config);
  fileRW.writeFile(SPIFFS, "/config.json", config);
  // wifiMulti.addAP(configJson["wfSsid"], configJson["wfPass"]);

  xQueueSend(display_queue, &display_page, portMAX_DELAY);
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  vTaskDelay(1000);
  if (WiFi.getMode() == WIFI_MODE_STA) {
    Serial.println("Trying to Reconnect");
    String SSID = configJson["wfSsid"];
    String PASS = configJson["wfPass"];
    WiFi.begin(SSID, PASS);
  }
}

void task_wifi(void *pvParameter) {
  unsigned long int starttime;
  unsigned long int publishTimer;
  int publishInterval = 600000;
  bool firstPublish = true;
  int apPage = display_info + 1;

  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  iot_init();
  mbIp.server();
  vTaskDelay(10000);

  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      if (millis() - publishTimer > 300000 || firstPublish) {
        firstPublish = false;
        String allData = String(dataL1.Vrms) + "|" + String(dataL1.Irms) + "~" + String(dataL1.realPower) + "~" + String(dataL1.apparentPower) + "~" + String(dataL1.kwh) + "~" + String(dataL1.powerFactor);
        iot_publish("sensor_1/voltage", String(dataL1.Vrms).c_str());
        iot_publish("sensor_1/current", String(dataL1.Irms).c_str());
        iot_publish("sensor_1/real", String(dataL1.realPower).c_str());
        iot_publish("sensor_1/apparent", String(dataL1.apparentPower).c_str());
        iot_publish("sensor_1/energy", String(dataL1.kwh).c_str());
        iot_publish("sensor_1/pf", String(dataL1.powerFactor).c_str());
        iot_publish("sensor_1/data", allData.c_str());

        allData = String(dataL2.Vrms) + "|" + String(dataL2.Irms) + "~" + String(dataL2.realPower) + "~" + String(dataL2.apparentPower) + "~" + String(dataL2.kwh) + "~" + String(dataL2.powerFactor);
        iot_publish("sensor_2/voltage", String(dataL2.Vrms).c_str());
        iot_publish("sensor_2/current", String(dataL2.Irms).c_str());
        iot_publish("sensor_2/real", String(dataL2.realPower).c_str());
        iot_publish("sensor_2/apparent", String(dataL2.apparentPower).c_str());
        iot_publish("sensor_2/energy", String(dataL2.kwh).c_str());
        iot_publish("sensor_2/pf", String(dataL2.powerFactor).c_str());
        iot_publish("sensor_2/data", allData.c_str());

        allData = String(dataL3.Vrms) + "|" + String(dataL3.Irms) + "~" + String(dataL3.realPower) + "~" + String(dataL3.apparentPower) + "~" + String(dataL3.kwh) + "~" + String(dataL3.powerFactor);
        iot_publish("sensor_3/voltage", String(dataL3.Vrms).c_str());
        iot_publish("sensor_3/current", String(dataL3.Irms).c_str());
        iot_publish("sensor_3/real", String(dataL3.realPower).c_str());
        iot_publish("sensor_3/apparent", String(dataL3.apparentPower).c_str());
        iot_publish("sensor_3/energy", String(dataL3.kwh).c_str());
        iot_publish("sensor_3/pf", String(dataL3.powerFactor).c_str());
        iot_publish("sensor_3/data", allData.c_str());

        publishTimer = millis();
      }
    }

    // reset wifi ssid setting
    if (ulTaskNotifyTake(pdTRUE, 0)) {
      Serial.println("notified");
      xQueueSend(display_queue, &apPage, 0);
      iot_reset();
    }

    mbIp.task();
    vTaskDelay(100);
    iot_loop();
    vTaskDelay(100);
  }
}

void configInit() {
  fileRW.init();

  // configJsonSet();

  config = fileRW.readFile(SPIFFS, json_filename);
  deserializeJson(configJson, config);
  serializeJson(configJson, Serial);

  // configJson["mbBaudrate"] = 3;
  // configJson["mbDatalength"] = 1;
  // configJson["mbParity"] = 0;
  // configJson["mbStopbit"] = 0;
  // configJson["mbAddress"] = 7;

  // configJson["calV1"] = 402.6;
  // configJson["calV2"] = 402.6;
  // configJson["calV3"] = 402.6;
  // configJson["calI1"] = 18.6;
  // configJson["calI2"] = 18.6;
  // configJson["calI3"] = 18.6;

  // serializeJson(configJson, config);
  // fileRW.writeFile(SPIFFS, json_filename, config);

  const char *ssid = configJson["wfSsid"];
  const char *pass = configJson["wfPass"];

  SSID = String(ssid);
  PASS = String(pass);

  mbConf.baudrate = configJson["mbBaudrate"];
  mbConf.dataLength = configJson["mbDatalength"];
  mbConf.parity = configJson["mbParity"];
  mbConf.stopBit = configJson["mbStopbit"];
  mbConf.address = configJson["mbAddress"];

  dataL1.kwh = configJson["kwh1"];
  dataL2.kwh = configJson["kwh2"];
  dataL3.kwh = configJson["kwh3"];
  kwhTotal = dataL1.kwh + dataL2.kwh + dataL3.kwh;

  cal.cal_v1 = configJson["calV1"];
  cal.cal_v2 = configJson["calV2"];
  cal.cal_v3 = configJson["calV3"];
  cal.cal_i1 = configJson["calI1"];
  cal.cal_i2 = configJson["calI2"];
  cal.cal_i3 = configJson["calI3"];

  display_page = configJson["displayPage"];
  // mb.onSetHreg(uint16_t offset)
  delay(100);
}

void configJsonSet(void) {
  JsonDocument doc;

  doc["wfSsid"] = "SAM-IND";
  doc["wfPass"] = "SamElementD8";
  doc["mbBaudrate"] = 3;
  doc["mbDatalength"] = 1;
  doc["mbParity"] = 0;
  doc["mbStopbit"] = 0;
  doc["mbAddress"] = 7;
  doc["calV1"] = 404.0579224;
  doc["calV2"] = 402.6;
  doc["calV3"] = 402.6;
  doc["calI1"] = 17.93642044;
  doc["calI2"] = 18.6;
  doc["calI3"] = 18.6;
  doc["kwh3"] = 0.490415394;
  doc["kwh2"] = 0.500533819;
  doc["kwh1"] = 0.49030301;

  String output;

  doc.shrinkToFit(); // optional

  serializeJson(doc, output);
  fileRW.writeFile(SPIFFS, "/config.json", output);
}

void buttonShortPressed() {
  Serial.println("short pressed");

  display_page += 1;
  if (display_page > display_info) {
    display_page = 0;
  }
  xQueueSend(display_queue, &display_page, 0);

  configJson["displayPage"] = display_page;
  serializeJson(configJson, config);
  fileRW.writeFile(SPIFFS, "/config.json", config);
}

void buttonLongPressed() {
  Serial.println("long pressed");
  if (display_page == display_info) {
    xTaskNotifyGive(task1_handle); // notify to reset wifi ssid
  } else if (display_page == display_ch1) {
    dataL1.kwh = 0;
    kwhReset(1);

    configJson["kwh1"] = dataL1.kwh;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);
  } else if (display_page == display_ch2) {
    dataL2.kwh = 0;
    kwhReset(2);

    configJson["kwh2"] = dataL2.kwh;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);
  } else if (display_page == display_ch3) {
    dataL3.kwh = 0;
    kwhReset(3);

    configJson["kwh3"] = dataL3.kwh;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);
  }
}

void kwhReset(uint8_t ch) {
  xQueueSend(kwhReset_queue, &ch, 0);
}

void calibration(calibrationId id, float value) {
  Serial.printf("id = %d, value : %f /n", id, value);
  if (id == V1) {
    float newCal = cal.cal_v1 * (value / emon1.Vrms);
    cal.cal_v1 = newCal;
    configJson["calV1"] = newCal;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);

    emon1.voltage(PIN_V1, newCal, 1.7); // Voltage: input pin, calibration, phase_shift
    Serial.println(newCal);
  } else if (id == V2) {
    float newCal = cal.cal_v2 * (value / emon2.Vrms);
    cal.cal_v2 = newCal;
    configJson["calV2"] = newCal;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);

    emon2.voltage(PIN_V2, newCal, 1.7); // Voltage: input pin, calibration, phase_shift
    Serial.println(newCal);
  } else if (id == V3) {
    float newCal = cal.cal_v3 * (value / emon3.Vrms);
    cal.cal_v3 = newCal;
    configJson["calV3"] = newCal;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);

    emon3.voltage(PIN_V3, newCal, 1.7); // Voltage: input pin, calibration, phase_shift
    Serial.println(newCal);
  } else if (id == I1) {
    float newCal = cal.cal_i1 * (value / emon1.Irms);
    cal.cal_i1 = newCal;
    configJson["calI1"] = newCal;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);

    emon1.current(PIN_CT1, newCal);
    Serial.println(newCal);
  } else if (id == I2) {
    float newCal = cal.cal_i2 * (value / emon2.Irms);
    cal.cal_i2 = newCal;
    configJson["calI2"] = newCal;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);

    emon2.current(PIN_CT2, newCal);
    Serial.println(newCal);
  } else if (id == I3) {
    float newCal = cal.cal_i3 * (value / emon3.Irms);
    cal.cal_i3 = newCal;
    configJson["calI3"] = newCal;
    serializeJson(configJson, config);
    fileRW.writeFile(SPIFFS, "/config.json", config);

    emon3.current(PIN_CT3, newCal);
    Serial.println(newCal);
  }
}
