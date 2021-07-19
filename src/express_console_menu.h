/**
 * @file express_console_menu.h
 * @author Mathew Kappert
 * @brief 
 * @version 0.1
 * @date 26-09-2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#define EXPRESS_CONSOLE_MENU_VER "0.1"
#pragma once

#include <Arduino.h>

#include "settings.h"
/*
 * Forward-Declarations
 */
struct express_console_menu;
extern express_console_menu eMenu;
struct MENU_STRUCT;

// #include <string>
// #include <unordered_map>
// using namespace std;
// using std::unordered_map;
#include <vector>
using std::vector;

// #include "EEPROM.h"
#include "express_console.h"
#define __EXPRESS_CONSOLE_MENU_LEVEL_INDEX 0

class express_console_menu;

#define CONSOLE_BUFFER_SIZE 100
typedef void (*cmd_action_t)(const char *, const char *, uint8_t);
typedef struct {
  const char *shortcut;
  const char *command;
  cmd_action_t action;
  const char *description;
} cmd_t;

typedef struct {
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
} Version;

typedef enum {
  isError = -1,
  isFalse = 0,
  isTrue  = 1,
} isTrue_t;

#if (USE_WIFI == true)
  #include "express_wifi.h"
#endif
#if (USE_CULEX == true)
  #include "express_culex.h"
#endif
#if (USE_PLOT == true)
  #include "express_plot.h"
#endif
#ifndef USE_NVS
  #define USE_NVS true
#endif
#if (USE_LED == true)
#include "express_status_led.h"
#endif
#include "express_nvs.h"
#include "express_utilities.h"
class express_console_menu : public express_console {

 private:
  uint8_t verbosity_level = 0;
  void nvs_init();

 public:
  size_t _bufferLen;
  char _buffer[CONSOLE_BUFFER_SIZE];
  Version version = {0, 0, 0};
  boolean hasFinishedInit = false;

  vector<MENU_STRUCT *> MENU_BUFFER_VECTOR;  // current menu
  vector<MENU_STRUCT *> MENU_MAIN_VECTOR;    // main menu
  vector<MENU_STRUCT *> MENU_GLOBAL_VECTOR;  // system menu
  vector<MENU_STRUCT *> MENU_SYSTEM_VECTOR;  // system sub menu

#if (USE_NVS == true)
  uint32_t reboot_counter            = 0;
  uint32_t reboot_counter_resettable = 0;
#endif
  // express_nvs nvs = express_nvs();

  void init(Print &printer);

  void default_verbosity_level() {
    // setLevel(3);
    verbosity_level     = DEFAULT_VERBOSITY_LEVEL;
    eMenu._filter_level = (express_console::Level)verbosity_level;
    eNvs.set("verbosity_level", &verbosity_level);
  }
  // TODO move to util
  void default_reboot_counter() {
    reboot_counter_resettable = 0;
    eNvs.set("reboot_cnt_rst", &reboot_counter_resettable);
  }

  void update();

  // TODO do i need this? i think not
  void setLevel(uint8_t level);

  // ┌───────────────────────────┬──────────────┐ TABLE_HEADER_START
  // │    (index)                │  value       │ TABLE_DATA
  // ├───────────────────────────┼──────────────┤ TABLE_HEADER_END
  // │                           │              │ TABLE_ROW
  // │    (index)                │  value       │ TABLE_DATA
  // │    (index)                │  value       │ TABLE_DATA
  // └───────────────────────────┴──────────────┘ TABLE_END

  enum tableState {
    TABLE_HEADER_START,
    TABLE_HEADER_END,
    TABLE_ROW,
    TABLE_DATA,
    TABLE_END,
  };
  char tableBuffer[MENU_TABLE_WIDTH - (MENU_OFFSET + MENU_PADDING)];
  void printTable(tableState state, const char *index = "", const char *value = "") {
    if (state == TABLE_HEADER_START) {
      v().p("\n\n┌");
      for (uint8_t i = 1; i < MENU_TABLE_WIDTH; ++i) {
        v().p(i == MENU_OFFSET - 2 ? "┬" : "─");
      }
      v().p("┐");
    } else if (state == TABLE_DATA) {
      v().p("│");
      for (uint8_t padding = 0; padding < MENU_PADDING; padding++) {
        v().p(" ");
      }
      v().p(index);
      for (uint8_t tab = (MENU_PADDING + strlen(index)); tab < MENU_OFFSET; tab++) {
        v().p(tab == MENU_OFFSET - 3 ? "│" : " ");
      }
      v().p(value);
      for (uint8_t end = (MENU_OFFSET + MENU_PADDING + strlen(value)); end < MENU_TABLE_WIDTH + 3; end++) {
        v().p(" ");
      }
      v().p("│");
    } else if (state == TABLE_HEADER_END) {
      v().p("├");
      for (uint8_t i = 1; i < MENU_TABLE_WIDTH; ++i) {
        v().p(i == MENU_OFFSET - 2 ? "┼" : "─");
      }
      v().p("┤");
    } else if (state == TABLE_ROW) {
      v().p("│");
      for (uint8_t i = 1; i < MENU_TABLE_WIDTH; ++i) {
        v().p(i == MENU_OFFSET - 2 ? "│" : " ");
      }
      v().p("│");
    } else if (state == TABLE_END) {
      v().p("└");
      for (uint8_t i = 1; i < MENU_TABLE_WIDTH; ++i) {
        v().p(i == MENU_OFFSET - 2 ? "┴" : "─");
      }
      v().p("┘");
    }

    v().pln("");
  }

  

  // TODO move to util
  // masks a string with asterisks (good for displaying passwords)
  String mask(const char *c, int n) {
    String s = "";
    int len  = strlen(c);
    for (int i = 0; i < len; i++) {
      if (i < n || i >= len - n)
        s += c[i];
      else
        s += '*';
    }
    return (s);
  }

 private:
  Stream *_stream;
  // cmd_t *_addedCommands[9];

  const cmd_t *_commands;

  void processCommand(char *_ptr);
  void processFunction(const char *cmd, const char *arg, const uint8_t length);

 public:
  void enterSubMenu(vector<MENU_STRUCT *> sub, boolean insertExitCallback = true);

  isTrue_t argIsTrue(const char *arg) {
    if (strlen(arg) <= 5) {
      char temp[6];
      strcpy(temp, arg);
      strlwr(temp);
      if (!strcmp(temp, "true") || !strcmp(temp, "1") || !strcmp(temp, "yes")) {
        return isTrue;
      } else if (!strcmp(temp, "false") || !strcmp(temp, "0") || !strcmp(temp, "no")) {
        return isFalse;
      } else {
        return isError;
      }
    } else {
      return isError;
    }
  }

  struct menu_information_t;
  struct menu_verbose_t;
  struct menu_reboot_t;
  struct menu_reset_t;
  struct menu_factoryReset_t;
  // struct menu_halt_t;
  struct menu_exitSubMenu_t;
  struct menu_help_t;
};

struct MENU_STRUCT {
  vector<char *> commands;
  char *help;
  boolean hidden;

  MENU_STRUCT(char *help, boolean hidden = false) {
    this->help   = help;
    this->hidden = hidden;
    // eMenu.MENU_STRUCT_VECTOR.push_back(this);
  }
  virtual void callback(const char *cmd = nullptr, const char *arg = nullptr, const uint8_t length = 0) {}
};
