#ifndef _WIFI_SERVICE_H__
#define _WIFI_SERVICE_H__

#include "M1128.h"

#define DEBUG true

// #define DEVELOPER_ROOT "1"
// #define DEVELOPER_USER "dmI0OkvoFRLRzHu3J3tEWQbIXQwDeF9q"  //Token user
// #define DEVELOPER_PASS "dyUiAb1cjkS8FRrokTXxtY1s4DUmOJsa"  // Token password

#define DEVELOPER_ROOT "180"
#define DEVELOPER_USER "5nUVTlrDwBvqPyUE4sXcYbkOVSMKe41k"                                                                                                 // Token user
#define DEVELOPER_PASS "sWfmhHZNiue729M7CJkAVEoQxbBLgC6Izl4R24FlzTtC3fKFZ4fUJDLhOm34Z9NyvSpyW5TvZm82CSHBWXwGBhXqD1vDaicGbCe7H8DGvLI7DqJAnfhYxfobgUOrc07o" // Token password

#define WIFI_DEFAULT_SSID "Energy Monitor"
#define WIFI_DEFAULT_PASS "abcd1234"

#define DEVICE_PIN_RESET 13

#define PROJECT_NAME          "Energy_Monitor"
#define PROJECT_MODEL         "SAM-EMS01"
#define FIRMWARE_NAME         "EnergyMonitor"
#define FIRMWARE_VERSION      "1.00"
#define FIRMWARE_VERSION_CODE 1

void iot_init(void);
void iot_reconnectWifi(String ssid, String pass);
void callbackOnConnect();
void callbackOnReconnect();
void callbackOnAPConfigTimeout();
void callbackOnWiFiConnectTimeout();
void initPublish();
void iot_loop();
void iot_publish(const char *topic, const char *payload, bool retained = true);
void iot_reset();

#endif
