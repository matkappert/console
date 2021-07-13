#include "Settings.h"
#include "express_console_menu.h"
#if (USE_WIFI == true)
  #include <WiFi.h>
#endif


struct menu_connect_t : menu_item {
  menu_connect_t() : menu_item({(char *)"Connect or Disconnect to Culex server"}) {
    this->commands.push_back((char *)"c");
    this->commands.push_back((char *)"connect");
    eMenu.MENU_ITEMS.push_back(this); // TODO do the globally in parent struct
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
}  menu_connect;

// struct menu_speedTest_t;

// replace speedTest
#define SPEED_TEST_COUNT (uint8_t)10
uint32_t speed_start[SPEED_TEST_COUNT];
uint32_t speed_end[SPEED_TEST_COUNT];
uint8_t speed_counter;

struct trans_speedTest_t : CULEX_TRANSPORT {
  trans_speedTest_t() : CULEX_TRANSPORT({(char *)"speedTest", (EXPRESS_TYPE_ENUM)Int32}) {}
  void callback(JsonObject object, const char *type) override {
    eCulex.typeToValue(&this->value, type, object, (EXPRESS_TYPE_ENUM)Int32);
    eMenu.info(__func__).p("value:").pln(this->value.Int32);
    speed_end[speed_counter] = millis();
    // eMenu.info(__func__).p("time diff:").pln(menu_speedTest.start - menu_speedTest.end);
  }
} trans_speedTest;

struct menu_speedTest_t : menu_item {
  menu_speedTest_t() : menu_item({(char *)"run a Culex speed test"}) {
    this->commands.push_back((char *)"speed");
    this->commands.push_back((char *)"speedTest");
    eMenu.MENU_ITEMS.push_back(this); // TODO do the globally in parent struct
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    speed_start[speed_counter] = millis();
    // data to send;
    trans_speedTest.publish();
  }
}  menu_speedTest;




void setup() {
  Serial.begin(115200);
  delay(100);
  eMenu.init(Serial);

  eWifi.init(&WiFi);
  eCulex.init(&WiFi);

  // eMenu.MENU_ITEMS.push_back(new menu_connect());
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