/*
    @file       express_wifi.cpp
    @author     matkappert
    @repo       github.com/matkappert/express_wifi
    @date       14/11/20
*/

#include "express_wifi.h"
// #include <WiFi.h>

#if (USE_WIFI == true)
  #if (USE_MENU == true)
    #include <esp_wifi.h>

    #include "express_console_menu.h"

express_wifi eWifi;  // global-scoped variable

// WIFI_REASON_UNSPECIFIED              = 1,
// WIFI_REASON_AUTH_EXPIRE              = 2,
// WIFI_REASON_AUTH_LEAVE               = 3,
// WIFI_REASON_ASSOC_EXPIRE             = 4,
// WIFI_REASON_ASSOC_TOOMANY            = 5,
// WIFI_REASON_NOT_AUTHED               = 6,
// WIFI_REASON_NOT_ASSOCED              = 7,
// WIFI_REASON_ASSOC_LEAVE              = 8,
// WIFI_REASON_ASSOC_NOT_AUTHED         = 9,
// WIFI_REASON_DISASSOC_PWRCAP_BAD      = 10,
// WIFI_REASON_DISASSOC_SUPCHAN_BAD     = 11,
// WIFI_REASON_IE_INVALID               = 13,
// WIFI_REASON_MIC_FAILURE              = 14,
// WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
// WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
// WIFI_REASON_IE_IN_4WAY_DIFFERS       = 17,
// WIFI_REASON_GROUP_CIPHER_INVALID     = 18,
// WIFI_REASON_PAIRWISE_CIPHER_INVALID  = 19,
// WIFI_REASON_AKMP_INVALID             = 20,
// WIFI_REASON_UNSUPP_RSN_IE_VERSION    = 21,
// WIFI_REASON_INVALID_RSN_IE_CAP       = 22,
// WIFI_REASON_802_1X_AUTH_FAILED       = 23,
// WIFI_REASON_CIPHER_SUITE_REJECTED    = 24,

// WIFI_REASON_BEACON_TIMEOUT           = 200,
// WIFI_REASON_NO_AP_FOUND              = 201,
// WIFI_REASON_AUTH_FAIL                = 202,
// WIFI_REASON_ASSOC_FAIL               = 203,
// WIFI_REASON_HANDSHAKE_TIMEOUT        = 204,
// WIFI_REASON_CONNECTION_FAIL          = 205,

void express_wifi::wifi_event(WiFiEvent_t event, WiFiEventInfo_t info) {
  eMenu.vvvv().p("Network event: ").pln(eWifi.system_event_id_cstr[(int)event]);
  if (event == SYSTEM_EVENT_STA_DISCONNECTED || event == SYSTEM_EVENT_AP_STADISCONNECTED) {
    eMenu.vvvv().p("Reason of disconnection: ").pln(info.disconnected.reason);
    delay(100);
    eWifi.wifi_event_disconnected();
    if (info.disconnected.reason <= 24) {
      eWifi.connection_retries = 0;
      eWifi.establish_connection();
    }
  } else if (event == SYSTEM_EVENT_STA_GOT_IP || event == SYSTEM_EVENT_AP_STA_GOT_IP6 || event == SYSTEM_EVENT_AP_STAIPASSIGNED) {
    eWifi.connection_retries = 0;
    eMenu.vvvv().pln("Obtained an IP address");
  }
}

