#include <console.h>
#include <EEPROM.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

_version version = {0, 0, 0};

const uint8_t menu_length = 3;

void ssidCallback(const char *arg, const uint8_t length);
void passwordCallback(const char *arg, const uint8_t length);
void resetWifiCallback(const char *arg, const uint8_t length);

cmd_t commands[] = {{"s", "ssid", ssidCallback, "\tSet WiFi SSID."},
                    {"p", "password", passwordCallback, "\tSet WiFi password."},
                    {"",  "reset-wifi", resetWifiCallback, "reset WiFi settings."}};

struct settings_s {
  uint8_t address;
  char buffer[30];
  uint8_t size;
};

typedef struct settings_s settings;
settings wifi_ssid = {0, "", 30};
settings wifi_password = {30, "", 30};

void setup() {
  Serial.begin(115200);
#ifdef ESP8266
  EEPROM.begin(512);
#endif
  delay(1000);

  console.version = version;
  console.setPrinter(Serial);
  console.setFilter(Level::vvvv);
  console.v().pln().pln();
  console.begin(commands, menu_length);

  wifi_ssid.address = 0;
  wifi_password.address = 30;

  readEEPROM(wifi_ssid.address, wifi_ssid.buffer, wifi_ssid.size);
  readEEPROM(wifi_password.address, wifi_password.buffer, wifi_password.size);

  bool wifi_params = true;
  if (strlen(wifi_ssid.buffer) <= 0) {
    console.pln("No WiFi SSID set");
    wifi_params = false;
  }
  if (strlen(wifi_password.buffer) <= 0) {
    console.pln("No WiFi password set");
    wifi_params = false;
  }

  if (!wifi_params) {
    console.v().pln("SYSTEM HALTED! please update WiFi settings.");
    while (true) {
      console.update();
    }
  } else {
    WiFi.begin((char *)wifi_ssid.buffer, (char *)wifi_password.buffer);
    while (WiFi.status() != WL_CONNECTED) {
      console.update();
      delay(500);
      console.vvvv().p(".");
    }
    console.vv().pln("WiFi connected.");
  }
}

void loop() { 
  console.update(); 
}

/***********************
  // Callbacks.
************************/

void ssidCallback(const char *arg, const uint8_t length) {
  if (length && length <= wifi_ssid.size) {
    writeEEPROM(wifi_ssid.address, (char *)arg);
    console.vv().pln("Reboot system to apply WiFi settings.");
  } else if(length==0) {
    readEEPROM(wifi_ssid.address, wifi_ssid.buffer, wifi_ssid.size);
    console.vv().p("SSID: ").pln(wifi_ssid.buffer);
  }else{
    console.v().pln("#ERROR!");
  }
}

void passwordCallback(const char *arg, const uint8_t length) {
  if (length && length <= wifi_password.size) {
    writeEEPROM(wifi_password.address, (char *)arg);
    console.vv().pln("Reboot system to apply WiFi settings.");
  } else if(length==0) {
    readEEPROM(wifi_password.address, wifi_password.buffer, wifi_password.size);
    console.vv().p("Password: ").pln(wifi_password.buffer);
  }else{
    console.v().pln("#ERROR!");
  }
}

void resetWifiCallback(const char *arg, const uint8_t length) {
  writeEEPROM(wifi_ssid.address, "");
  writeEEPROM(wifi_password.address, "");
}

void readEEPROM(uint8_t start_address, char *buffer, uint8_t buffer_size) {
  uint8_t buffer_address = 0;
  for (uint8_t i = start_address; i < start_address + 50; i++) {
    char in = EEPROM.read(i);
    if (buffer_address >= 50 || buffer_address >= buffer_size + start_address) {
      console.v().pln("#ERROR! buffer overflow.");
      buffer[buffer_address] = '\0';
      break;
    }
    if (in == '\0') {
      buffer[buffer_address] = '\0';
      break;
    }
    buffer[buffer_address++] = in;
  }
}

void writeEEPROM(uint8_t start_address, char *buffer) {
  uint8_t buffer_length = strlen(buffer);
  uint8_t buffer_address = 0;
  for (int i = start_address; i <= start_address + buffer_length; i++) {
    if (i >= start_address + buffer_length)
      EEPROM.write(i, '\0');
    else
      EEPROM.write(i, buffer[buffer_address++]);
  }
#ifdef ESP8266
  EEPROM.commit();
#endif
}