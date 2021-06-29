/*
    @file       express_plot.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       05/06/21
*/

#include "express_plot.h"

express_plot ePlot;  // global-scoped variable



struct express_plot::menu_time : menu_item {
  menu_time() : menu_item({(char *)"Plot time interval"}) {
    this->commands.push_back((char *)"t");
    this->commands.push_back((char *)"time");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (length > 0) {
      ePlot.interval = strtol(arg, nullptr, 10);
    } else {
      eMenu.vvv().p("Time: ").p(ePlot.interval).pln("ms.");
    }
  }
  int GETinterval() {
    return ePlot.interval;
  }
};

struct express_plot::menu_interrupt : menu_item {
  menu_interrupt() : menu_item({(char *)"Interrupt plot time"}) {
    this->commands.push_back((char *)"i");
    this->commands.push_back((char *)"inter");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    ePlot._print();
  }
};

struct express_plot::plotter : menu_item {
  bool isPlotting = false;
  Level last;

  plotter() : menu_item({(char *)"Plot data to console"}) {
    this->commands.push_back((char *)"p");
    this->commands.push_back((char *)"plot");
  }
  void callback(const char *cmd, const char *arg, const uint8_t length) override {
    if (this->isPlotting == false) {
      this->last = eMenu._filter_level;
      eMenu.setFilter(Level::p);
      this->isPlotting = true;
      eMenu.MENU_POINTER.clear();
      eMenu.newSubMenu();
      eMenu.MENU_POINTER.push_back(this);
      // eMenu.MENU_POINTER.push_back(ePlot._time);
      // eMenu.MENU_POINTER.push_back(ePlot._menu_interrupt);
      eMenu.MENU_POINTER.push_back(new menu_time());
      eMenu.MENU_POINTER.push_back(new menu_interrupt());
    } else {
      eMenu.setFilter(this->last);
      this->isPlotting = false;
      eMenu.v().p("verbose: ").pln((int)eMenu._filter_level);
    }
  }
};

void express_plot::init(int interval) {
  // _time           = new menu_time(interval);
  _plotter        = new plotter();
  // _menu_interrupt = new menu_interrupt();

  eMenu.MENU_ITEMS.push_back(_plotter);
  callback_plot = nullptr;
}

void express_plot::update() {
  if (_plotter->isPlotting == true) {
    if (millis() >= lastPlotted + interval) {
      _print();
    }
  }
}

void express_plot::interrupt() {
  _print();
}

void express_plot::_print() {
  if (callback_plot) {
    callback_plot();
  }
  lastPlotted = millis();
  for (auto &item : plots) {
    float value = *item->value.float_value * item->multiplier;
    value       = item->offset > 0 ? value + item->offset : value + item->offset;
    eMenu.plot().p(item->name).p(':').p(value).p("\t");
  }
  eMenu.plot().pln();
}
