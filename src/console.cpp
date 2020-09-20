#include "console.h"

// #include "menu_wifi.h"

// #ifdef __MENU_WIFI_H
// #include "menu_wifi.h"
// #endif


void _console::begin(const cmd_t* commands, size_t num, bool enable_prompt) {
  _bufferLen = 0;
  _commands = commands;
  _num = num;
  _includeWifi = false;

  if (enable_prompt) {
    prompt();
  }

  // MENUwifi.begin();
 
   

}

void _console::includeWifi() {
  #ifdef __MENU_WIFI_H
  _includeWifi = true;
  MENUwifi.begin();
  #endif
}



void _console::update() {
  while (_stream->available()) {
    char in = _stream->read();
    _printer->write(in);
    if (_bufferLen == 0 && in == ' ') {
      // trim
    } else if (in == '\r' || in == '\n') {
      if (in == '\r') {
        _printer->write('\n');
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
        --_bufferLen;
        _printer->write("\b \b", 3);
      }
    } else if (_bufferLen < CONSOLE_BUFFER_SIZE - 1) {
      _buffer[_bufferLen++] = in;
    }
  }
  // #ifdef __MENU_WIFI_H
  // if(_includeWifi);
  // MENUwifi.update();
  // #endif
}

void _console::processCommand(char* _ptr) {
  char* ptr = _ptr;
  char* arg = nullptr;
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
    String str = arg;
    uint16_t start_pos = str.indexOf('"') + 1;
    uint16_t end_pos = str.indexOf('"', start_pos);
    String value = str.substring(start_pos, end_pos);
    String then = str.substring(end_pos + 1, -1);
    then.trim();
    processFunction(_ptr, value.c_str(), strlen(value.c_str()) );
    if (strlen(then.c_str()) > 0) {
      processCommand((char*)then.c_str());
    } else {
      return;
    }
  } else {
    processFunction(_ptr, arg, strlen(arg));
  }
}

void _console::processFunction( const char *cmd, const char *arg, const uint8_t length) {
  // p("arg: ").pln(arg);
  // p("length: ").pln(length);
  // p("cmd: ").pln(cmd);
  if (strcmp("help", cmd) == 0 || strcmp("?", cmd) == 0) {
    printHelp();
  } else if (strcmp("info", cmd) == 0 || strcmp("i", cmd) == 0) {
    printInfo();
  } else if (strcmp("reboot", cmd) == 0) {
#if defined(ESP8266) || defined(ESP32)
    ESP.restart();
#endif
  } else if (strcmp("verbose", cmd) == 0 || strcmp("v", cmd) == 0) {
    vv().pln("@TODO: finish implementing verbose");
  } else {

#ifdef __MENU_WIFI_H
    if (_includeWifi && MENUwifi.processCommand(cmd, arg, length)) {
      return;
    }
#endif

    if (_commands && (_num > 0)) {
      const cmd_t* command = _commands;
      for (size_t i = 0; i < _num; ++i) {
        if (strcmp(command->command, cmd) == 0 || strcmp(command->shortcut, cmd) == 0) {
          command->action(cmd, arg, length);
          return;
        }
        ++command;
      }
      v().p("\nERROR: invalid option: ").pln(cmd);
      printHelp();
    }
  }


}


void _console::prompt() {
  _bufferLen = 0;
  _printer->print("~ $ ");
}

// _console console = _console();
_console console = _console();
