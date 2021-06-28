#include <Arduino.h>
// #include <WiFi.h>

#include "Configuration.h"

#if (USE_WIFI == true)
  #include <WiFi.h>
#endif
// extern const int A = 1;
// extern const int A = 1;

// #include <unordered_map>

#include "express_status_led.h"
express_status_led status = express_status_led();
void statusCallback() {
  status.callback();
};

#include "express_console_menu.h"
  // express_console_menu console = express_console_menu();

#include "express_nvs.h"
  // express_nvs nvs = express_nvs();
  // extern const int A           = 1;
  // express_console_menu console = express_console_menu();
  // express_nvs nvs = express_nvs();
  // express_nvs nvs = nvs;
  // on="" delay="2000" blink="12" delay="2000" reboot=""

// #include "esp_event_legacy.h"


void setup() {
  Serial.begin(BAUDRATE);
  delay(100);

  // init(Print &printer, const cmd_t* commands, size_t num, bool _prompt =
  // true, bool _eeprom = true, express_console_menu &self)
  // express_nvs::getInstance().init();

  //   console.init(console, Serial, nullptr, &nvs, 0, /*PROMPT*/ true);
  // console.init(console, Serial, nullptr, 0, /*PROMPT*/ true);
  express_console_menu::getInstance().init(Serial, nullptr, 0, /*PROMPT*/ true);

  express_console_menu::getInstance().version = {1, 2, 3};
  // console.setFilter(Level::vvvv);

  // status.init(21, false, statusCallback);

#if (USE_WIFI == true)
  express_wifi::getInstance().init(&WiFi);
#endif

  // status.blink(4);

  // status.addTask( 0, TASK::BLINK, 1 );
  // status.addTask( 1, TASK::BLINK, 4 );
  // status.addTask( 2, TASK::BLINK, 8 );
  // status.addTask( 3, TASK::BLINK, 16 );

  // delay(5000);
  // status.removeTask( 0 );

  // delay(5000);
  // status.removeTask( 1 );

  // delay(5000);
  // status.removeTask( 2 );

  // delay(5000);
  // status.addTask( 0, TASK::BLINK, 1 );
  // delay(5000);
  // status.removeTask( 0 );

  // status.removeTask( 3 );

  // status.removeAllTasks();
  express_console_menu::getInstance().MENU_POINTER.assign(express_console_menu::getInstance().MENU_ITEMS.begin(), express_console_menu::getInstance().MENU_ITEMS.end());
}

void loop() {
  express_console_menu::getInstance().update(/*ECHO*/ true);
}