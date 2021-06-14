/*
    @file       express_plot.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V1.0.0
    @date       05/06/21
*/

#include "express_plot.h"

void express_plot::init(express_plot &self, express_console_menu &console, int interval) {
  _self           = &self;
  _console        = &console;
  _time           = new menu_time(interval);
  _plotter        = new plotter(_self);
  _menu_interrupt = new menu_interrupt(_self);

  _console->MENU_ITEMS.push_back(_plotter);
  callback_plot = nullptr;
}

void express_plot::update() {
  if (_plotter->isPlotting == true) {
    if (millis() >= lastPlotted + _time->interval) {
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
  for (auto &plottt : plots) {
    float value = *plottt->value.float_value * plottt->multiplier;
    value       = plottt->offset > 0 ? value + plottt->offset : value + plottt->offset;
    _console->plot().p(plottt->name).p(':').p(value).p("\t");
  }
  _console->plot().pln();
}