struct express_wifi::menu_wifi_ssid : menu_item {
  menu_wifi_ssid() : menu_item({(char *)"Set WiFi SSID."}) {
    this->commands.push_back((char *)"s");
    this->commands.push_back((char *)"ssid");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.printBox(true);
      eMenu.v().pln("   Scanning for Networks...").pln();
      eWifi.scan();
      for (int i = 0; i < eWifi.numSSID; i++) {
        eMenu.v().p("\t").p(i + 1).p(") ").pln(eWifi.ssidList[i]);
      }
      eMenu.printBox(false);

    } else if (length == 1) {
      int selected = atoi(arg);
      eMenu.vvvv().p("selected:").pln(selected);
      if (selected > 0 && selected <= eWifi.numSSID) {
        eWifi.wifiData.ssid[0] = '\0';
        strcpy(eWifi.wifiData.ssid, eWifi.ssidList[selected - 1]);
        eMenu.v().p("New SSID: ").pln(eWifi.wifiData.ssid);
        eWifi.save_settings();
      } else {
        if (eWifi.numSSID == 0) {
          eMenu.v().pln("ERROR: scan for WiFi SSID before picking one.");
        } else {
          eMenu.v().p("ERROR: unkown option, pick a rang from 1-").pln(eWifi.numSSID > 9 ? 9 : eWifi.numSSID);
        }
      }
    } else {
      if (length <= MAX_SSID) {
        eWifi.wifiData.ssid[0] = '\0';
        strcpy(eWifi.wifiData.ssid, arg);

        eWifi.save_settings();

        eMenu.v().p("New SSID: ").pln(eWifi.wifiData.ssid);
      } else {
        eMenu.v().p("ERROR: to long! max=").pln(MAX_SSID);
      }
    }
  }
};

struct express_wifi::menu_wifi_password : menu_item {
  menu_wifi_password() : menu_item({(char *)"Set WiFi password."}) {
    this->commands.push_back((char *)"p");
    this->commands.push_back((char *)"pass");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("Password: ").pln(eMenu.mask(eWifi.wifiData.pwd, 2));
    } else {
      if (length <= MAX_PWD) {
        eWifi.wifiData.pwd[0] = '\0';
        strcpy(eWifi.wifiData.pwd, arg);

        eWifi.save_settings();

        eMenu.v().p("New password: ").pln(eMenu.mask(eWifi.wifiData.pwd, 2));
      } else {
        eMenu.v().p("ERROR: to long! max=").pln(MAX_PWD);
      }
    }
  }
};

struct express_wifi::menu_wifi_sta : menu_item {
  menu_wifi_sta() : menu_item({(char *)"Station or Access point"}) {
    this->commands.push_back((char *)"sta");
    this->commands.push_back((char *)"station");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("STA Mode: ").pln(eWifi.wifiData.is_station_mode ? "TRUE" : "FALSE");
    } else {
      isTrue_t value = eMenu.argIsTrue(arg);
      if (value == isError) {
        eMenu.v().pln("ERROR: unkown input! [true|false]");
        return;
      } else {
        eMenu.v().p("STA Mode: ").pln(value ? "TRUE" : "FALSE");
        eWifi.wifiData.is_station_mode = value;
        eWifi.save_settings();
      }
    }
  }
};

struct express_wifi::menu_wifi_static : menu_item {
  menu_wifi_static() : menu_item({(char *)"Static IP Address"}) {
    this->commands.push_back((char *)"ip");
    this->commands.push_back((char *)"static");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("Static IP Address: ").pln(eWifi.wifiData.is_static_IP ? "Static" : "Auto");
    } else {
      isTrue_t value = eMenu.argIsTrue(arg);
      if (value == isError) {
        eMenu.v().pln("ERROR: unkown input! [true|false]");
        return;
      } else {
        eMenu.v().p("Static IP Address: ").pln(value ? "Static" : "Auto");
        eWifi.wifiData.is_static_IP = value;
        eWifi.save_settings();
      }
    }
  }
};

