#include "mbCallBack.h"
#include "EnergyMonitor_global.h"
#include "emonlib.h"
#include "file_rw.h"

// extern ModbusConfig mbConf;

extern EnergyMonitor emon1;
extern EnergyMonitor emon2;
extern EnergyMonitor emon3;

extern ModbusRTU mb;
extern ModbusConfig mbConf;
extern FileRW fileRW;
extern JsonDocument configJson;
extern SensorCalibration cal;
extern String config;
extern int baudrate[];

int val_v1;
int val_v2;
int val_v3;
int val_i1;
int val_i2;
int val_i3;

int serialConfig(int _dataLength, int _parity, int _stopbit) {
  if (_dataLength == 0) {
    if (_parity == 0) {
      if (_stopbit == 0) {
        return SERIAL_7N1;
      } else if (_stopbit == 1) {
        return SERIAL_7N2;
      }
    } else if (_parity == 1) {
      if (_stopbit == 0) {
        return SERIAL_7O1;
      } else if (_stopbit == 1) {
        return SERIAL_7O2;
      }
    } else if (_parity == 2) {
      if (_stopbit == 0) {
        return SERIAL_7E1;
      } else if (_stopbit == 1) {
        return SERIAL_7E2;
      }
    }
  } else if (_dataLength == 1) {
    if (_parity == 0) {
      if (_stopbit == 0) {
        return SERIAL_8N1;
      } else if (_stopbit == 1) {
        return SERIAL_8N2;
      }
    } else if (_parity == 1) {
      if (_stopbit == 0) {
        return SERIAL_8O1;
      } else if (_stopbit == 1) {
        return SERIAL_8O2;
      }
    } else if (_parity == 2) {
      if (_stopbit == 0) {
        return SERIAL_8E1;
      } else if (_stopbit == 1) {
        return SERIAL_8E2;
      }
    }
  }
  return 0;
}

uint16_t cb_baudrate(TRegister *reg, uint16_t val) {
  Serial.println(val);
  Serial2.flush();
  Serial2.end();
  Serial2.begin(baudrate[val], serialConfig(mbConf.dataLength, mbConf.parity, mbConf.stopBit));

  configJson["mbBaudrate"] = val;
  serializeJson(configJson, config);
  fileRW.writeFile(SPIFFS, "/config.json", config);
  return val;
}

uint16_t cb_dataLength(TRegister *reg, uint16_t val) {
  Serial.println(val);
  Serial2.flush();
  Serial2.end();
  Serial2.begin(baudrate[mbConf.baudrate], serialConfig(val, mbConf.parity, mbConf.stopBit));

  configJson["mbDataLength"] = val;
  serializeJson(configJson, config);
  fileRW.writeFile(SPIFFS, "/config.json", config);
  return val;
}

uint16_t cb_parity(TRegister *reg, uint16_t val) {
  Serial.println(val);
  Serial2.flush();
  Serial2.end();
  Serial2.begin(baudrate[mbConf.baudrate], serialConfig(mbConf.dataLength, val, mbConf.stopBit));

  configJson["mbParity"] = val;
  serializeJson(configJson, config);
  fileRW.writeFile(SPIFFS, "/config.json", config);
  return val;
}

uint16_t cb_stopbit(TRegister *reg, uint16_t val) {
  Serial.println(val);
  Serial2.flush();
  Serial2.end();
  Serial2.begin(baudrate[mbConf.baudrate], serialConfig(mbConf.dataLength, mbConf.parity, val));

  configJson["mbStopbit"] = val;
  serializeJson(configJson, config);
  fileRW.writeFile(SPIFFS, "/config.json", config);
  return val;
}

uint16_t cb_address(TRegister *reg, uint16_t val) {
  Serial.println(val);
  mb.server(val);
  configJson["mbAddress"] = val;
  serializeJson(configJson, config);
  fileRW.writeFile(SPIFFS, "/config.json", config);
  return val;
}

