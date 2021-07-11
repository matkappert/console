/*
    @file       express_console_menu.cpp
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       26/09/20
*/

#include "express_console_menu.h"

#include "Settings.h"

express_console_menu eMenu;  // global-scoped variable

/*
  MENU: INFO
  */
struct express_console_menu::information : menu_item {
  information() : menu_item({(char *)"Displays firmware info."}) {
    this->commands.push_back((char *)"i");
    this->commands.push_back((char *)"info");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.printBox(true);
    eMenu.v().pln("   INFO:").pln();
    eMenu.v().p("   ").p("FW version").p(":\t\t").p(eMenu.version.major).p(".").p(eMenu.version.minor).p(".").p(eMenu.version.patch).pln();
    eMenu.v().p("   ").p("Build date").p(":\t\t").pln(__TIMESTAMP__);
    eMenu.v().p("   ").p("GCC version").p(":\t\t").pln(__VERSION__);

    eMenu.v().pln().pln("   Express Modules:");
    eMenu.v().p("   ").p("Menu").p(":\t\t").pln(EXPRESS_CONSOLE_MENU_VER);
#if (USE_NVS == true)
    eMenu.v().p("   ").p("NVS").p(":\t\t\t").pln(eNvs.version);
#endif
#if (USE_WIFI == true)
    eMenu.v().p("   ").p("WiFi").p(":\t\t").pln(eWifi.version);
#endif
#if (USE_PLOT == true)
    eMenu.v().p("   ").p("Plot").p(":\t\t").pln(ePlot.version);
#endif

#if defined(ESP8266) || defined(ESP8285)
    eMenu.v().pln().pln("   ESP8266:");
    uint32_t realSize   = ESP.getFlashChipRealSize();
    uint32_t ideSize    = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();
    eMenu.v().p("   ").p("Flash ID").p(":\t\t").pln(ESP.getFlashChipId());
    eMenu.v().p("   ").p("Flash size").p(":\t\t").pln(realSize);
    eMenu.v().p("   ").p("Flash state").p(":\t\t").pln(ideSize != realSize ? "ERROR: Wrong configuration!" : "OKAY");
    eMenu.v().p("   ").p("IDE size").p(":\t\t").pln(ideSize);
    eMenu.v().p("   ").p("IDE speed").p(":\t\t").pln(ESP.getFlashChipSpeed());
    eMenu.v().p("   ").p("IDE mode").p(":\t\t").pln(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");
#endif

#if defined(ESP_PLATFORM)

  #ifdef ARDUINO_VARIANT
    Serial.print("\nESP32 Board:      ");
    Serial.print(ARDUINO_VARIANT);
  #endif

    eMenu.v().pln().pln("   ESP32:");
    uint64_t chipid = ESP.getEfuseMac();  // The chip ID is essentially its MAC address(length: 6 bytes).
    eMenu.v().p("   ").p("Chip ID").p(":\t\t").p((uint16_t)(chipid >> 32), HEX).pln((uint32_t)chipid, HEX);

    eMenu.printInfo("Chip Model", ESP.getChipModel());
    eMenu.printInfo("Chip Revision", ESP.getChipRevision());

    eMenu.printInfo("Chip Cores", ESP.getChipCores());
    eMenu.printInfo("Heap Size", ESP.getHeapSize());
    eMenu.printInfo("Free Size", ESP.getFreeHeap());

    eMenu.printInfo("Sketch Size", ESP.getSketchSize());
    eMenu.printInfo("Sketch MD5", ESP.getSketchMD5());
    eMenu.printInfo("SDK Version", ESP.getSdkVersion());
    eMenu.printInfo("Free Sketch Space", ESP.getFreeSketchSpace());

    eMenu.printInfo("Flash Chip Size", ESP.getFlashChipSize());
    eMenu.printInfo("Flash Chip Speed", ESP.getFlashChipSpeed());
    eMenu.printInfo("Flash Chip Mode", ESP.getFlashChipMode());
    // eMenu.printInfo("magicFlashChipSize", ESP.magicFlashChipSize(ESP.getFlashChipSize()));
    // eMenu.printInfo("magicFlashChipSpeed", ESP.magicFlashChipSpeed(ESP.getFlashChipSpeed()));
    // eMenu.printInfo("magicFlashChipSpeed", ESP.magicFlashChipMode(ESP.getFlashChipMode()));

    // eMenu.v().p("   ").p("ESP-IDF").p(":\t\t").pln(esp_get_idf_version());
    eMenu.v().p("   ").p("Temprature").p(":\t\t").p((temprature_sens_read() - 32) / 1.8).pln("c");
#endif
#if defined(ESP_PLATFORM)  && (USE_NVS == true)
    extern int32_t reboot_counter;
    eMenu.v().pln().pln("   Reboot Counter:");
    eMenu.v().p("   ").p("Lifetime").p(":\t\t").pln(eMenu.reboot_counter);
    eMenu.v().p("   ").p("Resettable").p(":\t\t").pln(eMenu.reboot_counter_resettable);
#endif

    eMenu.printBox(false);
  }
};

/*
    MENU: VERBOSE
    */
struct express_console_menu::verbose : menu_item {
  verbose() : menu_item({(char *)"Sets the message verbosity level."}) {
    this->commands.push_back((char *)"v");
    this->commands.push_back((char *)"verbose");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length) {
      uint8_t level = strtol(arg, nullptr, 10);
      if (level >= 0 && level < 5) {
        eMenu.verbosity_level = level;
        eMenu._filter_level   = (express_console::Level)eMenu.verbosity_level;
        eNvs.set("verbosity_level", &eMenu.verbosity_level);
        eMenu.vvv().p("verbose set: ").pln(eMenu.verbosity_level);
      } else {
        eMenu.v().pln("ERROR: unkown level, pick a rang from 1-4");
      }
    } else {
      eMenu.v().p("verbose: ").pln(eMenu.verbosity_level);
    }
  }
};

