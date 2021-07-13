/*
    @file       express_console_menu.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       26/09/20
*/
#define EXPRESS_CONSOLE_MENU_VER "2.3.1"
#pragma once

#include <Arduino.h>
#include "settings.h"
/*
 * Forward-Declarations
 */
struct express_console_menu;
extern express_console_menu eMenu;




// #include <string>
// #include <unordered_map>
// using namespace std;
// using std::unordered_map;
#include <vector>
using std::vector;

// #include "EEPROM.h"
#include "express_console.h"
#define __EXPRESS_CONSOLE_MENU_LEVEL_INDEX 0

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

class express_console_menu;

struct menu_item {
  vector<char *> commands;
  char *help;  // e.g. 'Reboot ESP'
  boolean hidden = false;

  menu_item(char *help) {
    this->help = help;
  }
  virtual void callback(const char *cmd, const char *arg, const uint8_t length) {}
};

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
#include "express_nvs.h"

class express_console_menu : public express_console {

 private:
  uint8_t verbosity_level = 0;
  void nvs_init();

 public:
  size_t _bufferLen;
  char _buffer[CONSOLE_BUFFER_SIZE];
  Version version                       = {0, 0, 0};
  vector<menu_item *> MENU_ITEMS;
  vector<menu_item *> MENU_POINTER;
#if (USE_NVS == true)
  uint32_t reboot_counter            = 0;
  uint32_t reboot_counter_resettable = 0;
#endif
  // express_nvs nvs = express_nvs();

  void init(Print &printer);

  void default_verbosity_level() {
    // setLevel(3);
    verbosity_level                                   = DEFAULT_VERBOSITY_LEVEL;
    eMenu._filter_level = (express_console::Level)verbosity_level;
    eNvs.set("verbosity_level", &verbosity_level);
  }

  void default_reboot_counter() {
    reboot_counter_resettable = 0;
    eNvs.set("reboot_cnt_rst", &reboot_counter_resettable);
  }

  void update();

  void setLevel(uint8_t level);

  void printBox(bool isStart) {
    v().p(isStart ? "\n┌" : "└");
    for (uint8_t i = 0; i < 113; ++i) {
      v().p("─");
    }
    v().pln(isStart ? "┐" : "┘");
  };

  void printInfoKey(const char *key) {
    for (uint8_t padding = 0; padding < MENU_PADDING; padding++) {
      v().p(' ');
    }
    v().p(key).p(':');
    for (uint8_t tab = (MENU_PADDING + strlen(key)); tab < MENU_OFFSET; tab++) {
      v().p(' ');
    }
  }

  void printInfo(const char *key, const char *value) {
    printInfoKey(key);
    v().pln(value);
  }

  template <typename Type>
  void printInfo(const char *key, Type value) {
    printInfoKey(key);
    v().pln(value);
  }

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
  void newSubMenu();
  void newSubMenu(vector<menu_item *> sub);

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
  // express_console_menu *_self;

  // struct info;
  struct verbose;
  struct reboot;
  struct reset;
  struct factory_reset;
  struct halt;
  struct exit_sub;

  struct verbose;
  struct information;

};
