/**
 * @file express_status_led.h
 * @author Mathew Kappert
 * @brief
 * @version 0.1
 * @date 28-09-2020
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef __EXPRESS_LED_STATUS_H
#define __EXPRESS_LED_STATUS_H
#pragma once
#define EXPRESS_LED_STATUS_VER "0.1"

#include "arduino.h"
#include "settings.h"

/*
 * Forward-Declarations
 */
struct express_status_led;
extern express_status_led eLED;

#if defined(ESP32)
extern "C" {
  #include "esp_timer.h"
}
#elif defined(ESP8266) || defined(ESP8285)
  #include <Ticker.h>
#endif

enum TASK { OFF, BLINK, ERROR };

typedef struct {
  uint8_t mode;
  uint8_t value;
} task_t;

struct express_status_led {
 public:
  const String version = EXPRESS_LED_STATUS_VER;

 private:
#define sizeof_tasks 4
  task_t tasks[sizeof_tasks];
  task_t task;
  uint8_t task_state;

 public:
  bool state;

  typedef void (*callback_t)(void);
  typedef void (*callback_with_arg_t)(void*);
  callback_t callback_with_arg;

  void init();

  bool get();
  void flip(bool detach_timer = true);
  void on(bool detach_timer = true);
  void off(bool detach_timer = true);

  void callback();
  static void static_callback();
  void blink(uint8_t value);
  void error(uint8_t value);

  void addTask(uint8_t priority, uint8_t mode, uint8_t mode_value);
  void removeTask(uint8_t priority);
  void removeAllTasks();

 protected:
  void _nextTask();
  void _set(bool set_state, bool detach_timer = true);

#if defined(ESP32)
  esp_timer_handle_t _timer;
#endif
  void _attach_ms(uint32_t milliseconds);
  void _detach();
};

#endif