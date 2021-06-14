/*
    @file       express_plot.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V1.0.0
    @date       05/06/21
*/

#ifndef __EXPRESS_PLOT_H
#define __EXPRESS_PLOT_H

#include <Arduino.h>
// #include <unordered_map>
#include <vector>
using std::vector;

#include "express_console.h"
#include "express_console_menu.h"


class express_plot;

class express_plot : public express_console_menu {

 private:
  express_console_menu *_console;
  express_plot *_self;
  long int lastPlotted;

 public:
  typedef void (*callback_t)(void);
  callback_t callback_plot = nullptr;

  struct plots_stu {
    String name = "undefined";
    union {
      int *int_value;
      float *float_value;
    } value;
    float offset     = 0.0;
    float multiplier = 1.0;
  };
  vector<plots_stu *> plots;

  void init(express_plot &self, express_console_menu &console, int interval);
  void update();

  void add(String name, int *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot       = new plots_stu();
    new_plot->name            = name;
    new_plot->value.int_value = value_ptr;
    new_plot->offset          = offset;
    new_plot->multiplier      = multiplier;
    plots.push_back(new_plot);
  }

  void add(String name, float *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot         = new plots_stu();
    new_plot->name              = name;
    new_plot->value.float_value = value_ptr;
    new_plot->offset            = offset;
    new_plot->multiplier        = multiplier;
    plots.push_back(new_plot);
  }

  void callback(callback_t callback) {
    callback_plot = callback;
  }

  void interrupt();

  struct plotter;
  plotter *_plotter;

  struct menu_time;
  menu_time *_time;

  struct menu_interrupt;
  menu_interrupt *_menu_interrupt;

  struct plotter : menu_item {
    bool isPlotting = false;
    Level last;
    express_plot *_self;

    plotter(express_plot *self) : menu_item({"Plot data to console"}) {
      this->commands.push_back("p");
      this->commands.push_back("plot");
      this->_self = self;
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      if (this->isPlotting == false) {
        this->last = console._filter_level;
        console.setFilter(Level::p);
        this->isPlotting = true;
        console.MENU_POINTER.clear();
        console.newSubMenu();
        console.MENU_POINTER.push_back(this);
        console.MENU_POINTER.push_back(_self->_time);
        console.MENU_POINTER.push_back(_self->_menu_interrupt);
      } else {
        console.setFilter(this->last);
        this->isPlotting = false;
        console.v().p("verbose: ").pln((int)console._filter_level);
      }
    }
  };

  struct menu_time : menu_item {
    int interval = 30;
    typedef void (*callback_t)(void);
    callback_t callback_inter = nullptr;
    menu_time(int interval) : menu_item({"Plot time interval"}) {
      this->commands.push_back("t");
      this->commands.push_back("time");
      this->interval = interval;
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      if (length > 0) {
        this->interval = strtol(arg, nullptr, 10);
      } else {
        console.vvv().p("Time: ").p(this->interval).pln("ms.");
      }
    }
    int GETinterval() {
      return this->interval;
    }
  };

  struct menu_interrupt : menu_item {
    express_plot *_self;
    menu_interrupt(express_plot *self) : menu_item({"Interrupt plot time"}) {
      this->commands.push_back("i");
      this->commands.push_back("inter");
      this->_self = self;
    }
    void callback_console(const char *cmd, const char *arg, const uint8_t length, express_console_menu &console) override {
      _self->_print();
    }
  };

 private:
  void _print();
};

#endif