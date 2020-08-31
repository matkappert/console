/*
   Thanks to klenov
   advancedSerial Library
   https://github.com/klenov/advancedSerial
*/

#ifndef ADVANCED_SERIAL
#define ADVANCED_SERIAL

#include <stdarg.h>
#include "Arduino.h"

#define CONSOLE_BUFFER_SIZE 100
typedef void (*cmd_action_t)(const char *);
typedef struct {
  const char *shortcut;
  const char *command;
  cmd_action_t action;
} cmd_t;


class _console {
public:
    enum class Level { v, vv, vvv, vvvv };
private:
    bool _output_enabled;

    Level _filter_level;
    const Level default_filter_level = Level::v;

    Level _message_level;
    const Level default_message_level = Level::v;

    Print*  _printer;
    Stream* _stream;

    // cmd >>
    const cmd_t *_commands;
    size_t _num;
    size_t _bufferLen;
    char _buffer[CONSOLE_BUFFER_SIZE];

    inline bool shouldBePrinted(void) { return _output_enabled && ( _message_level <= _filter_level ); }
public:
    /*! * default Constructor */
    _console()
      : _output_enabled(true),
        _filter_level(default_filter_level),
        _message_level(default_message_level),
        _printer(nullptr),
        // cmd >>
        _bufferLen(0),
        _commands(nullptr),
        _num(0){}

    void setPrinter(Print &printer) {
      _printer = &printer;
      _stream =  &printer;
    }

    void setFilter(Level f_level) {
      _filter_level = f_level;
    }

    void on(void)  { _output_enabled = true;  }
    void off(void) { _output_enabled = false; }

    _console& level(Level m_level) {
      _message_level = m_level;
      return *this;
    }
    inline _console& l(Level m_level) { return level(m_level); }

    _console& v() {
      return level(Level::v);
    }

    _console& vv() {
      return level(Level::vv);
    }

    _console& vvv() {
      return level(Level::vvv);
    }

    _console& vvvv() {
      return level(Level::vvvv);
    }

    template <typename Type>
    _console& print(Type tX) {
      if ( shouldBePrinted() ) {
        _printer->print(tX);
      }
      return *this;
    }

    template <typename Type>
    _console& p(Type tX) {
      return print(tX);
    }

    template <typename Type>
    _console& println(Type tX) {
      if( shouldBePrinted() ) {
        print(tX);
        _printer->println();
      }
      return *this;
    }

    template <typename Type>
    _console& pln(Type tX) {
      return println(tX);
    }

    _console& print(long n, int base) {
      if ( shouldBePrinted() ) {
        _printer->print(n, base);
      }
      return *this;
    }
    inline _console& p(long n, int base) { return print(n, base); }

    _console& println(long n, int base) {
      if ( shouldBePrinted() ) {
        print(n, base);
        _printer->println();
      }
      return *this;
    }
    inline _console& pln(long n, int base) { return println(n, base); }

    _console& println(void) {
      if ( shouldBePrinted() ) {
        _printer->println();
      }
      return *this;
    }
    inline _console& pln(void) { return println(); }



    void begin(const cmd_t *commands, size_t num);
    void update();

private:
    void reset();
    void processCommand(char *_ptr);
    void prompt();
    char trim(char *cstr);
};

extern _console console;
typedef _console::Level Level;
#endif
