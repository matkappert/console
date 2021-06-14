/*
    @file       express_console_menu.cpp
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V2.1.0
    @date       26/09/20
*/

#include "express_console_menu.h"

#include "EEPROM.h"

void express_console_menu::init(express_console_menu &self, Print &printer, const cmd_t *commands, size_t num, bool _prompt, bool eeprom) {
  _bufferLen = 0;
  _commands  = commands;
  _num       = num;
  _eeprom    = eeprom;
  _printer   = &printer;
  _stream    = (Stream *)&printer;
  _self      = (express_console_menu *)&self;
  p("\n\n\n");

  if (_eeprom) {
#ifdef ESP32
    if (!EEPROM.begin(512)) {
      v().pln("ERROR! failed to initialise EEPROM");
      while (true) {
        delay(500);
      }
    }
#elif defined(ESP8266) || defined(ESP8285)
    EEPROM.begin(512);
#endif
    EEPROM.get(__EXPRESS_CONSOLE_MENU_LEVEL_INDEX, menu_data);
    if (menu_data.CRC != crc16()) {
      v().pln("ERROR: EEPROM corrupt, loading defaults");
      defaultSettings();
    } else {
      p("level: ").pln(menu_data.level);
      setLevel((uint8_t)menu_data.level);
    }
  }

  if (_prompt) {
    prompt();
  }

  MENU_ITEMS.push_back(new MENU::help(self));
  MENU_ITEMS.push_back(new info(self));
  MENU_ITEMS.push_back(new verbose(self));
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
  MENU_ITEMS.push_back(new reboot(self));
#endif
  MENU_ITEMS.push_back(new halt(self));
}

void express_console_menu::newSubMenu() {
  MENU_POINTER.insert(MENU_POINTER.begin(), new MENU::help(*_self));
  MENU_POINTER.insert(MENU_POINTER.begin(), new exit_sub(*_self));
}

void express_console_menu::newSubMenu(vector<menu_item *> sub) {
  MENU_POINTER.assign(sub.begin(), sub.end());
  MENU_POINTER.insert(MENU_POINTER.begin(), new MENU::help(*_self));
  MENU_POINTER.insert(MENU_POINTER.begin(), new exit_sub(*_self));
}

void express_console_menu::setLevel(uint8_t level) {
  switch (level) {
    case 0:
      _filter_level   = Level::p;
      menu_data.level = 0;
      break;
    case 1:
      _filter_level   = Level::v;
      menu_data.level = 1;
      break;
    case 2:
      _filter_level   = Level::vv;
      menu_data.level = 2;
      break;
    case 3:
      _filter_level   = Level::vvv;
      menu_data.level = 3;
      break;
    case 4:
      _filter_level   = Level::vvvv;
      menu_data.level = 4;
      break;
    default:
      _filter_level   = Level::vvvv;
      menu_data.level = 4;
      break;
  }
}

void express_console_menu::update(bool _echo) {
  while (_stream->available()) {
    char in = _stream->read();
    if (_echo) {
      if (_self->_filter_level > Level::p) {
        _printer->write(in);
      }
    }
    if (_bufferLen == 0 && in == ' ') {
      // trim
    } else if (in == '\r' || in == '\n') {
      if (in == '\r') {
        if (_self->_filter_level > Level::p) {
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
        if (_self->_filter_level > Level::p) {
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
          item->callback_console(cmd, arg, length, *_self);
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
  if (_self->_filter_level > Level::p) {
    _printer->print("~ $ ");
  }
}
