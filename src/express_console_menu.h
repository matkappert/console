#ifndef __EXPRESS_CONSOLE_MENU_H
#define __EXPRESS_CONSOLE_MENU_H
/*
    @file       express_console_menu.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       26/09/20
*/
#define EXPRESS_CONSOLE_MENU_VER "2.2.0"

#include <Arduino.h>

#include "Configuration.h"

#ifndef USE_NVS
  #define USE_NVS true
#endif

#include "express_nvs.h"
// #include <string>
// #include <unordered_map>
// using namespace std;
// using std::unordered_map;
#include <vector>
using std::vector;

// #include "EEPROM.h"
#include "express_console.h"
#define __EXPRESS_CONSOLE_MENU_LEVEL_INDEX 0
#include "./CRCx/CRCx.h"

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
  express_console_menu *console;

  menu_item(char *help) {
    this->help = help;
  }
  virtual void callback(const char *cmd, const char *arg, const uint8_t length) {}
  virtual void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) {}
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

typedef void (*print_help_callback)();

#if (USE_WIFI == true)
  #include "express_wifi.h"
#endif

class express_console_menu : public express_console {
 public:
  static express_console_menu &getInstance() {
    static express_console_menu instance;  // Guaranteed to be destroyed.
                                           // Instantiated on first use.
    return instance;
  }

 private:
  express_console_menu() {}  // Constructor? (the {} brackets) are needed here.
 public:
  express_console_menu(express_console_menu const &) = delete;
  void operator=(express_console_menu const &) = delete;

 private:
  uint8_t verbosity_level = 0;
  void nvs_init();
  // struct menu_data_struct {
  //   uint8_t level;
  //   uint16_t CRC = 0;
  // } menu_data;

 public:
  size_t _num;
  size_t _bufferLen;
  char _buffer[CONSOLE_BUFFER_SIZE];
  Version version                       = {0, 0, 0};
  print_help_callback printHelpCallback = nullptr;
  vector<menu_item *> MENU_ITEMS;
  vector<menu_item *> MENU_POINTER;
#if (USE_NVS == true)
  uint32_t reboot_counter            = 0;
  uint32_t reboot_counter_resettable = 0;
#endif
  // express_nvs nvs = express_nvs();

  // void init(express_console_menu &self, Print &printer, const cmd_t *commands, size_t num, bool _prompt = true, bool _eeprom = true);
  void init(Print &printer, const cmd_t *commands, size_t num, bool _prompt = true);

  // uint16_t crc16() {
  //   return crcx::crc16((uint8_t *)&menu_data, sizeof(menu_data) - sizeof(menu_data.CRC));
  // }

  void default_verbosity_level() {
    // setLevel(3);
    verbosity_level                                   = DEFAULT_VERBOSITY_LEVEL;
    express_console_menu::getInstance()._filter_level = (express_console::Level)verbosity_level;
    express_nvs::getInstance().set("verbosity_level", &verbosity_level);
  }

  void default_reboot_counter() {
    reboot_counter_resettable = 0;
    express_nvs::getInstance().set("reboot_cnt_rst", &reboot_counter_resettable);
  }

  //   void saveSettings() {
  //     if (_eeprom) {
  //       menu_data.CRC = crc16();
  //       // EEPROM.put(__EXPRESS_CONSOLE_MENU_LEVEL_INDEX, menu_data);
  // #if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
  //         // EEPROM.commit();
  // #endif
  //     }
  //   }

  void update(bool _echo = true);

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
  void prompt();

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

  struct info;
  struct verbose;
  struct reboot;
  struct reset;
  struct factory_reset;
  struct halt;
  struct exit_sub;

  struct verbose;
  struct info;

  /*
  MENU: HELP
  */
  // namespace MENU {
  // struct express_console_menu::help : menu_item {
  //   express_console_menu *c;
  //   help() : menu_item({(char *)"Displays a list of the available commands."}) {
  //     this->commands.push_back((char *)"?");
  //     this->commands.push_back((char *)"help");
  //     this->console = &express_console_menu::getInstance();
  //   }
  //   void callback(const char *cmd, const char *arg, const uint8_t length) override {
  //     console->printBox(true);
  //     console->v().pln("   OPTIONS:").pln();
  //     for (auto &item : console->MENU_POINTER) {
  //       if (item->hidden == false) {
  //         console->v().p("   ");
  //         for (auto &command : item->commands) {
  //           console->v().p(command).p(", ");
  //         }
  //         console->v().p("\t\t").pln(item->help);
  //       }
  //     }
  //     console->printBox(false);
  //   }
  // };
  // };  // namespace MENU
};

#endif