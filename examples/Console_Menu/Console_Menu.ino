#include <Arduino.h>
#define USE_MENU  true
#define USE_NVS   true
#define USE_WIFI  false
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

vector<MENU_STRUCT *> MENU;  // main menu

struct menu_MAIN_t : MENU_STRUCT {
  menu_MAIN_t() : MENU_STRUCT({(char *)"MAIN MENU ITEM"}) {
    this->commands.push_back((char *)"1");
    this->commands.push_back((char *)"MAIN");
    eMenu.MENU_MAIN_VECTOR.push_back(this);  // add to main menu vector
  }
  void callback(const char *cmd = nullptr, const char *arg = nullptr, const uint8_t length = 0) override {
    eMenu.enterSubMenu(MENU);  // enter into our sub menu
  }
} menu_main;

struct menu_SUB_t : MENU_STRUCT {
  menu_SUB_t() : MENU_STRUCT({(char *)"SUB MENU ITEM"}) {
    this->commands.push_back((char *)"2");
    this->commands.push_back((char *)"SUB");
    MENU.push_back(this);  // add to our sub menu vector
  }
  void callback(const char *cmd = nullptr, const char *arg = nullptr, const uint8_t length = 0) override {}
} menu_sub;

void setup() {
  Serial.begin(BAUDRATE);
  delay(100);
  eMenu.init(Serial);
  eMenu.version = {1, 2, 3};
  
}

void loop() {
  eMenu.update();
}