/*
MENU: REBOOT
*/
#if defined(ESP_PLATFORM)
struct express_console_menu::reboot : menu_item {
  reboot() : menu_item({(char *)"Reboot system."}) {
    this->commands.push_back((char *)"reboot");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.v().pln("INFO: Software Rebooting...").pln();
    delay(1000);
    ESP.restart();
  }
};
#endif

/*
MENU: RESET
*/
struct express_console_menu::reset : menu_item {
  reset() : menu_item({(char *)"reset system settings."}) {
    this->commands.push_back((char *)"reset");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.v().pln("INFO: Reset system settings...").pln();
    eMenu.default_verbosity_level();
    eMenu.default_reboot_counter();
  }
};

/*
MENU: FACTORY RESET
*/
struct express_console_menu::factory_reset : menu_item {
  factory_reset() : menu_item({(char *)""}) {
    this->commands.push_back((char *)"factory-reset");
    this->hidden = true;
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.v().pln("INFO: factory resetting the system...").pln();
#if (USE_NVS == true)
    eNvs.erase_all();
  #if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
    delay(1000);
    ESP.restart();
  #endif
#endif
  }
};

/*
MENU: HALT
*/
struct express_console_menu::halt : menu_item {
  halt() : menu_item({(char *)"Halt system for N ms"}) {
    this->commands.push_back((char *)"delay");
    this->commands.push_back((char *)"sleep");
    this->commands.push_back((char *)"halt");
    this->hidden = true;
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    uint32_t value = strtol(arg, nullptr, 10);
    eMenu.v().p("delay(").p(value).p(")...").pln();
    delay(value);
  }
};

/*
MENU: EXIT
*/
struct express_console_menu::exit_sub : menu_item {
  exit_sub() : menu_item({(char *)"return from sub menu"}) {
    this->commands.push_back((char *)"exit");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.MENU_POINTER.assign(eMenu.MENU_ITEMS.begin(), eMenu.MENU_ITEMS.end());
  }
};

/*
MENU: HELP
*/
namespace MENU {
struct help : menu_item {
  express_console_menu *c;
  help() : menu_item({(char *)"Displays a list of the available commands."}) {
    this->commands.push_back((char *)"?");
    this->commands.push_back((char *)"help");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    eMenu.printBox(true);
    eMenu.v().pln("   OPTIONS:").pln();
    for (auto &item : eMenu.MENU_POINTER) {
      char cmds[30] = "";
      if (item->hidden == false) {
        uint8_t cmd_counter = 0;
        for (auto &command : item->commands) {
          cmd_counter++;
          strcat(cmds, command);
          if (cmd_counter < item->commands.size()) {
            strcat(cmds, ", ");
          }
        }
        eMenu.printInfo(cmds, item->help);
        // eMenu.v().p("\t\t").pln();
      }
    }
    eMenu.printBox(false);
  }
};
};  // namespace MENU

void express_console_menu::init(Print &printer) {
  _bufferLen = 0;
  _printer   = &printer;
  _stream    = (Stream *)&printer;

  delay(100);
  v().pln();
  v().pln();

  MENU_ITEMS.push_back(new MENU::help());
  MENU_ITEMS.push_back(new information());
  MENU_ITEMS.push_back(new verbose());
  MENU_ITEMS.push_back(new reset());
  MENU_ITEMS.push_back(new factory_reset());
#if defined(ESP_PLATFORM)
  MENU_ITEMS.push_back(new reboot());
#endif
  MENU_ITEMS.push_back(new halt());

  nvs_init();


}

