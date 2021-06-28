/*
    @file       express_console_menu.cpp
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V2.2.0
    @date       26/09/20
*/

#include "express_console_menu.h"

/*
  MENU: INFO
  */
struct express_console_menu::info : menu_item {
  info() : menu_item({(char *)"Displays firmware info."}) {
    this->commands.push_back((char *)"i");
    this->commands.push_back((char *)"info");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    console->printBox(true);
    console->v().pln("   INFO:").pln();
    console->v().p("   ").p("FW version").p(":\t\t").p(console->version.major).p(".").p(console->version.minor).p(".").p(console->version.patch).pln();
    console->v().p("   ").p("Build date").p(":\t\t").pln(__TIMESTAMP__);
    console->v().p("   ").p("GCC version").p(":\t\t").pln(__VERSION__);

    console->v().pln().pln("   Express Modules:");
    console->v().p("   ").p("Menu").p(":\t\t").pln(EXPRESS_CONSOLE_MENU_VER);
#if (USE_NVS == true)
    console->v().p("   ").p("NVS").p(":\t\t\t").pln(express_nvs::getInstance().version);
#endif
    #if (USE_WIFI == true)
          console->v().p("   ").p("WiFi").p(":\t\t").pln(express_wifi::getInstance().version);
    #endif

#if defined(ESP8266) || defined(ESP8285)
    console->v().pln().pln("   ESP8266:");
    uint32_t realSize   = ESP.getFlashChipRealSize();
    uint32_t ideSize    = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();
    console->v().p("   ").p("Flash ID").p(":\t\t").pln(ESP.getFlashChipId());
    console->v().p("   ").p("Flash size").p(":\t\t").pln(realSize);
    console->v().p("   ").p("Flash state").p(":\t\t").pln(ideSize != realSize ? "ERROR: Wrong configuration!" : "OKAY");
    console->v().p("   ").p("IDE size").p(":\t\t").pln(ideSize);
    console->v().p("   ").p("IDE speed").p(":\t\t").pln(ESP.getFlashChipSpeed());
    console->v().p("   ").p("IDE mode").p(":\t\t").pln(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");
#endif

#if defined(ESP32)
    console->v().pln().pln("   ESP32:");
    uint64_t chipid = ESP.getEfuseMac();  // The chip ID is essentially its MAC address(length: 6 bytes).
    console->v().p("   ").p("Chip ID").p(":\t\t").p((uint16_t)(chipid >> 32), HEX).pln((uint32_t)chipid, HEX);

    console->printInfo("Chip Model", ESP.getChipModel());
    console->printInfo("Chip Revision", ESP.getChipRevision());

    console->printInfo("Chip Cores", ESP.getChipCores());
    console->printInfo("Heap Size", ESP.getHeapSize());
    console->printInfo("Free Size", ESP.getFreeHeap());

    console->printInfo("Sketch Size", ESP.getSketchSize());
    console->printInfo("Sketch MD5", ESP.getSketchMD5());
    console->printInfo("SDK Version", ESP.getSdkVersion());
    console->printInfo("Free Sketch Space", ESP.getFreeSketchSpace());

    console->printInfo("Flash Chip Size", ESP.getFlashChipSize());
    console->printInfo("Flash Chip Speed", ESP.getFlashChipSpeed());
    console->printInfo("Flash Chip Mode", ESP.getFlashChipMode());
    // console->printInfo("magicFlashChipSize", ESP.magicFlashChipSize(ESP.getFlashChipSize()));
    // console->printInfo("magicFlashChipSpeed", ESP.magicFlashChipSpeed(ESP.getFlashChipSpeed()));
    // console->printInfo("magicFlashChipSpeed", ESP.magicFlashChipMode(ESP.getFlashChipMode()));

    // console->v().p("   ").p("ESP-IDF").p(":\t\t").pln(esp_get_idf_version());
    console->v().p("   ").p("Temprature").p(":\t\t").p((temprature_sens_read() - 32) / 1.8).pln("c");
#endif
#if defined(ESP32) || defined(ESP8266) || defined(ESP8285) && (USE_NVS == true)
    extern int32_t reboot_counter;
    console->v().pln().pln("   Reboot Counter:");
    console->v().p("   ").p("Lifetime").p(":\t\t").pln(express_console_menu::getInstance().reboot_counter);
    console->v().p("   ").p("Resettable").p(":\t\t").pln(express_console_menu::getInstance().reboot_counter_resettable);
#endif

    express_console_menu::getInstance().printBox(false);
  }
};

/*
    MENU: VERBOSE
    */
struct express_console_menu::verbose : menu_item {
  verbose() : menu_item({(char *)"Sets the message verbosity level."}) {
    this->commands.push_back((char *)"v");
    this->commands.push_back((char *)"verbose");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length) {
      uint8_t level = strtol(arg, nullptr, 10);
      if (level >= 0 && level < 5) {
        console->verbosity_level                          = level;
        express_console_menu::getInstance()._filter_level = (express_console::Level)console->verbosity_level;
        express_nvs::getInstance().set("verbosity_level", &console->verbosity_level);
        console->vvv().p("verbose set: ").pln(console->verbosity_level);
      } else {
        console->v().pln("ERROR: unkown level, pick a rang from 1-4");
      }
    } else {
      console->v().p("verbose: ").pln(console->verbosity_level);
    }
  }
};

