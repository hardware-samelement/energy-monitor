#include "iot_service.h"
#include "WiFiType.h"
#include "esp32-hal-gpio.h"
M1128 iot;
HardwareSerial *SerialDEBUG = &Serial;
String addr;

extern String SSID;
extern String PASS;

void iot_init() {
  addr = String(DEVELOPER_ROOT) + "S";
  addr = addr + String((uint32_t)(ESP.getEfuseMac() >> 32));

  pinMode(DEVICE_PIN_RESET, INPUT_PULLUP);
  iot.pinReset = DEVICE_PIN_RESET;
  iot.prod = true;
  iot.cleanSession = true;
  iot.setWill = true;
  iot.apConfigTimeout = 300000;
  iot.devConfig(DEVELOPER_ROOT, DEVELOPER_USER, DEVELOPER_PASS);
  iot.wifiConfig(WIFI_DEFAULT_SSID, WIFI_DEFAULT_PASS);

  iot.onConnect = callbackOnConnect;
  iot.onReconnect = callbackOnReconnect;
  iot.onAPConfigTimeout = callbackOnAPConfigTimeout;
  // iot.onWiFiConnectTimeout = callbackOnWiFiConnectTimeout;
  iot.init(DEBUG ? SerialDEBUG : NULL);

  vTaskDelay(10);
}

void callbackOnConnect() {
  initPublish();
  // initSubscribe();
  iot.mqtt->publish(iot.constructTopic("$state"), "ready", true);
}

void callbackOnReconnect() {
  Serial.println("Reconnected to server.");
  // initSubscribe();
}

void callbackOnAPConfigTimeout() {
  // iot.restart();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  Serial.printf("connecting back to %s : %s\n", SSID.c_str(), PASS.c_str());
  WiFi.begin(SSID, PASS);
}

void callbackOnWiFiConnectTimeout() {
  // iot.restart();
  WiFi.disconnect();
  WiFi.begin(SSID, PASS);
}

