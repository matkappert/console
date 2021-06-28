/*
    @file       express_wifi.cpp
    @author     matkappert
    @repo       github.com/matkappert/express_wifi
    @version    V2.0.0
    @date       14/11/20
*/

#include "express_wifi.h"
// #include <WiFi.h>

#if (USE_WIFI == true)
  #if (USE_MENU == true)
    #include <esp_wifi.h>

    #include "express_console_menu.h"

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
  express_console_menu::getInstance().vvvv().p("Network event: ").pln(express_wifi::getInstance().system_event_id_cstr[(int)event]);
  if (event == SYSTEM_EVENT_STA_DISCONNECTED || event == SYSTEM_EVENT_AP_STADISCONNECTED) {
    express_console_menu::getInstance().vvvv().p("Reason of disconnection: ").pln(info.disconnected.reason);
    delay(100);
    express_wifi::getInstance().wifi_event_disconnected();
    if (info.disconnected.reason <= 24) {
      express_wifi::getInstance().connection_retries = 0;
      express_wifi::getInstance().establish_connection();
    }
  } else if (event == SYSTEM_EVENT_STA_GOT_IP || event == SYSTEM_EVENT_AP_STA_GOT_IP6 || event == SYSTEM_EVENT_AP_STAIPASSIGNED) {
    express_wifi::getInstance().connection_retries = 0;
    express_console_menu::getInstance().vvvv().pln("Obtained an IP address");
  }
}

struct express_wifi::menu_wifi_ssid : menu_item {
  menu_wifi_ssid() : menu_item({(char *)"Set WiFi SSID."}) {
    this->commands.push_back((char *)"s");
    this->commands.push_back((char *)"ssid");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->printBox(true);
      console->v().pln("   Scanning for Networks...").pln();
      express_wifi::getInstance().scan();
      for (int i = 0; i < express_wifi::getInstance().numSSID; i++) {
        console->v().p("\t").p(i + 1).p(") ").pln(express_wifi::getInstance().ssidList[i]);
      }
      console->printBox(false);

    } else if (length == 1) {
      int selected = atoi(arg);
      console->vvvv().p("selected:").pln(selected);
      if (selected > 0 && selected <= express_wifi::getInstance().numSSID) {
        express_wifi::getInstance().wifiData.ssid[0] = '\0';
        strcpy(express_wifi::getInstance().wifiData.ssid, express_wifi::getInstance().ssidList[selected - 1]);
        console->v().p("New SSID: ").pln(express_wifi::getInstance().wifiData.ssid);
        express_wifi::getInstance().save_settings();
      } else {
        if (express_wifi::getInstance().numSSID == 0) {
          console->v().pln("ERROR: scan for WiFi SSID before picking one.");
        } else {
          console->v().p("ERROR: unkown option, pick a rang from 1-").pln(express_wifi::getInstance().numSSID > 9 ? 9 : express_wifi::getInstance().numSSID);
        }
      }
    } else {
      if (length <= MAX_SSID) {
        express_wifi::getInstance().wifiData.ssid[0] = '\0';
        strcpy(express_wifi::getInstance().wifiData.ssid, arg);

        express_wifi::getInstance().save_settings();

        console->v().p("New SSID: ").pln(express_wifi::getInstance().wifiData.ssid);
      } else {
        console->v().p("ERROR: to long! max=").pln(MAX_SSID);
      }
    }
  }
};

struct express_wifi::menu_wifi_password : menu_item {
  menu_wifi_password() : menu_item({(char *)"Set WiFi password."}) {
    this->commands.push_back((char *)"p");
    this->commands.push_back((char *)"pass");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("Password: ").pln(console->mask(express_wifi::getInstance().wifiData.pwd, 2));
    } else {
      if (length <= MAX_PWD) {
        express_wifi::getInstance().wifiData.pwd[0] = '\0';
        strcpy(express_wifi::getInstance().wifiData.pwd, arg);

        express_wifi::getInstance().save_settings();

        console->v().p("New password: ").pln(console->mask(express_wifi::getInstance().wifiData.pwd, 2));
      } else {
        console->v().p("ERROR: to long! max=").pln(MAX_PWD);
      }
    }
  }
};

