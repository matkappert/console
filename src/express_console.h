/**
    @file       express_console.h
    @author     matkappert
    @repo       github.com/matkappert/express_console
    @version    V2.0.0
    @date       26/09/20
    @format     C++=""-A14-xn-xc-xl-xk-xV-T4-C-N-xW-w-Y-p-xg-H-W0-xb-j-xf-xh-c-xp-xC200""
*/


#ifndef __EXPRESS_CONSOLE_H
#define __EXPRESS_CONSOLE_H

#include <stdarg.h>
#include "Arduino.h"

class express_console {
  public:
  enum class Level { p, v, vv, vvv, vvvv };
  Print*  _printer;

  bool _output_enabled;

  Level _filter_level;
  const Level default_filter_level = Level::v;

  Level _message_level;
  const Level default_message_level = Level::v;

inline bool shouldBePrinted(void) {
    return _output_enabled && ( _message_level <= _filter_level );
  }
  private:
  
  public:
  /*! * default Constructor */
  express_console()
    : _output_enabled(true),
      _filter_level(default_filter_level),
      _message_level(default_message_level),
      _printer(nullptr) {}

  void init(Print &printer) {
    _printer = &printer;
  }

  void setFilter(Level f_level) {
    _filter_level = f_level;
  }

  void on(void)  {
    _output_enabled = true;
  }
  void off(void) {
    _output_enabled = false;
  }

  express_console& level(Level m_level) {
    _message_level = m_level;
    return *this;
  }
  inline express_console& l(Level m_level) {
    return level(m_level);
  }

  express_console& v() {
    return level(Level::v);
  }

  express_console& vv() {
    return level(Level::vv);
  }

  express_console& vvv() {
    return level(Level::vvv);
  }

  express_console& vvvv() {
    return level(Level::vvvv);
  }

  express_console &plot()
  {
    return level(Level::p);
  }

  template <typename Type>
  express_console& print(Type tX) {
    if ( shouldBePrinted() ) {
      _printer->print(tX);
    }
    return *this;
  }

  template <typename Type>
  express_console& p(Type tX) {
    return print(tX);
  }

  template <typename Type>
  express_console& println(Type tX) {
    if ( shouldBePrinted() ) {
      print(tX);
      _printer->println();
    }
    return *this;
  }

  template <typename Type>
  express_console& pln(Type tX) {
    return println(tX);
  }

  express_console& print(long n, int base) {
    if ( shouldBePrinted() ) {
      _printer->print(n, base);
    }
    return *this;
  }
  inline express_console& p(long n, int base) {
    return print(n, base);
  }

  express_console& println(long n, int base) {
    if ( shouldBePrinted() ) {
      print(n, base);
      _printer->println();
    }
    return *this;
  }
  inline express_console& pln(long n, int base) {
    return println(n, base);
  }

  express_console& println(void) {
    if ( shouldBePrinted() ) {
      _printer->println();
    }
    return *this;
  }
  inline express_console& pln(void) {
    return println();
  }
};

// extern express_console aSerial;
typedef express_console::Level Level;
#endif