/*
MENU: REBOOT
*/
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
struct express_console_menu::reboot : menu_item {
  reboot() : menu_item({(char *)"Reboot system."}) {
    this->commands.push_back((char *)"reboot");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    this->console->v().pln("INFO: Software Rebooting...").pln();
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
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    this->console->v().pln("INFO: Reset system settings...").pln();
    express_console_menu::getInstance().default_verbosity_level();
    express_console_menu::getInstance().default_reboot_counter();
  }
};

/*
MENU: FACTORY RESET
*/
struct express_console_menu::factory_reset : menu_item {
  factory_reset() : menu_item({(char *)""}) {
    this->commands.push_back((char *)"factory-reset");
    this->hidden  = true;
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    this->console->v().pln("INFO: factory resetting the system...").pln();
#if (USE_NVS == true)
    express_nvs::getInstance().erase_all();
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
    this->hidden  = true;
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    uint32_t value = strtol(arg, nullptr, 10);
    console->v().p("delay(").p(value).p(")...").pln();
    delay(value);
  }
};

/*
MENU: EXIT
*/
struct express_console_menu::exit_sub : menu_item {
  exit_sub() : menu_item({(char *)"return from sub menu"}) {
    this->commands.push_back((char *)"exit");
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    console->MENU_POINTER.assign(console->MENU_ITEMS.begin(), console->MENU_ITEMS.end());
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
    this->console = &express_console_menu::getInstance();
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    console->printBox(true);
    console->v().pln("   OPTIONS:").pln();
    for (auto &item : console->MENU_POINTER) {
      char cmds[30] = "";
      if (item->hidden == false) {
        uint8_t cmd_counter = 0;
         for (auto &command : item->commands) {
           cmd_counter++;
            strcat(cmds, command);
           if (cmd_counter < item->commands.size()){
             strcat(cmds, ", ");
           }
        }
        express_console_menu::getInstance().printInfo(cmds, item->help);
        // console->v().p("\t\t").pln();
      }
    }
    console->printBox(false);
  }
};
};

