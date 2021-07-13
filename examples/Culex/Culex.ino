#include "Settings.h"
#include "express_console_menu.h"
#if (USE_WIFI == true)
  #include <WiFi.h>
#endif

// struct trans_control_reboot_t : CULEX_TRANSPORT {
//   trans_control_reboot_t() : CULEX_TRANSPORT({(char *)"Node Control/Reboot", (EXPRESS_TYPE_ENUM)Boolean}) {}
//   void callback() override {
//     eMenu.v().pln("trans_bdSeq->callback()");
//   }
// } trans_control_reboot;

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
      // trans_bdSeq.value.UInt8++;
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

  EXPRESS_TYPE_UNION value;
  char cbuf[13];
  value.Int8 = 127;
  eMenu.v().p("Int8:\t\t").p(eUtil.valueToBuffer(Int8, &value)).pln("\t\t127");
  value.Int8++;
  eMenu.v().p("Int8:\t\t").p(eUtil.valueToBuffer(Int8, &value)).pln("\t\t-128");

  value.Int16 = 32767;
  eMenu.v().p("Int16:\t\t").p(eUtil.valueToBuffer(Int16, &value)).pln("\t\t32767");
  value.Int16++;
  eMenu.v().p("Int16:\t\t").p(eUtil.valueToBuffer(Int16, &value)).pln("\t\t-32768");

  value.Int32 = 2147483647;
  eMenu.v().p("Int32:\t\t").p(eUtil.valueToBuffer(Int32, &value)).pln("\t2147483647");
  value.Int32++;
  eMenu.v().p("Int32:\t\t").p(eUtil.valueToBuffer(Int32, &value)).pln("\t-2147483648");

  // value.Int64 = 9223372036854775800;
  // eMenu.v().p("Int64:\t\t").p(eUtil.valueToBuffer(Int64, &value)).pln("\t9223372036854775807");
  // value.Int64++;
  // eMenu.v().p("Int64:\t\t").p(eUtil.valueToBuffer(Int64, &value)).pln("\t-9223372036854775808");

  value.Int8 = 255;
  eMenu.v().p("UInt8:\t\t").p(eUtil.valueToBuffer(UInt8, &value)).pln("\t\t255");
  value.UInt8++;
  eMenu.v().p("UInt8:\t\t").p(eUtil.valueToBuffer(UInt8, &value)).pln("\t\t0");

  value.UInt16 = 65535;
  eMenu.v().p("UInt16:\t\t").p(eUtil.valueToBuffer(UInt16, &value)).pln("\t\t65535");
  value.UInt16++;
  eMenu.v().p("UInt16:\t\t").p(eUtil.valueToBuffer(UInt16, &value)).pln("\t\t0");

  value.UInt32 = 4294967295;
  eMenu.v().p("UInt32:\t\t").p(eUtil.valueToBuffer(UInt32, &value)).pln("\t4294967295");
  value.UInt32++;
  eMenu.v().p("UInt32:\t\t").p(eUtil.valueToBuffer(UInt32, &value)).pln("\t\t0");

  value.Float = PI;
  eMenu.v().p("Float:\t\t").p(eUtil.valueToBuffer(Float, &value)).pln("\t3.141593");
  value.Float = value.Float*2;
  eMenu.v().p("Float:\t\t").p(eUtil.valueToBuffer(Float, &value)).pln("\t6.283185");

  value.Double = PI;
  eMenu.v().p("Double:\t\t").p(eUtil.valueToBuffer(Double, &value)).pln("\t3.141593");
  value.Double = value.Double*2;
  eMenu.v().p("Double:\t\t").p(eUtil.valueToBuffer(Double, &value)).pln("\t6.283185");

  value.Boolean = false;
  eMenu.v().p("Boolean:\t").p(eUtil.valueToBuffer(Boolean, &value)).pln("\t\tFALSE");
  value.Boolean = true;
  eMenu.v().p("Boolean:\t").p(eUtil.valueToBuffer(Boolean, &value)).pln("\t\tTRUE");

  // EXPRESS_TYPE_UNION *value;
  //  eCulex.objectValue(&value, UInt8_t, x);
}

void loop() {
  eCulex.update();
  eMenu.update();
}