struct express_wifi::menu_wifi_sta : menu_item {
  menu_wifi_sta() : menu_item({(char *)"Station or Access point"}) {
    this->commands.push_back((char *)"sta");
    this->commands.push_back((char *)"station");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("STA Mode: ").pln(express_wifi::getInstance().wifiData.is_station_mode ? "TRUE" : "FALSE");
    } else {
      isTrue_t value = express_console_menu::getInstance().argIsTrue(arg);
      if (value == isError) {
        console->v().pln("ERROR: unkown input! [true|false]");
        return;
      } else {
        console->v().p("STA Mode: ").pln(value ? "TRUE" : "FALSE");
        express_wifi::getInstance().wifiData.is_station_mode = value;
        express_wifi::getInstance().save_settings();
      }
    }
  }
};

struct express_wifi::menu_wifi_static : menu_item {
  menu_wifi_static() : menu_item({(char *)"Static IP Address"}) {
    this->commands.push_back((char *)"ip");
    this->commands.push_back((char *)"static");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("Static IP Address: ").pln(express_wifi::getInstance().wifiData.is_static_IP ? "Static" : "Auto");
    } else {
      isTrue_t value = express_console_menu::getInstance().argIsTrue(arg);
      if (value == isError) {
        console->v().pln("ERROR: unkown input! [true|false]");
        return;
      } else {
        console->v().p("Static IP Address: ").pln(value ? "Static" : "Auto");
        express_wifi::getInstance().wifiData.is_static_IP = value;
        express_wifi::getInstance().save_settings();
      }
    }
  }
};

struct express_wifi::menu_wifi_info : menu_item {
  menu_wifi_info() : menu_item({(char *)"WiFi Info."}) {
    this->commands.push_back((char *)"i");
    this->commands.push_back((char *)"info");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    console->printBox(true);
    console->v().pln("   INFO:").pln();

    if (WiFi.getMode() == WIFI_MODE_NULL) {
      console->printInfo("Status", "DISABLED");
    } else {
      console->printInfo("Status", express_wifi::getInstance().wl_status_cstr[(uint8_t)WiFi.status() == 255 ? 7 : (uint8_t)WiFi.status()]);
    }

    if (WiFi.getMode() == WIFI_MODE_STA) {
      console->printInfo("Mode", "STA");
      console->printInfo("SSID", WiFi.SSID().c_str());
      console->printInfo("Password", console->mask(WiFi.psk().c_str(), 2).c_str());
      console->printInfo("RSSI", WiFi.RSSI());
      console->printInfo("IPv4 Address", WiFi.localIP().toString().c_str());
        console->printInfo("IPv6 Address", WiFi.localIPv6().toString().c_str());
      console->printInfo("Subnet Mask", WiFi.subnetMask().toString().c_str());
      console->printInfo("Router", WiFi.gatewayIP().toString().c_str());
      console->v().pln();
      console->printInfo("MAC Address", WiFi.macAddress().c_str());
      console->printInfo("Hostname", WiFi.getHostname());

    } else if (WiFi.getMode() == WIFI_MODE_AP) {
      console->printInfo("Mode", "AP");
      console->printInfo("SSID", WiFi.softAPSSID().c_str());
      console->printInfo("Password", console->mask(express_wifi::getInstance().wifiData.softap_pwd, 2).c_str());
      console->printInfo("IPv4 Address", WiFi.softAPIP().toString().c_str());
        console->printInfo("IPv6 Address", WiFi.softAPIPv6().toString().c_str());
      console->printInfo("MAC Address", WiFi.softAPmacAddress().c_str());
      console->printInfo("Hostname", WiFi.softAPgetHostname());
    }else{
      console->printInfo("Mode", "UNKNOWN");
    }

    console->v().pln();
    console->printInfo("Auto Connect", WiFi.getAutoConnect() ? "TRUE" : "FALSE");
    console->printInfo("Auto Reconnect", WiFi.getAutoReconnect() ? "TRUE" : "FALSE");
    console->printInfo("Modem Sleep", WiFi.getSleep() ? "TRUE" : "FALSE");
    console->printInfo("TX Power", WiFi.getTxPower());

    console->printBox(false);

    express_wifi::getInstance().print_settings();
  }
};

