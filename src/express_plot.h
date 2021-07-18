
#ifndef __EXPRESS_PLOT_H
#define __EXPRESS_PLOT_H
/**
 * @file express_plot.h
 * @author Mathew Kappert
 * @brief Print arduino's serial plotter
 * @version 0.1
 * @date 05-06-2021
 *
 * @copyright Copyright (c) 2021
 *
 */
#define EXPRESS_PLOT_VER "0.1"

#include <Arduino.h>

#include "Settings.h"
#include "express_utilities.h"
/*
 * Forward-Declarations
 */
struct express_plot;
extern express_plot ePlot;
struct PLOT_STRUCT;

// #include <unordered_map>
#include <vector>
using std::vector;

#include "express_console.h"
#include "express_console_menu.h"

class express_plot {
 public:
  const String version = EXPRESS_PLOT_VER;
  typedef void (*callback_t)(void);
  callback_t callback_plot = nullptr;

 private:
  long int lastPlotted;
  int interval = DEFAULT_INTERVAL;

 public:
  vector<PLOT_STRUCT *> PLOT_STRUCT_VECTOR;
  char payload[150];

  void init(int _interval = DEFAULT_INTERVAL);
  void update();

#define ADD_h(TYPE, ENUM) size_t add(char *name, TYPE *value_ptr, float offset = 0, float multiplier = 1.0);
#define ADD_cpp(TYPE, ENUM)                                                               \
  size_t express_plot::add(char *name, TYPE *value_ptr, float offset, float multiplier) { \
    PLOT_STRUCT *new_plot = new PLOT_STRUCT({name, ENUM});                                \
    new_plot->value.ENUM  = value_ptr;                                                    \
    new_plot->offset      = offset;                                                       \
    new_plot->multiplier  = multiplier;                                                   \
    PLOT_STRUCT_VECTOR.push_back(new_plot);                                               \
  }
  ADD_h(int8_t, Int8);
  ADD_h(int16_t, Int16);
  ADD_h(int32_t, Int32);
  ADD_h(uint8_t, UInt8);
  ADD_h(uint16_t, UInt16);
  ADD_h(uint32_t, UInt32);
  ADD_h(float, Float);
  ADD_h(double, Double);
  ADD_h(boolean, Boolean);

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

  String valueToBuffer(PLOT_STRUCT *plot, float offset = 0, float multiplier = 1);

 private:
  void _print();
};

struct PLOT_STRUCT {
  String name = "undefined";
  EXPRESS_TYPE_ENUM type;
  union {
    int8_t *Int8;
    int16_t *Int16;
    int32_t *Int32;
    uint8_t *UInt8;
    uint16_t *UInt16;
    uint32_t *UInt32;
    float *Float;
    double *Double;
    boolean *Boolean;
  } value;
  float offset     = 0.0;
  float multiplier = 1.0;
  PLOT_STRUCT(char *name, EXPRESS_TYPE_ENUM type) {
    this->name = name;
    this->type = type;
    ePlot.PLOT_STRUCT_VECTOR.push_back(this);
  }
};

#endif