void express_console_menu::nvs_init() {
#if (USE_NVS == true)
  vvvv().pln("express_console_menu::nvs_init() ...");
  eNvs.init();
  esp_err_t err;

  //
  // verbosity_level
  err = eNvs.get("verbosity_level", &verbosity_level);
  if (err == ESP_OK) {
    vvvv().p("_filter_level ").pln((uint8_t)eMenu._filter_level);
    eMenu._filter_level = (express_console::Level)verbosity_level;
  } else {
    v().pln("ERROR: restoring defaults for verbosity_level");
    default_verbosity_level();
  }

  //
  // restart_conters
  eNvs.get("reboot_cnt", &reboot_counter);
  reboot_counter++;
  eNvs.set("reboot_cnt", &reboot_counter);
  eNvs.get("reboot_cnt_rst", &reboot_counter_resettable);
  reboot_counter_resettable++;
  eNvs.set("reboot_cnt_rst", &reboot_counter_resettable);

  vvvv().pln("express_console_menu::nvs_init() ... done");
#else
  verbosity_level = DEFAULT_VERBOSITY_LEVEL;
  setLevel(DEFAULT_VERBOSITY_LEVEL);
#endif
}

void express_console_menu::newSubMenu() {
  MENU_POINTER.insert(MENU_POINTER.begin(), new MENU::help());
  MENU_POINTER.insert(MENU_POINTER.begin(), new exit_sub());
}

void express_console_menu::newSubMenu(vector<menu_item *> sub) {
  MENU_POINTER.assign(sub.begin(), sub.end());
  MENU_POINTER.insert(MENU_POINTER.begin(), new MENU::help());
  MENU_POINTER.insert(MENU_POINTER.begin(), new exit_sub());
}

void express_console_menu::update() {
  while (_stream->available()) {
    char in = _stream->read();

    if (_bufferLen == 0 && in == ' ') {
      // trim
    } else if (in == '\r' || in == '\n') {
      if (in == '\r') {
        if (eMenu._filter_level > Level::p) {
          _printer->write('\n');
        }
      } else if (_bufferLen == 0) {
        // received "\n" alone: ignore it
        break;
      }

      // finish command
      _buffer[_bufferLen] = '\0';

// echo the command back
#if (DEFAULT_MENU_ECHO == true)
      // @TODO: save echo in nvs
      if (eMenu._filter_level > Level::p) {
        v().pln().p("-> ").pln(_buffer);
      }
#endif

      // process it
      if (_bufferLen > 0) {
        processCommand(_buffer);
      }

      // start again
      _bufferLen = 0;

      break;
    } else if ((in == 0x7F) || (in == '\b')) {
      // backspace
      if (_bufferLen > 0) {
        if (eMenu._filter_level > Level::p) {
          --_bufferLen;
          _printer->write("\b \b", 3);
        }
      }
    } else if (_bufferLen < CONSOLE_BUFFER_SIZE - 1) {
      _buffer[_bufferLen++] = in;
    }
  }
}

void express_console_menu::processCommand(char *_ptr) {
  char *ptr = _ptr;
  char *arg = nullptr;
  while (*ptr != '\0') {
    if (*ptr == ' ' || *ptr == '=') {
      *ptr++ = '\0';
      break;
    }
    ++ptr;
  }
  // trim whitespace after command
  while (*ptr == ' ' || *ptr == '=') {
    ++ptr;
  }

  arg = ptr;
  if (arg[0] == '"') {
    String str         = arg;
    uint16_t start_pos = str.indexOf('"') + 1;
    uint16_t end_pos   = str.indexOf('"', start_pos);
    String value       = str.substring(start_pos, end_pos);
    String then        = str.substring(end_pos + 1, -1);
    then.trim();
    processFunction(_ptr, value.c_str(), strlen(value.c_str()));
    if (strlen(then.c_str()) > 0) {
      processCommand((char *)then.c_str());
    } else {
      return;
    }
  } else {
    processFunction(_ptr, arg, strlen(arg));
  }
}

void express_console_menu::processFunction(const char *cmd, const char *arg, const uint8_t length) {
  vvvv().pln();
  vvvv().p("cmd: ").pln(cmd);
  vvvv().p("arg: ").pln(arg);
  vvvv().p("length: ").pln(length);

  boolean matched = false;
  for (auto &item : MENU_POINTER) {
    if (matched == true) {
      break;
    }
    for (auto &command : item->commands) {
      if (strcmp(command, cmd) == 0) {
        vvvv().p("command: \"").p(command).pln("\" = true");
        matched = true;
        // if ((void (*)())(item->*(&menu_item::callback_console)) != (void (*)())(&menu_item::callback_console)) {
        //   item->callback_console(cmd, arg, length, eMenu);
        // } else {
        item->callback(cmd, arg, length);
        // }
        break;
      } else {
        vvvv().p("command: \"").p(command).pln("\" = false");
      }
    }
    vvvv().pln();
  }
  if (matched == false) {
    v().p("\nERROR: invalid option: ").pln(cmd);
  }
}