struct express_wifi::menu_wifi_info : menu_item {
  menu_wifi_info() : menu_item({(char *)"WiFi Info."}) {
    this->commands.push_back((char *)"i");
    this->commands.push_back((char *)"info");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.printBox(true);
    eMenu.v().pln("   INFO:").pln();

    if (WiFi.getMode() == WIFI_MODE_NULL) {
      eMenu.printInfo("Status", "DISABLED");
    } else {
      eMenu.printInfo("Status", eWifi.wl_status_cstr[(uint8_t)WiFi.status() == 255 ? 7 : (uint8_t)WiFi.status()]);
    }

    if (WiFi.getMode() == WIFI_MODE_STA) {
      eMenu.printInfo("Mode", "STA");
      eMenu.printInfo("SSID", WiFi.SSID().c_str());
      eMenu.printInfo("Password", eMenu.mask(WiFi.psk().c_str(), 2).c_str());
      eMenu.printInfo("RSSI", WiFi.RSSI());
      eMenu.printInfo("IPv4 Address", WiFi.localIP().toString().c_str());
        eMenu.printInfo("IPv6 Address", WiFi.localIPv6().toString().c_str());
      eMenu.printInfo("Subnet Mask", WiFi.subnetMask().toString().c_str());
      eMenu.printInfo("Router", WiFi.gatewayIP().toString().c_str());
      eMenu.v().pln();
      eMenu.printInfo("MAC Address", WiFi.macAddress().c_str());
      eMenu.printInfo("Hostname", WiFi.getHostname());

    } else if (WiFi.getMode() == WIFI_MODE_AP) {
      eMenu.printInfo("Mode", "AP");
      eMenu.printInfo("SSID", WiFi.softAPSSID().c_str());
      eMenu.printInfo("Password", eMenu.mask(eWifi.wifiData.softap_pwd, 2).c_str());
      eMenu.printInfo("IPv4 Address", WiFi.softAPIP().toString().c_str());
        eMenu.printInfo("IPv6 Address", WiFi.softAPIPv6().toString().c_str());
      eMenu.printInfo("MAC Address", WiFi.softAPmacAddress().c_str());
      eMenu.printInfo("Hostname", WiFi.softAPgetHostname());
    }else{
      eMenu.printInfo("Mode", "UNKNOWN");
    }

    eMenu.v().pln();
    eMenu.printInfo("Auto Connect", WiFi.getAutoConnect() ? "TRUE" : "FALSE");
    eMenu.printInfo("Auto Reconnect", WiFi.getAutoReconnect() ? "TRUE" : "FALSE");
    eMenu.printInfo("Modem Sleep", WiFi.getSleep() ? "TRUE" : "FALSE");
    eMenu.printInfo("TX Power", WiFi.getTxPower());

    eMenu.printBox(false);

    eWifi.print_settings();
  }
};

struct express_wifi::menu_wifi_reconnect : menu_item {
  menu_wifi_reconnect() : menu_item({(char *)"Reapply WiFi settings"}) {
    this->commands.push_back((char *)"c");
    this->commands.push_back((char *)"connect");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().pln("Reapply WiFi settings!");
      eWifi.establish_connection();
    } else {
      isTrue_t value = eMenu.argIsTrue(arg);
      if (value == isError) {
        eMenu.v().pln("ERROR: unkown input! [true|false]");
        return;
      }
      if (value == isTrue) {
        eWifi.establish_connection();
      } else if (value == isFalse) {
        if (WiFi.getMode() == WIFI_MODE_AP) {
          WiFi.softAPdisconnect(true);
        } else if (WiFi.getMode() == WIFI_MODE_STA) {
          WiFi.disconnect(true);
        }
      }
    }
  }
};

struct express_wifi::menu_wifi_defaults : menu_item {
  menu_wifi_defaults() : menu_item({(char *)"Default settings"}) {
    this->commands.push_back((char *)"reset");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eWifi.default_settings();
  }
};

