#include <Arduino.h>
#define USE_MENU  true
#define USE_NVS   true
#define USE_WIFI  true
#define USE_CULEX false

#include "Settings.h"
#if (USE_MENU == true)
  #include "express_console_menu.h"
#else
  #include "express_console.h"
#endif
#if (USE_WIFI == true)
  #include "express_wifi.h"
#endif
#if (USE_CULEX == true)
  #include "express_culex.h"
#endif
#if (USE_PLOT == true)
  #include "express_plot.h"
#endif
#if (USE_LED == true)
  #include "express_status_led.h"
#endif




void setup() {
  Serial.begin(BAUDRATE);
  delay(100);
  eMenu.init(Serial);
  eMenu.version = {1, 2, 3};

  eWifi.init(&WiFi);
}

void loop() {

  eMenu.update();
}