void express_console_menu::init(Print &printer, const cmd_t *commands, size_t num, bool _prompt) {
  _bufferLen = 0;
  _commands  = commands;
  _num       = num;
  // _eeprom    = eeprom;
  _printer = &printer;
  _stream  = (Stream *)&printer;

  delay(100);
  v().pln();
  v().pln();

  //   if (_eeprom) {
  // #ifdef ESP32
  //     if (!EEPROM.begin(512)) {
  //       v().pln("ERROR! failed to initialise EEPROM");
  //       while (true) {
  //         delay(500);
  //       }
  //     }
  // #elif defined(ESP8266) || defined(ESP8285)
  //     EEPROM.begin(512);
  // #endif
  //     EEPROM.get(__EXPRESS_CONSOLE_MENU_LEVEL_INDEX, menu_data);
  //     if (menu_data.CRC != crc16()) {
  //       v().pln("ERROR: EEPROM corrupt, loading defaults");
  //       defaultSettings();
  //     } else {
  //       p("level: ").pln(menu_data.level);
  // setLevel((uint8_t)menu_data.level);
  //     }
  //   }
  // setLevel(4);
  MENU_ITEMS.push_back(new MENU::help());
  MENU_ITEMS.push_back(new info());
  MENU_ITEMS.push_back(new verbose());
  MENU_ITEMS.push_back(new reset());
  MENU_ITEMS.push_back(new factory_reset());
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
      MENU_ITEMS.push_back(new reboot());
#endif
  MENU_ITEMS.push_back(new halt());

  nvs_init();

  if (_prompt) {
    prompt();
  }
}

void express_console_menu::nvs_init() {
#if (USE_NVS == true)
  vvvv().pln("express_console_menu::nvs_init() ...");
  express_nvs::getInstance().init();
  esp_err_t err;

  //
  // verbosity_level
  err = express_nvs::getInstance().get("verbosity_level", &verbosity_level);
  if (err == ESP_OK) {
    vvvv().p("_filter_level ").pln((uint8_t)express_console_menu::getInstance()._filter_level);
    express_console_menu::getInstance()._filter_level = (express_console::Level)verbosity_level;
  } else {
    v().pln("ERROR: restoring defaults for verbosity_level");
    default_verbosity_level();
  }

  //
  // restart_conters
  express_nvs::getInstance().get("reboot_cnt", &reboot_counter);
  reboot_counter++;
  express_nvs::getInstance().set("reboot_cnt", &reboot_counter);
  express_nvs::getInstance().get("reboot_cnt_rst", &reboot_counter_resettable);
  reboot_counter_resettable++;
  express_nvs::getInstance().set("reboot_cnt_rst", &reboot_counter_resettable);

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

// void express_console_menu::setLevel(uint8_t level) {
//   switch (level) {
//     case 0:
//       _filter_level   = Level::p;
//       verbosity_level = 0;
//       break;
//     case 1:
//       _filter_level   = Level::v;
//       verbosity_level = 1;
//       break;
//     case 2:
//       _filter_level   = Level::vv;
//       verbosity_level = 2;
//       break;
//     case 3:
//       _filter_level   = Level::vvv;
//       verbosity_level = 3;
//       break;
//     case 4:
//       _filter_level   = Level::vvvv;
//       verbosity_level = 4;
//       break;
//     default:
//       _filter_level   = Level::vvvv;
//       verbosity_level = 4;
//       break;
//   }
// }

void express_console_menu::update(bool _echo) {
  while (_stream->available()) {
    char in = _stream->read();
    if (_echo) {
      if (express_console_menu::getInstance()._filter_level > Level::p) {
        _printer->write(in);
      }
    }
    if (_bufferLen == 0 && in == ' ') {
      // trim
    } else if (in == '\r' || in == '\n') {
      if (in == '\r') {
        if (express_console_menu::getInstance()._filter_level > Level::p) {
          _printer->write('\n');
        }
      } else if (_bufferLen == 0) {
        // received "\n" alone: ignore it
        break;
      }

      // finish command
      _buffer[_bufferLen] = '\0';

      // process it
      if (_bufferLen > 0) {
        processCommand(_buffer);
      }

      // start again
      prompt();
      break;
    } else if ((in == 0x7F) || (in == '\b')) {
      // backspace
      if (_bufferLen > 0) {
        if (express_console_menu::getInstance()._filter_level > Level::p) {
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
        if ((void (*)())(item->*(&menu_item::callback_console)) != (void (*)())(&menu_item::callback_console)) {
          item->callback_console(cmd, arg, length, express_console_menu::getInstance());
        } else {
          item->callback(cmd, arg, length);
        }
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

void express_console_menu::prompt() {
  _bufferLen = 0;
  if (_filter_level > Level::p) {
    v().p("~ $ ");
  }
}
