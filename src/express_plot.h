#pragma once
/*
    @file       express_plot.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V1.0.0
    @date       05/06/21
*/
#define EXPRESS_PLOT_VER "1.1.0"

#include <Arduino.h>

#include "Settings.h"
/*
 * Forward-Declarations
 */
struct express_plot;
extern express_plot ePlot;

// #include <unordered_map>
#include <vector>
using std::vector;

#include "express_console.h"
#include "express_console_menu.h"

class express_plot{
 public:
  const String version = EXPRESS_PLOT_VER;

 private:
  long int lastPlotted;
  int interval = DEFAULT_INTERVAL;

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

  void init(int interval = DEFAULT_INTERVAL);
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
  // menu_time *_time;
  struct menu_interrupt;
  // menu_interrupt *_menu_interrupt;



 private:
  void _print();
};
