#include "Settings.h"
#include "express_console_menu.h"
#if (USE_WIFI == true)
  #include <WiFi.h>
#endif



struct trans_bdSeq_t : CULEX_TRANSPORT {
  trans_bdSeq_t() : CULEX_TRANSPORT({(char *)"bdSeq", (EXPRESS_TYPE_ENUM)UInt8}) {
    this->POST = true; // TODO do this in the class
  }
  void callback() override {
    eMenu.v().pln("trans_bdSeq->callback()");
  }
} trans_bdSeq;

struct trans_control_reboot_t : CULEX_TRANSPORT {
  trans_control_reboot_t() : CULEX_TRANSPORT({(char *)"Node Control/Reboot", (EXPRESS_TYPE_ENUM)Boolean}) {}
  void callback() override {
    eMenu.v().pln("trans_bdSeq->callback()");
  }
} trans_control_reboot;



struct connect : menu_item {
  connect() : menu_item({(char *)"Connect or Disconnect to Culex server"}) {
    this->commands.push_back((char *)"c");
    this->commands.push_back((char *)"connect");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    isTrue_t value = eMenu.argIsTrue(arg);
    if (value == isError) {
      eMenu.v().pln("ERROR: unkown input! [true|false]");
      return;
    } else if (value == isTrue) {
      eCulex.connect();
      trans_bdSeq.value.UInt8++;
    } else {
      eCulex.disconnect();
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(100);
  eMenu.init(Serial);

  eWifi.init(&WiFi);
  eCulex.init(&WiFi);

  eMenu.MENU_ITEMS.push_back(new connect());
  eMenu.MENU_POINTER.assign(eMenu.MENU_ITEMS.begin(), eMenu.MENU_ITEMS.end());


  eCulex.CULEX_TRANSPORT_VECTORS.push_back(&trans_bdSeq);
  eCulex.CULEX_TRANSPORT_VECTORS.push_back(&trans_control_reboot);
}

void loop() {
  eCulex.update();
  eMenu.update();
}