struct express_wifi::menu_wifi_reconnect : menu_item {
  menu_wifi_reconnect() : menu_item({(char *)"Reapply WiFi settings"}) {
    this->commands.push_back((char *)"c");
    this->commands.push_back((char *)"connect");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().pln("Reapply WiFi settings!");
      express_wifi::getInstance().establish_connection();
    } else {
      isTrue_t value = express_console_menu::getInstance().argIsTrue(arg);
      if (value == isError) {
        console->v().pln("ERROR: unkown input! [true|false]");
        return;
      }
      if (value == isTrue) {
        express_wifi::getInstance().establish_connection();
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
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    express_wifi::getInstance().default_settings();
  }
};

struct express_wifi::menu_wifi_local_ip : menu_item {
  menu_wifi_local_ip() : menu_item({(char *)"Set local IP address."}) {
    this->commands.push_back((char *)"l");
    this->commands.push_back((char *)"local");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("local IP: ").pln(express_wifi::getInstance().wifiData.local_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        express_console_menu::getInstance().vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        express_wifi::getInstance().wifiData.local_IP = ipv4;
        express_wifi::getInstance().save_settings();

        console->v().p("New local IP: ").pln(express_wifi::getInstance().wifiData.local_IP);
      } else {
        console->v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_gatway_ip : menu_item {
  menu_wifi_gatway_ip() : menu_item({(char *)"Set gateway IP address."}) {
    this->commands.push_back((char *)"g");
    this->commands.push_back((char *)"gateway");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("gateway IP: ").pln(express_wifi::getInstance().wifiData.gateway_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        express_console_menu::getInstance().vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        express_wifi::getInstance().wifiData.gateway_IP = ipv4;
        express_wifi::getInstance().save_settings();

        console->v().p("New gateway IP: ").pln(express_wifi::getInstance().wifiData.gateway_IP);
      } else {
        console->v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_subnetmask : menu_item {
  menu_wifi_subnetmask() : menu_item({(char *)"Set subnet mask."}) {
    this->commands.push_back((char *)"subnet");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("Subnet mask: ").pln(express_wifi::getInstance().wifiData.subnet_mask);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        express_console_menu::getInstance().vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        express_wifi::getInstance().wifiData.subnet_mask = ipv4;
        express_wifi::getInstance().save_settings();

        console->v().p("New subnet mask: ").pln(express_wifi::getInstance().wifiData.subnet_mask);
      } else {
        console->v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_primaryDNS : menu_item {
  menu_wifi_primaryDNS() : menu_item({(char *)"Set primary DNS address."}) {
    this->commands.push_back((char *)"dns1");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("Primary DNS: ").pln(express_wifi::getInstance().wifiData.primary_DNS);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        express_console_menu::getInstance().vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        express_wifi::getInstance().wifiData.primary_DNS = ipv4;
        express_wifi::getInstance().save_settings();

        console->v().p("New primary DNS: ").pln(express_wifi::getInstance().wifiData.primary_DNS);
      } else {
        console->v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_secondaryDNS : menu_item {
  menu_wifi_secondaryDNS() : menu_item({(char *)"Set secondary DNS address."}) {
    this->commands.push_back((char *)"dns2");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("secondary DNS: ").pln(express_wifi::getInstance().wifiData.secondary_DNS);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        express_console_menu::getInstance().vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        express_wifi::getInstance().wifiData.secondary_DNS = ipv4;
        express_wifi::getInstance().save_settings();

        console->v().p("New secondary DNS: ").pln(express_wifi::getInstance().wifiData.secondary_DNS);
      } else {
        console->v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_softap_local_ip : menu_item {
  menu_wifi_softap_local_ip() : menu_item({(char *)"Set SoftAP local IP address."}) {
    this->commands.push_back((char *)"sl");
    this->commands.push_back((char *)"slocal");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("SoftAP local IP: ").pln(express_wifi::getInstance().wifiData.softap_local_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        express_console_menu::getInstance().vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        express_wifi::getInstance().wifiData.softap_local_IP = ipv4;
        express_wifi::getInstance().save_settings();

        console->v().p("New SoftAP local IP: ").pln(express_wifi::getInstance().wifiData.softap_local_IP);
      } else {
        console->v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_wifi_softap_gatway_ip : menu_item {
  menu_wifi_softap_gatway_ip() : menu_item({(char *)"Set SoftAP gateway IP address."}) {
    this->commands.push_back((char *)"sg");
    this->commands.push_back((char *)"sgateway");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length == 0) {
      console->v().p("SoftAP gateway IP: ").pln(express_wifi::getInstance().wifiData.softap_gateway_IP);
    } else {
      if (length >= 7 || length <= 15) {  // 0.0.0.0 to 000.000.000.000
        IPAddress ipv4;
        bool err = !ipv4.fromString(arg);
        express_console_menu::getInstance().vvv().p("Importing IP address: ").p(arg).p(" ... ").pln(err ? "ERROR" : "DONE");
        if (err) {
          return;
        }
        express_wifi::getInstance().wifiData.softap_gateway_IP = ipv4;
        express_wifi::getInstance().save_settings();

        console->v().p("New SoftAP gateway IP: ").pln(express_wifi::getInstance().wifiData.softap_gateway_IP);
      } else {
        console->v().pln("ERROR: invalid input!");
      }
    }
  }
};

struct express_wifi::menu_sub_wifi : menu_item {
  menu_sub_wifi() : menu_item({(char *)"WiFi settings."}) {
    this->commands.push_back((char *)"wifi");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    console->MENU_POINTER.clear();
    console->newSubMenu();
    console->MENU_POINTER.push_back(express_wifi::getInstance()._menu_wifi_info);
    console->MENU_POINTER.push_back(express_wifi::getInstance()._menu_wifi_ssid);
    console->MENU_POINTER.push_back(express_wifi::getInstance()._menu_wifi_password);
    console->MENU_POINTER.push_back(express_wifi::getInstance()._menu_wifi_reconnect);
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_sta());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_static());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_local_ip());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_gatway_ip());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_subnetmask());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_primaryDNS());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_secondaryDNS());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_softap_local_ip());
    console->MENU_POINTER.push_back(new express_wifi::menu_wifi_softap_gatway_ip());
    console->MENU_POINTER.push_back(express_wifi::getInstance()._menu_wifi_defaults);
  }
};

  #endif

void express_wifi::wifi_event_disconnected() {
  if (connection_retries >= WIFI_CONNECTION_RETRIES) {
    connection_retries = 0;
    express_wifi::getInstance().establish_connection();
  } else {
    connection_retries++;
    express_console_menu::getInstance().vvv().p("Connections retrying in: ").pln((WIFI_CONNECTION_RETRIES - connection_retries) + 1);
  }
}

void express_wifi::establish_connection() {
  // express_console_menu::getInstance().vvv().pln().p("Connecting to SSID: ").p(wifiData.ssid).pln(" ... ");
  WiFi.disconnect(true);
  delay(100);

  // const uint8_t mac[6] = {0x24, 0x62, 0xab, 0xf9, 0x43, 0x02};
  // esp_wifi_set_mac(WIFI_IF_STA, &mac[0]);

  // WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  // express_console_menu::getInstance().vvv().pln().p("Applying Hostname: ").p(wifiData.host_name).p(" ... ").pln(WiFi.setHostname(wifiData.host_name) ? "DONE" : "ERROR!");

  // Station
  if (wifiData.is_station_mode) {
    WiFi.mode(WIFI_STA);
    if (wifiData.is_static_IP) {
      express_console_menu::getInstance().vvv().p("Applying STA settings: ").p(wifiData.local_IP).p(" ... ").pln(WiFi.config(wifiData.local_IP, wifiData.gateway_IP, wifiData.subnet_mask, wifiData.primary_DNS, wifiData.secondary_DNS) ? "DONE" : "ERROR!");
    }
    express_console_menu::getInstance().vvv().p("Applying modem sleep: ").p(WIFI_MODEM_SLEEP ? "TRUE" : "FALSE").p(" ... ").pln(WiFi.setSleep(WIFI_MODEM_SLEEP) ? "DONE" : "ERROR!");
    express_console_menu::getInstance().vvv().p("Applying TX power: ").p(WIFI_MODEM_TX_POWER).p(" ... ").pln(WiFi.setTxPower((wifi_power_t)WIFI_MODEM_TX_POWER) ? "DONE" : "ERROR!");
    express_console_menu::getInstance().vvv().p("Applying long range mode: ").p(WIFI_LONGRANG_MODE ? "TRUE" : "FALSE").p(" ... ");
    WiFi.enableLongRange(WIFI_LONGRANG_MODE);
    express_console_menu::getInstance().vvv().pln("DONE");
    delay(100);
    WiFi.begin(wifiData.ssid, wifiData.pwd);

    // SoftAP
  } else {
    WiFi.mode(WIFI_AP);
    express_console_menu::getInstance().vvv().p("Applying SoftAP settings: ").p(wifiData.local_IP).p(" ... ").pln(WiFi.softAPConfig(wifiData.softap_local_IP, wifiData.softap_gateway_IP, wifiData.softap_subnet_mask) ? "DONE" : "ERROR!");
    express_console_menu::getInstance().vvv().p("Applying TX power: ").p(WIFI_MODEM_TX_POWER).p(" ... ").pln(WiFi.setTxPower((wifi_power_t)WIFI_MODEM_TX_POWER) ? "DONE" : "ERROR!");
    express_console_menu::getInstance().vvv().p("Applying long range mode: ").p(WIFI_LONGRANG_MODE ? "TRUE" : "FALSE").p(" ... ");
    WiFi.enableLongRange(WIFI_LONGRANG_MODE);
    express_console_menu::getInstance().vvv().pln("DONE");
    delay(100);
    WiFi.softAP(wifiData.softap_ssid, wifiData.softap_pwd);
  }
}


void express_wifi::init(WiFiClass *WiFi) {
  _WiFi = WiFi;

  #if (USE_MENU == true)
  express_console_menu::getInstance().vvvv().p("wifi init ... ");
  _menu_sub_wifi       = new menu_sub_wifi();
  _menu_wifi_ssid      = new menu_wifi_ssid();
  _menu_wifi_password  = new menu_wifi_password();
  _menu_wifi_info      = new menu_wifi_info();
  _menu_wifi_reconnect = new menu_wifi_reconnect();
  _menu_wifi_defaults  = new menu_wifi_defaults();

  express_console_menu::getInstance().MENU_ITEMS.push_back(_menu_sub_wifi);
  express_console_menu::getInstance().vvvv().pln("done");
  #endif

  #if (USE_NVS == true)
  esp_err_t err;

  wifiData_t import_data;
  unsigned int sizeOfwifiData = sizeof(import_data);
  err                         = express_nvs::getInstance().get("wifi_data", &import_data, &sizeOfwifiData);

  if (err == ESP_OK) {
    wifiData = import_data;
  } else {
    express_console_menu::getInstance().v().p("ERROR->menu_wifi_info: ").pln(err);
    express_console_menu::getInstance().vv().pln("ERROR: restoring defaults for wifi settings");
    default_settings();
  }
  #endif

  _WiFi->onEvent(express_wifi::getInstance().wifi_event);

  establish_connection();
}

void express_wifi::update() {}







void express_wifi::default_settings() {
  express_console_menu::getInstance().v().pln("INFO: Applying default WiFi settings ...").pln();
  wifiData_t new_settings;

  // Host name
  // uint8_t macAddress[6];
  // WiFi.macAddress(macAddress);
  // sprintf(new_settings.host_name, "%s-%x%x", DEFAULT_WIFI_HOST_NAME, macAddress[4], macAddress[5]);
  // sprintf(new_settings.softap_ssid, "%s-%X%X", DEFAULT_WIFI_SOFTAP_SSID, macAddress[4], macAddress[5]);

  wifiData            = new_settings;
  unsigned int length = sizeof(new_settings);
  express_nvs::getInstance().set("wifi_data", &new_settings, length);
  express_console_menu::getInstance().v().pln(" DONE ");
}






///////////////////////////////

void express_wifi::print_settings() {
  express_console_menu::getInstance().printBox(true);
  express_console_menu::getInstance().v().pln("   SETTINGS:").pln();

  express_console_menu::getInstance().printInfo("ssid", wifiData.ssid);
  express_console_menu::getInstance().printInfo("pwd", express_console_menu::getInstance().mask(wifiData.pwd, 2).c_str());
  express_console_menu::getInstance().printInfo("host_name", wifiData.host_name);
  // Static STA
  express_console_menu::getInstance().v().pln().pln(" --Static STA");

  express_console_menu::getInstance().printInfo("local_IP", wifiData.local_IP.toString().c_str());
  express_console_menu::getInstance().printInfo("gateway_IP", wifiData.gateway_IP.toString().c_str());
  express_console_menu::getInstance().printInfo("subnet_mask", wifiData.subnet_mask.toString().c_str());
  express_console_menu::getInstance().printInfo("primary_DNS", wifiData.primary_DNS.toString().c_str());
  express_console_menu::getInstance().printInfo("secondary_DNS", wifiData.secondary_DNS.toString().c_str());
  express_console_menu::getInstance().printInfo("is_static_IP", wifiData.is_static_IP ? "TRUE" : "FALSE");
  express_console_menu::getInstance().printInfo("is_station_mode", wifiData.is_station_mode ? "TRUE" : "FALSE");

  // Soft AP
  express_console_menu::getInstance().v().pln().pln(" --Soft AP");

  express_console_menu::getInstance().printInfo("softap_ssid", wifiData.softap_ssid);
  express_console_menu::getInstance().printInfo("softap_pwd", express_console_menu::getInstance().mask(wifiData.softap_pwd, 2).c_str());
  express_console_menu::getInstance().printInfo("softap_local_IP", wifiData.softap_local_IP.toString().c_str());
  express_console_menu::getInstance().printInfo("softap_gateway_IP", wifiData.softap_gateway_IP.toString().c_str());
  express_console_menu::getInstance().printInfo("softap_subnet_mask", wifiData.softap_subnet_mask.toString().c_str());

  express_console_menu::getInstance().printBox(false);
}

void express_wifi::save_settings() {
  esp_err_t err;
  unsigned int length = sizeof(express_wifi::getInstance().wifiData);
  err                 = express_nvs::getInstance().set("wifi_data", &express_wifi::getInstance().wifiData, length);

  if (err == ESP_OK) {
    express_console_menu::getInstance().vvvv().pln("saved settings.");
  } else {
    express_console_menu::getInstance().v().pln("ERROR: Failed to save settings!");
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
