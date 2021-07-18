#include <Arduino.h>

#include "Settings.h"
#include "express_console_menu.h"

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