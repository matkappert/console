/*
    @file       express_console_menu.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V2.1.0
    @date       26/09/20
*/

#ifndef __EXPRESS_CONSOLE_MENU_H
#define __EXPRESS_CONSOLE_MENU_H

#include <Arduino.h>

// #include <string>
// #include <unordered_map>
// using namespace std;
// using std::unordered_map;
#include <vector>
using std::vector;


#include "EEPROM.h"
#include "express_console.h"
#define __EXPRESS_CONSOLE_MENU_LEVEL_INDEX 0
#include "./CRCx/CRCx.h"


class express_console_menu;

struct menu_item {
  vector<char *> commands;
  char *help;  // e.g. 'Reboot ESP'
  boolean hidden = false;

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

typedef void (*print_help_callback)();

struct menu_item;

class express_console_menu : public express_console {
 private:
  bool _eeprom = false;

  struct menu_data_struct {
    uint8_t level;
    uint16_t CRC = 0;
  } menu_data;

 public:
  size_t _num;
  size_t _bufferLen;
  char _buffer[CONSOLE_BUFFER_SIZE];
  Version version                       = {0, 0, 0};
  print_help_callback printHelpCallback = nullptr;
  vector<menu_item *> MENU_ITEMS;
  vector<menu_item *> MENU_POINTER;


  void init(express_console_menu &self, Print &printer, const cmd_t *commands, size_t num, bool _prompt = true, bool _eeprom = true);

  uint16_t crc16() {
    return crcx::crc16((uint8_t *)&menu_data, sizeof(menu_data) - sizeof(menu_data.CRC));
  }

  void defaultSettings() {
    setLevel(3);
  }

  void saveSettings() {
    if (_eeprom) {
      menu_data.CRC = crc16();
      EEPROM.put(__EXPRESS_CONSOLE_MENU_LEVEL_INDEX, menu_data);
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
      EEPROM.commit();
#endif
    }
  }

  void update(bool _echo = true);

  void setLevel(uint8_t level);

  void printBox(bool isStart) {
    v().p(isStart ? "\n┌" : "└");
    for (uint8_t i = 0; i < 113; ++i) {
      v().p("─");
    }
    v().pln(isStart ? "┐" : "┘");
  };

 private:
  Stream *_stream;
  cmd_t *_addedCommands[9];

  const cmd_t *_commands;

  void processCommand(char *_ptr);
  void processFunction(const char *cmd, const char *arg, const uint8_t length);
  void prompt();

 public:
  void newSubMenu();
  void newSubMenu(vector<menu_item *> sub);
  express_console_menu *_self;

  struct info : menu_item {
    express_console_menu *c;
    info(express_console_menu console) : menu_item({"Displays firmware info."}) {
      this->c = &console;
      this->commands.push_back("i");
      this->commands.push_back("info");
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      console.printBox(true);
      console.v().pln("   INFO v2:").pln();
      console.v().p("   ").p("FW version").p(":\t\t").p(console.version.major).p(".").p(console.version.minor).p(".").p(console.version.patch).pln();
      console.v().p("   ").p("Build date").p(":\t\t").pln(__TIMESTAMP__);
      console.v().p("   ").p("GCC version").p(":\t\t").pln(__VERSION__);

#if defined(ESP8266) || defined(ESP8285)
      console.v().pln().pln("   ESP8266:");
      uint32_t realSize   = ESP.getFlashChipRealSize();
      uint32_t ideSize    = ESP.getFlashChipSize();
      FlashMode_t ideMode = ESP.getFlashChipMode();
      console.v().p("   ").p("Flash ID").p(":\t\t").pln(ESP.getFlashChipId());
      console.v().p("   ").p("Flash size").p(":\t\t").pln(realSize);
      console.v().p("   ").p("Flash state").p(":\t\t").pln(ideSize != realSize ? "ERROR: Wrong configuration!" : "OKAY");
      console.v().p("   ").p("IDE size").p(":\t\t").pln(ideSize);
      console.v().p("   ").p("IDE speed").p(":\t\t").pln(ESP.getFlashChipSpeed());
      console.v().p("   ").p("IDE mode").p(":\t\t").pln(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");
#endif

#if defined(ESP32)
      console.v().pln().pln("   ESP32:");
      uint64_t chipid = ESP.getEfuseMac();  // The chip ID is essentially its MAC address(length: 6 bytes).
      console.v().p("   ").p("Chip ID").p(":\t\t").p((uint16_t)(chipid >> 32), HEX).pln((uint32_t)chipid, HEX);
#endif
      console.printBox(false);
    }
  };

  struct verbose : menu_item {
    express_console_menu *c;
    verbose(express_console_menu console) : menu_item({"Sets the message verbosity level."}) {
      this->c = &console;
      this->commands.push_back("v");
      this->commands.push_back("verbose");
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      if (length) {
        uint8_t level = strtol(arg, nullptr, 10);
        if (level >= 0 && level < 5) {
          console.setLevel(level);
          console.saveSettings();
          console.vvv().p("verbose set: ").pln(level);
        } else {
          console.v().pln("ERROR: unkown level, pick a rang from 1-4");
        }
      } else {
        console.v().p("verbose: ").pln(console.menu_data.level);
      }
    }
  };

#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
  struct reboot : menu_item {
    reboot(express_console_menu console) : menu_item({"Reboot system."}) {
      this->commands.push_back("reboot");
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      console.v().pln("INFO: Software Rebooting...").pln();
      delay(1000);
      ESP.restart();
    }
  };
#endif

  struct halt : menu_item {
    halt(express_console_menu console) : menu_item({"Halt system for N ms"}) {
      this->commands.push_back("delay");
      this->commands.push_back("sleep");
      this->commands.push_back("halt");
      this->hidden = true;
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      uint32_t value = strtol(arg, nullptr, 10);
      console.v().p("delay(").p(value).p(")...").pln();
      delay(value);
    }
  };

  struct exit_sub : menu_item {
    exit_sub(express_console_menu console) : menu_item({"return from sub menu"}) {
      this->commands.push_back("exit");
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      console.MENU_POINTER.assign(console.MENU_ITEMS.begin(), console.MENU_ITEMS.end());
    }
  };
};

namespace MENU {
struct help : menu_item {
  express_console_menu *c;
  help(express_console_menu console) : menu_item({"Displays a list of the available commands."}) {
    this->c = &console;
    this->commands.push_back("?");
    this->commands.push_back("help");
  }
  void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
    console.printBox(true);
    console.v().pln("   OPTIONS:").pln();
    for (auto &item : console.MENU_POINTER) {
      if (item->hidden == false) {
        console.v().p("   ");
        for (auto &command : item->commands) {
          console.v().p(command).p(", ");
        }
        console.v().p("\t\t").pln(item->help);
      }
    }
    console.printBox(false);
  }
};
}

#endif