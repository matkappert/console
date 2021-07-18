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
  // int GETinterval() {
  //   return ePlot.interval;
  // }
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

ADD_cpp(int8_t, Int8);
ADD_cpp(int16_t, Int16);
ADD_cpp(int32_t, Int32);
ADD_cpp(uint8_t, UInt8);
ADD_cpp(uint16_t, UInt16);
ADD_cpp(uint32_t, UInt32);
ADD_cpp(float, Float);
ADD_cpp(double, Double);
ADD_cpp(boolean, Boolean);

void express_plot::init(int _interval) {
  interval = _interval;
  _plotter = new plotter();

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

String express_plot::valueToBuffer(PLOT_STRUCT *plot, float offset, float multiplier) {
  char buffer[(20 * sizeof(char)) + 1];
  if (plot->type == Int8) {
    itoa((*plot->value.Int8 * multiplier) + offset, buffer, 10);
  } else if (plot->type == Int16) {
    itoa((*plot->value.Int16 * multiplier) + offset, buffer, 10);
  } else if (plot->type == Int32) {
    itoa((*plot->value.Int32 * multiplier) + offset, buffer, 10);
  } else if (plot->type == UInt8) {
    utoa((*plot->value.UInt8 * multiplier) + offset, buffer, 10);
  } else if (plot->type == UInt16) {
    utoa((*plot->value.UInt16 * multiplier) + offset, buffer, 10);
  } else if (plot->type == UInt32) {
    utoa((*plot->value.UInt32 * multiplier) + offset, buffer, 10);
  } else if (plot->type == Float) {
    dtostrf((*plot->value.Float * multiplier) + offset, -6, 3, buffer);
  } else if (plot->type == Double) {
    dtostrf((*plot->value.Double * multiplier) + offset, -6, 3, buffer);
  } else if (plot->type == Boolean) {
    sprintf(buffer, "%s", *plot->value.Boolean ? "TRUE" : "FALSE");
  }
  return String(buffer);
}

void express_plot::_print() {
  if (callback_plot) {
    callback_plot();
  }
  lastPlotted = millis();
  payload[0]  = '\0';
  for (auto &plot : PLOT_STRUCT_VECTOR) {
    char temp[50];
    sprintf(temp, "%s:%s\t", plot->name, valueToBuffer(plot, plot->offset, plot->multiplier));
    strcat(payload, temp);
  }
  eMenu.plot().pln(payload);
}