struct express_wifi::menu_wifi_local_ip : menu_item {
  menu_wifi_local_ip() : menu_item({(char *)"Set local IP address."}) {
    this->commands.push_back((char *)"l");
    this->commands.push_back((char *)"local");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("local IP: ").pln(eWifi.wifiData.local_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        eMenu.vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        eWifi.wifiData.local_IP = ipv4;
        eWifi.save_settings();

        eMenu.v().p("New local IP: ").pln(eWifi.wifiData.local_IP);
      } else {
        eMenu.v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_gatway_ip : menu_item {
  menu_wifi_gatway_ip() : menu_item({(char *)"Set gateway IP address."}) {
    this->commands.push_back((char *)"g");
    this->commands.push_back((char *)"gateway");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("gateway IP: ").pln(eWifi.wifiData.gateway_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        eMenu.vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        eWifi.wifiData.gateway_IP = ipv4;
        eWifi.save_settings();

        eMenu.v().p("New gateway IP: ").pln(eWifi.wifiData.gateway_IP);
      } else {
        eMenu.v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_subnetmask : menu_item {
  menu_wifi_subnetmask() : menu_item({(char *)"Set subnet mask."}) {
    this->commands.push_back((char *)"subnet");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("Subnet mask: ").pln(eWifi.wifiData.subnet_mask);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        eMenu.vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        eWifi.wifiData.subnet_mask = ipv4;
        eWifi.save_settings();

        eMenu.v().p("New subnet mask: ").pln(eWifi.wifiData.subnet_mask);
      } else {
        eMenu.v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_primaryDNS : menu_item {
  menu_wifi_primaryDNS() : menu_item({(char *)"Set primary DNS address."}) {
    this->commands.push_back((char *)"dns1");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("Primary DNS: ").pln(eWifi.wifiData.primary_DNS);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        eMenu.vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        eWifi.wifiData.primary_DNS = ipv4;
        eWifi.save_settings();

        eMenu.v().p("New primary DNS: ").pln(eWifi.wifiData.primary_DNS);
      } else {
        eMenu.v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_secondaryDNS : menu_item {
  menu_wifi_secondaryDNS() : menu_item({(char *)"Set secondary DNS address."}) {
    this->commands.push_back((char *)"dns2");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("secondary DNS: ").pln(eWifi.wifiData.secondary_DNS);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        eMenu.vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        eWifi.wifiData.secondary_DNS = ipv4;
        eWifi.save_settings();

        eMenu.v().p("New secondary DNS: ").pln(eWifi.wifiData.secondary_DNS);
      } else {
        eMenu.v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_softap_local_ip : menu_item {
  menu_wifi_softap_local_ip() : menu_item({(char *)"Set SoftAP local IP address."}) {
    this->commands.push_back((char *)"sl");
    this->commands.push_back((char *)"slocal");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("SoftAP local IP: ").pln(eWifi.wifiData.softap_local_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        eMenu.vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        eWifi.wifiData.softap_local_IP = ipv4;
        eWifi.save_settings();

        eMenu.v().p("New SoftAP local IP: ").pln(eWifi.wifiData.softap_local_IP);
      } else {
        eMenu.v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_softap_gatway_ip : menu_item {
  menu_wifi_softap_gatway_ip() : menu_item({(char *)"Set SoftAP gateway IP address."}) {
    this->commands.push_back((char *)"sg");
    this->commands.push_back((char *)"sgateway");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      eMenu.v().p("SoftAP gateway IP: ").pln(eWifi.wifiData.softap_gateway_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        eMenu.vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        eWifi.wifiData.softap_gateway_IP = ipv4;
        eWifi.save_settings();

        eMenu.v().p("New SoftAP gateway IP: ").pln(eWifi.wifiData.softap_gateway_IP);
      } else {
        eMenu.v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_sub_wifi : menu_item {
  menu_sub_wifi() : menu_item({(char *)"WiFi settings."}) {
    this->commands.push_back((char *)"wifi");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.MENU_POINTER.clear();
    eMenu.newSubMenu();
    eMenu.MENU_POINTER.push_back(new menu_wifi_info());
    eMenu.MENU_POINTER.push_back(new menu_wifi_ssid());
    eMenu.MENU_POINTER.push_back(new menu_wifi_password());
    eMenu.MENU_POINTER.push_back(new menu_wifi_reconnect());
    eMenu.MENU_POINTER.push_back(new menu_wifi_sta());
    eMenu.MENU_POINTER.push_back(new menu_wifi_static());
    eMenu.MENU_POINTER.push_back(new menu_wifi_local_ip());
    eMenu.MENU_POINTER.push_back(new menu_wifi_gatway_ip());
    eMenu.MENU_POINTER.push_back(new menu_wifi_subnetmask());
    eMenu.MENU_POINTER.push_back(new menu_wifi_primaryDNS());
    eMenu.MENU_POINTER.push_back(new menu_wifi_secondaryDNS());
    eMenu.MENU_POINTER.push_back(new menu_wifi_softap_local_ip());
    eMenu.MENU_POINTER.push_back(new menu_wifi_softap_gatway_ip());
    eMenu.MENU_POINTER.push_back(new menu_wifi_defaults());
  }
};

  #endif

void express_wifi::wifi_event_disconnected() {
  if (connection_retries >= WIFI_CONNECTION_RETRIES) {
    connection_retries = 0;
    eWifi.establish_connection();
  } else {
    connection_retries++;
    eMenu.vvv().p("Connections retrying in: ").pln((WIFI_CONNECTION_RETRIES - connection_retries) + 1);
  }
}

void express_wifi::establish_connection() {
  // eMenu.vvv().pln().p("Connecting to SSID: ").p(wifiData.ssid).pln(" ... ");
  WiFi.disconnect(true);
  delay(100);

  // const uint8_t mac[6] = {0x24, 0x62, 0xab, 0xf9, 0x43, 0x02};
  // esp_wifi_set_mac(WIFI_IF_STA, &mac[0]);

  // WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  // eMenu.vvv().pln().p("Applying Hostname: ").p(wifiData.host_name).p(" ... ").pln(WiFi.setHostname(wifiData.host_name) ? "DONE" : "ERROR!");

  // Station
  if (wifiData.is_station_mode) {
    WiFi.mode(WIFI_STA);
    if (wifiData.is_static_IP) {
      eMenu.vvv().p("Applying STA settings: ").p(wifiData.local_IP).p(" ... ").pln(WiFi.config(wifiData.local_IP, wifiData.gateway_IP, wifiData.subnet_mask, wifiData.primary_DNS, wifiData.secondary_DNS) ? "DONE" : "ERROR!");
    }
    eMenu.vvv().p("Applying modem sleep: ").p(WIFI_MODEM_SLEEP ? "TRUE" : "FALSE").p(" ... ").pln(WiFi.setSleep(WIFI_MODEM_SLEEP) ? "DONE" : "ERROR!");
    eMenu.vvv().p("Applying TX power: ").p(WIFI_MODEM_TX_POWER).p(" ... ").pln(WiFi.setTxPower((wifi_power_t)WIFI_MODEM_TX_POWER) ? "DONE" : "ERROR!");
    eMenu.vvv().p("Applying long range mode: ").p(WIFI_LONGRANG_MODE ? "TRUE" : "FALSE").p(" ... ");
    WiFi.enableLongRange(WIFI_LONGRANG_MODE);
    eMenu.vvv().pln("DONE");
    delay(100);
    WiFi.begin(wifiData.ssid, wifiData.pwd);

    // SoftAP
  } else {
    WiFi.mode(WIFI_AP);
    eMenu.vvv().p("Applying SoftAP settings: ").p(wifiData.local_IP).p(" ... ").pln(WiFi.softAPConfig(wifiData.softap_local_IP, wifiData.softap_gateway_IP, wifiData.softap_subnet_mask) ? "DONE" : "ERROR!");
    eMenu.vvv().p("Applying TX power: ").p(WIFI_MODEM_TX_POWER).p(" ... ").pln(WiFi.setTxPower((wifi_power_t)WIFI_MODEM_TX_POWER) ? "DONE" : "ERROR!");
    eMenu.vvv().p("Applying long range mode: ").p(WIFI_LONGRANG_MODE ? "TRUE" : "FALSE").p(" ... ");
    WiFi.enableLongRange(WIFI_LONGRANG_MODE);
    eMenu.vvv().pln("DONE");
    delay(100);
    WiFi.softAP(wifiData.softap_ssid, wifiData.softap_pwd);
  }
}


void express_wifi::init(WiFiClass *WiFi) {
  _WiFi = WiFi;

  #if (USE_MENU == true)
  eMenu.vvvv().p("wifi init ... ");

  eMenu.MENU_ITEMS.push_back(new menu_sub_wifi());
  eMenu.vvvv().pln("done");
  #endif

  #if (USE_NVS == true)
  esp_err_t err;

  wifiData_t import_data;
  unsigned int sizeOfwifiData = sizeof(import_data);
  err                         = eNvs.get("wifi_data", &import_data, &sizeOfwifiData);

  if (err == ESP_OK) {
    wifiData = import_data;
  } else {
    eMenu.v().p("ERROR->menu_wifi_info: ").pln(err);
    eMenu.vv().pln("ERROR: restoring defaults for wifi settings");
    default_settings();
  }
  #endif

  _WiFi->onEvent(eWifi.wifi_event);

  establish_connection();
}



void express_wifi::default_settings() {
  eMenu.v().pln("INFO: Applying default WiFi settings ...").pln();
  wifiData_t new_settings;

  // Host name
  uint8_t macAddress[6];
  WiFi.macAddress(macAddress);
  sprintf(new_settings.host_name, "%s-%x%x", DEFAULT_WIFI_HOST_NAME, macAddress[4], macAddress[5]);
  sprintf(new_settings.softap_ssid, "%s-%X%X", DEFAULT_WIFI_SOFTAP_SSID, macAddress[4], macAddress[5]);

  wifiData            = new_settings;
  unsigned int length = sizeof(new_settings);
  eNvs.set("wifi_data", &new_settings, length);
  eMenu.v().pln(" DONE ");
}






///////////////////////////////

void express_wifi::print_settings() {
  eMenu.printBox(true);
  //  eMenu.printBox(true);
  eMenu.v().pln("   SETTINGS:").pln();

  eMenu.printInfo("ssid", wifiData.ssid);
  eMenu.printInfo("pwd", eMenu.mask(wifiData.pwd, 2).c_str());
  eMenu.printInfo("host_name", wifiData.host_name);
  // Static STA
  eMenu.v().pln().pln(" --Static STA");

  eMenu.printInfo("local_IP", wifiData.local_IP.toString().c_str());
  eMenu.printInfo("gateway_IP", wifiData.gateway_IP.toString().c_str());
  eMenu.printInfo("subnet_mask", wifiData.subnet_mask.toString().c_str());
  eMenu.printInfo("primary_DNS", wifiData.primary_DNS.toString().c_str());
  eMenu.printInfo("secondary_DNS", wifiData.secondary_DNS.toString().c_str());
  eMenu.printInfo("is_static_IP", wifiData.is_static_IP ? "TRUE" : "FALSE");
  eMenu.printInfo("is_station_mode", wifiData.is_station_mode ? "TRUE" : "FALSE");

  // Soft AP
  eMenu.v().pln().pln(" --Soft AP");

  eMenu.printInfo("softap_ssid", wifiData.softap_ssid);
  eMenu.printInfo("softap_pwd", eMenu.mask(wifiData.softap_pwd, 2).c_str());
  eMenu.printInfo("softap_local_IP", wifiData.softap_local_IP.toString().c_str());
  eMenu.printInfo("softap_gateway_IP", wifiData.softap_gateway_IP.toString().c_str());
  eMenu.printInfo("softap_subnet_mask", wifiData.softap_subnet_mask.toString().c_str());

  // eMenu.printBox(false);
  eMenu.printBox(false);
}

void express_wifi::save_settings() {
  esp_err_t err;
  unsigned int length = sizeof(eWifi.wifiData);
  err                 = eNvs.set("wifi_data", &eWifi.wifiData, length);

  if (err == ESP_OK) {
    eMenu.vvvv().pln("saved settings.");
  } else {
    eMenu.v().pln("ERROR: Failed to save settings!");
  }
}

void express_wifi::scan() {
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();

  free(ssidList);
  ssidList = (char **)calloc(n, sizeof(char *));
  numSSID  = 0;

  for (int i = 0; i < n; i++) {
    boolean found = false;
    for (int j = 0; j < numSSID; j++) {
      if (!strcmp(WiFi.SSID(i).c_str(), ssidList[j]))
        found = true;
    }
    if (!found) {
      ssidList[numSSID] = (char *)calloc(WiFi.SSID(i).length() + 1, sizeof(char));
      sprintf(ssidList[numSSID], "%s", WiFi.SSID(i).c_str());
      numSSID++;
    }
  }
}

#endif
