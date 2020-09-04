/**
 * @file    console.h
 * @author  matkappert
 * @repo    github.com/matkappert/console
 * @version V0.1.1
 * @date    03/09/20
 * @format  http://format.krzaq.cc (style: google)
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdarg.h>

#include "Arduino.h"

typedef struct {
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
} _version;

#define CONSOLE_BUFFER_SIZE 100
typedef void (*cmd_action_t)(const char *, uint8_t);
typedef struct {
  const char *shortcut;
  const char *command;
  cmd_action_t action;
  const char *description;
} cmd_t;

class _console {
 public:
  enum class Level { v = 0, vv, vvv, vvvv };

 private:
  bool _output_enabled;

  Level _filter_level;
  const Level default_filter_level = Level::v;

  Level _message_level;
  const Level default_message_level = Level::v;

  Print *_printer;
  Stream *_stream;
  cmd_t *_addedCommands[9];

  // cmd >>
  const cmd_t *_commands;

  size_t _num;
  size_t _bufferLen;
  char _buffer[CONSOLE_BUFFER_SIZE];
  char _reg[CONSOLE_BUFFER_SIZE];

  inline bool shouldBePrinted(void) {
    return _output_enabled && (_message_level <= _filter_level);
  }

 public:
  _version version = {0, 0, 1};
  /*! * default Constructor */
  _console()
      : _output_enabled(true),
        _filter_level(default_filter_level),
        _message_level(default_message_level),
        _printer(nullptr),

        _bufferLen(0),
        _commands(nullptr),
        _num(0) {}

  void setPrinter(Print &printer) {
    _printer = &printer;
    _stream = (Stream *)&printer;
  }

  void setFilter(Level f_level) { _filter_level = f_level; }

  void on(void) { _output_enabled = true; }
  void off(void) { _output_enabled = false; }

  String messageLevel() {
    switch (_message_level) {
      case Level::v:
        return "Level: 1";
        break;
      case Level::vv:
        return "Level: 2";
        break;
      case Level::vvv:
        return "Level: 3";
        break;
      case Level::vvvv:
        return "Level: 4";
        break;
      default:
        return "Level: UNKNOWN";
        break;
    }
  }

  _console &level(Level m_level) {
    _message_level = m_level;
    return *this;
  }
  inline _console &l(Level m_level) { return level(m_level); }

  _console &v() { return level(Level::v); }

  _console &vv() { return level(Level::vv); }

  _console &vvv() { return level(Level::vvv); }

  _console &vvvv() { return level(Level::vvvv); }

  template <typename Type>
  _console &print(Type tX) {
    if (shouldBePrinted()) {
      _printer->print(tX);
    }
    return *this;
  }

  template <typename Type>
  _console &p(Type tX) {
    return print(tX);
  }

  template <typename Type>
  _console &println(Type tX) {
    if (shouldBePrinted()) {
      print(tX);
      _printer->println();
    }
    return *this;
  }

  template <typename Type>
  _console &pln(Type tX) {
    return println(tX);
  }

  _console &print(long n, int base) {
    if (shouldBePrinted()) {
      _printer->print(n, base);
    }
    return *this;
  }
  inline _console &p(long n, int base) { return print(n, base); }

  _console &println(long n, int base) {
    if (shouldBePrinted()) {
      print(n, base);
      _printer->println();
    }
    return *this;
  }
  inline _console &pln(long n, int base) { return println(n, base); }

  _console &println(void) {
    if (shouldBePrinted()) {
      _printer->println();
    }
    return *this;
  }
  inline _console &pln(void) { return println(); }

  void begin(const cmd_t *commands, size_t num, bool enable_prompt = false);

  void printHelp() {
    const cmd_t *command = _commands;
    printBox(true);
    v().pln("   OPTIONS:").pln();
    v().p("   ?, help\t\t").pln("Displays a list of the available commands.");
    v().p("   i, info\t\t").pln("Displays firmware info.");
    v().p("   v, verbose\t\t")
        .p("Sets the message verbosity level. (")
        .p(messageLevel())
        .pln(")");
#ifdef ESP8266
    v().pln("      reboot\t\tReboot system.");
#endif
    v().pln();
    for (size_t i = 0; i < _num; ++i) {
      v().p("   ");
      strlen(command->shortcut) ? p(command->shortcut).p(", ") : p("   ");
      p(command->command).p("\t").p(command->description).pln();
      ++command;
    }
    printBox(false);
  }

  void printInfo() {
    printBox(true);
    v().pln("   INFO:").pln();
    v().p("   ")
        .p("FW version")
        .p(":\t\t")
        .p(version.major)
        .p(".")
        .p(version.minor)
        .p(".")
        .p(version.patch)
        .pln();
    v().p("   ").p("Build date").p(":\t\t").pln(__TIMESTAMP__);
    v().p("   ").p("GCC version").p(":\t\t").pln(__VERSION__);

#ifdef ESP8266
    v().pln().pln("   ESP8266:");
    uint32_t realSize = ESP.getFlashChipRealSize();
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();
    v().p("   ").p("Flash ID").p(":\t\t").pln(ESP.getFlashChipId());
    v().p("   ").p("Flash size").p(":\t\t").pln(realSize);
    v().p("   ")
        .p("Flash state")
        .p(":\t\t")
        .pln(ideSize != realSize ? "ERROR: Wrong configuration!" : "OKAY");
    v().p("   ").p("IDE size").p(":\t\t").pln(ideSize);
    v().p("   ").p("IDE speed").p(":\t\t").pln(ESP.getFlashChipSpeed());
    v().p("   ")
        .p("IDE mode")
        .p(":\t\t")
        .pln(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT
                                             ? "QOUT"
                                             : ideMode == FM_DIO
                                                   ? "DIO"
                                                   : ideMode == FM_DOUT
                                                         ? "DOUT"
                                                         : "UNKNOWN");
#endif

    printBox(false);
  }

  void update();

  void printBox(bool isStart) {
    v().p(isStart ? "\n┌" : "└");
    for (uint8_t i = 0; i < 115; ++i) {
      v().p("─");
    }
    v().pln(isStart ? "┐" : "┘");
  }

  void addCommand(cmd_t *commands);

 private:
  void reset();
  void processCommand(char *_ptr);
  void prompt();
  void find(char *buf);
};

extern _console console;
typedef _console::Level Level;
#endif
