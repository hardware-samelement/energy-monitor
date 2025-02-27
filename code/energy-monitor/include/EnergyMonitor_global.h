#ifndef energymonitor_global_h
#define energymonitor_global_h

#include "EmonLib.h" //https://github.com/Savjee/EmonLib-esp32
#include "ModbusIP_ESP8266.h"
#include "file_rw.h"
#include "iot_service.h"
#include "lcd_display.h"
#include "mbCallBack.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ModbusRTU.h>
#include <WiFiMulti.h>

#include <BLEDevice.h>
#include <BLEScan.h>

#define PIN_V1  36
#define PIN_V2  39
#define PIN_V3  34
#define PIN_CT1 35
#define PIN_CT2 32
#define PIN_CT3 33

#define PIN_LED_I1 25
#define PIN_LED_I2 26
#define PIN_LED_I3 27

#define PIN_EN_RS485 4

#define PIN_BTN1 19
#define PIN_BTN2 18

typedef struct {
  float realPower, apparentPower, powerFactor, Vrms, Irms, kwh;
} ReadData;

typedef struct {
  int baudrate, dataLength, parity, stopBit, address;
} ModbusConfig;

typedef struct {
  float cal_v1, cal_v2, cal_v3, cal_i1, cal_i2, cal_i3;
} SensorCalibration;

enum calibrationId {
  V1,
  V2,
  V3,
  I1,
  I2,
  I3
};

void calibration(calibrationId id, float value);

#endif