void initPublish() {
  if (iot.mqtt->connected()) {
    Serial.println("init publish, mqtt connected");
    iot.mqtt->publish(iot.constructTopic("$state"), "init", false);
    iot.mqtt->publish(iot.constructTopic("$sammy"), "1.0.0", false);
    iot.mqtt->publish(iot.constructTopic("$name"), PROJECT_NAME, false);
    iot.mqtt->publish(iot.constructTopic("$model"), PROJECT_MODEL, false);
    iot.mqtt->publish(iot.constructTopic("$mac"), WiFi.macAddress().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$localip"), WiFi.localIP().toString().c_str(), false);
    iot.mqtt->publish(iot.constructTopic("$fw/name"), "Energy_monitoring", false);
    iot.mqtt->publish(iot.constructTopic("$fw/version"), "1.00", false);
    iot.mqtt->publish(iot.constructTopic("$reset"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$restart"), "true", false);
    iot.mqtt->publish(iot.constructTopic("$nodes"), "sensor[]", false);

    // define node "sensor"
    iot.mqtt->publish(iot.constructTopic("sensor/$name"), "sensor", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$type"), "sensor-01", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$properties"), "voltage,current,real,apparent,energy,pf,data", false);
    iot.mqtt->publish(iot.constructTopic("sensor/$array"), "1-3", false);

    iot.mqtt->publish(iot.constructTopic("sensor_1/voltage/$name"), "Voltage", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/voltage/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/voltage/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/voltage/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/voltage/$unit"), "V", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/voltage/$format"), "100:400", false);

    iot.mqtt->publish(iot.constructTopic("sensor_1/current/$name"), "Current", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/current/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/current/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/current/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/current/$unit"), "A", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/current/$format"), "0:10", false);

    iot.mqtt->publish(iot.constructTopic("sensor_1/real/$name"), "Real Power", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/real/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/real/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/real/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/real/$unit"), "W", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/real/$format"), "0:10000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_1/apparent/$name"), "Apparent Power", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/apparent/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/apparent/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/apparent/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/apparent/$unit"), "VA", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/apparent/$format"), "0:10000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_1/energy/$name"), "Energy", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/energy/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/energy/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/energy/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/energy/$unit"), "kWh", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/energy/$format"), "0:100000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_1/pf/$name"), "Power Factor", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/pf/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/pf/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/pf/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/pf/$unit"), "Power Factor", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/pf/$format"), "-1:1", false);

    iot.mqtt->publish(iot.constructTopic("sensor_1/data/$name"), "All Data", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/data/$settable"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/data/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_1/data/$datatype"), "string", false);

    iot.mqtt->publish(iot.constructTopic("sensor_2/voltage/$name"), "Voltage", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/voltage/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/voltage/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/voltage/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/voltage/$unit"), "V", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/voltage/$format"), "100:400", false);

    iot.mqtt->publish(iot.constructTopic("sensor_2/current/$name"), "Current", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/current/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/current/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/current/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/current/$unit"), "A", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/current/$format"), "0:10", false);

    iot.mqtt->publish(iot.constructTopic("sensor_2/real/$name"), "Real Power", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/real/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/real/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/real/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/real/$unit"), "W", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/real/$format"), "0:10000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_2/apparent/$name"), "Apparent Power", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/apparent/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/apparent/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/apparent/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/apparent/$unit"), "VA", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/apparent/$format"), "0:10000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_2/energy/$name"), "Energy", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/energy/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/energy/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/energy/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/energy/$unit"), "kWh", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/energy/$format"), "0:100000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_2/pf/$name"), "Power Factor", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/pf/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/pf/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/pf/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/pf/$unit"), "Power Factor", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/pf/$format"), "-1:1", false);

    iot.mqtt->publish(iot.constructTopic("sensor_2/data/$name"), "All Data", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/data/$settable"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/data/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_2/data/$datatype"), "string", false);

    iot.mqtt->publish(iot.constructTopic("sensor_3/voltage/$name"), "Voltage", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/voltage/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/voltage/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/voltage/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/voltage/$unit"), "V", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/voltage/$format"), "100:400", false);

    iot.mqtt->publish(iot.constructTopic("sensor_3/current/$name"), "Current", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/current/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/current/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/current/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/current/$unit"), "A", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/current/$format"), "0:10", false);

    iot.mqtt->publish(iot.constructTopic("sensor_3/real/$name"), "Real Power", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/real/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/real/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/real/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/real/$unit"), "W", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/real/$format"), "0:10000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_3/apparent/$name"), "Apparent Power", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/apparent/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/apparent/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/apparent/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/apparent/$unit"), "VA", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/apparent/$format"), "0:10000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_3/energy/$name"), "Energy", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/energy/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/energy/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/energy/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/energy/$unit"), "kWh", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/energy/$format"), "0:100000", false);

    iot.mqtt->publish(iot.constructTopic("sensor_3/pf/$name"), "Power Factor", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/pf/$settable"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/pf/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/pf/$datatype"), "float", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/pf/$unit"), "Power Factor", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/pf/$format"), "-1:1", false);

    iot.mqtt->publish(iot.constructTopic("sensor_3/data/$name"), "All Data", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/data/$settable"), "false", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/data/$retained"), "true", false);
    iot.mqtt->publish(iot.constructTopic("sensor_3/data/$datatype"), "string", false);
  }
}

void iot_loop() {
  iot.loop();
}

void iot_publish(const char *topic, const char *payload, bool retained) {
  if (iot.mqtt->connected()) {
    iot.mqtt->publish(iot.constructTopic(topic), payload, retained);
    Serial.printf("      %s : %s\n", topic, payload);
  }
}

void iot_reset() {
  iot.reset();
}

bool iot_isConnected() {
  if (iot.mqtt->connected()) {
    return true;
  } else
    false;
}
