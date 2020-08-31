/*
   advancedSerial Library
   https://github.com/klenov/advancedSerial
*/

#include "console.h"

// All implementation in .h because avr goes crazy with templates in .cpp

void _console::begin(const cmd_t * commands, size_t num) {
  _bufferLen = 0;
  _commands = commands;
  _num = num;

  prompt();
}

void _console::update() {
  while (_stream -> available()) {
    char in = _stream -> read();
    _printer -> write( in );
    if (_bufferLen == 0 && in == ' ') {
      // trim
    } else if ( in == '\r' || in == '\n') {
      if ( in == '\r') {
        _printer -> write('\n');
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
    } else if (( in == 0x7F) || ( in == '\b')) {
      // backspace
      if (_bufferLen > 0) {
        --_bufferLen;
        _printer -> write("\b \b", 3);
      }
    } else if (_bufferLen < CONSOLE_BUFFER_SIZE - 1) {
      _buffer[_bufferLen++] = in ;
    }
  }
}

char _console::trim(char * cstr) {
  char * str = cstr;
  while ( * str == ' ' || * str == '=') {
    ++str;
  }
  return str;
}

void _console::processCommand(char * _ptr) {
  char * ptr = _ptr;
  char * arg = nullptr;

  while ( * ptr != '\0') {
    if ( * ptr == ' ' || * ptr == '=') {
      * ptr++ = '\0';
      break;
    }
    ++ptr;
  }

  // trim whitespace after command
  while ( * ptr == ' ' || * ptr == '=') {
    ++ptr;
  }
  
  arg = ptr;
  if (_commands && (_num > 0)) {
    const cmd_t * command = _commands;
    for (size_t i = 0; i < _num; ++i) {
      if (strcmp(command -> command, _ptr) == 0 || strcmp(command -> shortcut, _ptr) == 0) {
        if (arg[0] == '"') {
          String str = arg;

          uint16_t start_pos = str.indexOf('"') + 1;
          uint16_t end_pos = str.indexOf('"', start_pos);
          String value = str.substring(start_pos, end_pos);
          String then = str.substring(end_pos + 1, -1);
          then.trim();
          ptr = then.c_str();
          command -> action(value.c_str());
          if (then.indexOf('"', start_pos + 1) >= 0) {
            processCommand(ptr);
          } else {
            return;
          }
        } else {
          command -> action(arg);
        }
        return;
      }
      ++command;
    }
    v().p("\nERROR: '").p(_ptr).pln("' command not found.");
  }
}

void _console::prompt() {
  _bufferLen = 0;
  _printer -> print("> ");
}

_console console = _console();