uint16_t cb_cal_v1(TRegister *reg, uint16_t val) {
  if (reg->address.address == 0x2100) {
    val_v1 = val << 16;
  }
  if (reg->address.address == 0x2101) {
    val_v1 += val;
    Serial.println(val_v1);

    if (val_v1 > 10000 && val_v1 < 50000) {
      float newCal = cal.cal_v1 * ((val_v1 / 100.0) / emon1.Vrms);
      cal.cal_v1 = newCal;
      configJson["calV1"] = newCal;
      serializeJson(configJson, config);
      fileRW.writeFile(SPIFFS, "/config.json", config);

      emon1.voltage(PIN_V1, newCal, 1.7); // Voltage: input pin, calibration, phase_shift
      Serial.println(newCal);
    }
  }
  return val;
}

uint16_t cb_cal_v2(TRegister *reg, uint16_t val) {
  if (reg->address.address == 0x2102) {
    val_v2 = val << 16;
  }
  if (reg->address.address == 0x2103) {
    val_v2 += val;
    Serial.println(val_v2);

    if (val_v2 > 10000 && val_v2 < 50000) {
      float newCal = cal.cal_v2 * ((val_v2 / 100.0) / emon2.Vrms);
      cal.cal_v2 = newCal;
      configJson["calV2"] = newCal;
      serializeJson(configJson, config);
      fileRW.writeFile(SPIFFS, "/config.json", config);

      emon2.voltage(PIN_V2, newCal, 1.7); // Voltage: input pin, calibration, phase_shift
      Serial.println(newCal);
    }
  }
  return val;
}

uint16_t cb_cal_v3(TRegister *reg, uint16_t val) {
  if (reg->address.address == 0x2104) {
    val_v3 = val << 16;
  }
  if (reg->address.address == 0x2105) {
    val_v3 += val;
    Serial.println(val_v3);

    if (val_v3 > 10000 && val_v3 < 50000) {
      float newCal = cal.cal_v3 * ((val_v3 / 100.0) / emon3.Vrms);
      cal.cal_v3 = newCal;
      configJson["calV3"] = newCal;
      serializeJson(configJson, config);
      fileRW.writeFile(SPIFFS, "/config.json", config);

      emon3.voltage(PIN_V3, newCal, 1.7); // Voltage: input pin, calibration, phase_shift
      Serial.println(newCal);
    }
  }
  return val;
}

uint16_t cb_cal_i1(TRegister *reg, uint16_t val) {
  if (reg->address.address == 0x2106) {
    val_i1 = val << 16;
  }
  if (reg->address.address == 0x2107) {
    val_i1 += val;
    Serial.println(val_i1);

    if (val_i1 > 0 && val_i1 < 10000) {
      float newCal = cal.cal_i1 * ((val_i1 / 100.0) / emon1.Irms);
      cal.cal_i1 = newCal;
      configJson["calI1"] = newCal;
      serializeJson(configJson, config);
      fileRW.writeFile(SPIFFS, "/config.json", config);

      emon1.current(PIN_CT1, newCal);
      Serial.println(newCal);
    }
  }
  return val;
}

uint16_t cb_cal_i2(TRegister *reg, uint16_t val) {
  if (reg->address.address == 0x2108) {
    val_i2 = val << 16;
  }
  if (reg->address.address == 0x2109) {
    val_i2 += val;
    Serial.println(val_i2);

    if (val_i2 > 0 && val_i2 < 10000) {
      float newCal = cal.cal_i2 * ((val_i2 / 100.0) / emon2.Irms);
      cal.cal_i2 = newCal;
      configJson["calI2"] = newCal;
      serializeJson(configJson, config);
      fileRW.writeFile(SPIFFS, "/config.json", config);

      emon2.current(PIN_CT2, newCal);
      Serial.println(newCal);
    }
  }
  return val;
}

uint16_t cb_cal_i3(TRegister *reg, uint16_t val) {
  if (reg->address.address == 0x210A) {
    val_i3 = val << 16;
  }
  if (reg->address.address == 0x210B) {
    val_i3 += val;
    Serial.println(val_i3);

    if (val_i3 > 0 && val_i3 < 10000) {
      float newCal = cal.cal_i3 * ((val_i3 / 100.0) / emon3.Irms);
      cal.cal_i3 = newCal;
      configJson["calI3"] = newCal;
      serializeJson(configJson, config);
      fileRW.writeFile(SPIFFS, "/config.json", config);

      emon3.current(PIN_CT3, newCal);
      Serial.println(newCal);
    }
  }
  return val;
}
