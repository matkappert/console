#pragma once
/*
    @file       express_plot.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V1.0.0
    @date       05/06/21
*/
#define EXPRESS_PLOT_VER "1.1.2"

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

class express_plot {
 public:
  const String version = EXPRESS_PLOT_VER;

 private:
  long int lastPlotted;
  int interval = DEFAULT_INTERVAL;

 public:
  typedef void (*callback_t)(void);
  callback_t callback_plot = nullptr;

  enum PLOT_TYPE {
    PLOT_INT = 0,
    PLOT_U_INT,
    PLOT_LONG,
    PLOT_U_LONG,
    PLOT_LONG_LONG,
    PLOT_U_LONG_LONG,
    PLOT_DOUBLE,
  };

  struct plots_stu {
    String name    = "undefined";
    PLOT_TYPE type = PLOT_DOUBLE;
    union {
      int *int_value;
      unsigned int *u_int_value;
      long *long_value;
      unsigned long *u_long_value;
      long long *long_long_value;
      unsigned long long *u_long_long_value;
      double *double_value;
    } value;
    float offset     = 0.0;
    float multiplier = 1.0;
  };
  vector<plots_stu *> plots;

  void init(int _interval = DEFAULT_INTERVAL);
  void update();

  size_t add(String name, int *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot       = new plots_stu();
    new_plot->name            = name;
    new_plot->type            = PLOT_INT;
    new_plot->value.int_value = value_ptr;
    new_plot->offset          = offset;
    new_plot->multiplier      = multiplier;
    plots.push_back(new_plot);
  }
  size_t add(String name, unsigned int *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot         = new plots_stu();
    new_plot->name              = name;
    new_plot->type              = PLOT_U_INT;
    new_plot->value.u_int_value = value_ptr;
    new_plot->offset            = offset;
    new_plot->multiplier        = multiplier;
    plots.push_back(new_plot);
  }
  size_t add(String name, long *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot        = new plots_stu();
    new_plot->name             = name;
    new_plot->type             = PLOT_LONG;
    new_plot->value.long_value = value_ptr;
    new_plot->offset           = offset;
    new_plot->multiplier       = multiplier;
    plots.push_back(new_plot);
  }
  size_t add(String name, unsigned long *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot          = new plots_stu();
    new_plot->name               = name;
    new_plot->type               = PLOT_U_LONG;
    new_plot->value.u_long_value = value_ptr;
    new_plot->offset             = offset;
    new_plot->multiplier         = multiplier;
    plots.push_back(new_plot);
  }
  size_t add(String name, long long *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot             = new plots_stu();
    new_plot->name                  = name;
    new_plot->type                  = PLOT_LONG_LONG;
    new_plot->value.long_long_value = value_ptr;
    new_plot->offset                = offset;
    new_plot->multiplier            = multiplier;
    plots.push_back(new_plot);
  }
  size_t add(String name, unsigned long long *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot               = new plots_stu();
    new_plot->name                    = name;
    new_plot->type                    = PLOT_U_LONG_LONG;
    new_plot->value.u_long_long_value = value_ptr;
    new_plot->offset                  = offset;
    new_plot->multiplier              = multiplier;
    plots.push_back(new_plot);
  }
  size_t add(String name, double *value_ptr, float offset = 0, float multiplier = 1.0) {
    plots_stu *new_plot          = new plots_stu();
    new_plot->name               = name;
    new_plot->type               = PLOT_DOUBLE;
    new_plot->value.double_value = value_ptr;
    new_plot->offset             = offset;
    new_plot->multiplier         = multiplier;
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
