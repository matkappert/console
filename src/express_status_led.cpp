#include "express_status_led.h"
#if defined(ESP8266) || defined(ESP8285)
Ticker tick;
#endif

express_status_led eLED;  // global-scoped variable

/************************************************************
    @brief Create an express_status_led instance

    @param led_pin, Microcontroller GPIO pin (LED)
    @param led_inverted, Invert the ouput pin
    @param callback, used for the timmer trigger
*************************************************************/
void express_status_led::init() {
  callback_with_arg = eLED.static_callback;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, false ^ LED_IS_INVERTED);
}

void express_status_led::flip(bool detach_timer) {
  _set(!state, detach_timer);
}

void express_status_led::on(bool detach_timer) {
  _set(true, detach_timer);
}

void express_status_led::off(bool detach_timer) {
  _set(false, detach_timer);
}

bool express_status_led::get() {
  _detach();
  state = digitalRead(LED_BUILTIN) ^ LED_IS_INVERTED;
  return state;
}

void express_status_led::static_callback() {
  eLED.callback();
}
void express_status_led::callback() {
  if (task.mode == OFF) {
    off(true);
  } else if (task.mode == BLINK) {
    task.value = task.value % 17;
    if (task.value <= 0) {
      task.value = 1;
    }

    flip(false);
    _attach_ms((1000 / task.value) / 2);
  } else if (task.mode == ERROR) {
    task.value = task.value % 11;
    if (task.value <= 0) {
      task.value = 1;
    }

    if (state == false && task_state < task.value) {
      on(false);
      ++task_state;
      _attach_ms(100);
    } else if (state == true && task_state <= task.value) {
      off(false);
      _attach_ms(250);
    } else {
      task_state = 0;
      off(false);
      _attach_ms(1250);
    }
  }
}

/************************************************************
    @brief Blink the output pin at a 50/50 ratio

    @param value, Speed value in Hz [rang = 1-16]
    @param detach_timer [default = true]
*************************************************************/
void express_status_led::blink(uint8_t value) {
  _set(true, true);
  task.mode  = BLINK;
  task.value = value;
  callback();
}

/************************************************************
    @brief Blink an error code, pause & repeat

    @param value, Error code, (n) time to blink [rang = 1-10]
    @param detach_timer [default = true]
*************************************************************/
void express_status_led::error(uint8_t value) {
  _set(true, true);
  task.mode  = ERROR;
  task.value = value;
  callback();
}

/************************************************************
    @brief Add an LED effect to a task que, 0 = highest priority

    @param priority, Que number [rang = 0-3]
    @param effect, Use flash effect from TASK:: [OFF, BLINK, ERROR]
    @param value, effect value settings
*************************************************************/
void express_status_led::addTask(uint8_t priority, uint8_t mode, uint8_t mode_value) {
  tasks[priority] = {mode, mode_value};
  _nextTask();
}

/************************************************************
    @brief Remove the task and apply the next task

    @param priority, Que number [rang = 0-3]
*************************************************************/
void express_status_led::removeTask(uint8_t priority) {
  tasks[priority] = {OFF, 0};
  _nextTask();
}

/************************************************************
    @brief Remove all the tasks and turn off

*************************************************************/
void express_status_led::removeAllTasks() {
  for (int x; x < sizeof_tasks; x++) {
    tasks[x] = {OFF, 0};
  }
  _nextTask();
}

/*********************************************************************
      Protected
**********************************************************************/

void express_status_led::_nextTask() {
  off(true);
  bool clear = true;
  for (int x; x < sizeof_tasks; x++) {
    if (tasks[x].mode != OFF) {
      clear      = false;
      task.mode  = tasks[x].mode;
      task.value = tasks[x].value;
      task_state = 0;
      break;
    }
  }
  if (clear) {
    off(true);
  } else {
    callback();
  }
}

void express_status_led::_set(bool set_state, bool detach_timer) {
  if (detach_timer) {
    _detach();
  }
  state = set_state;
  digitalWrite(LED_BUILTIN, state ^ LED_IS_INVERTED);
}

void express_status_led::_attach_ms(uint32_t milliseconds) {
#if defined(ESP32)
  esp_timer_create_args_t _timerConfig;
  _timerConfig.arg             = reinterpret_cast<void*>(0);
  _timerConfig.callback        = reinterpret_cast<callback_with_arg_t>(callback_with_arg);
  _timerConfig.dispatch_method = ESP_TIMER_TASK;
  _timerConfig.name            = "express_status_led";
  if (_timer) {
    esp_timer_stop(_timer);
    esp_timer_delete(_timer);
  }
  esp_timer_create(&_timerConfig, &_timer);
  esp_timer_start_once(_timer, milliseconds * 1000ULL);
#elif defined(ESP8266) || defined(ESP8285)
  tick.once_ms(milliseconds, callback_with_arg);
#endif
}

void express_status_led::_detach() {
#if defined(ESP32)
  if (_timer) {
    esp_timer_stop(_timer);
    esp_timer_delete(_timer);
    _timer = nullptr;

    task.mode  = OFF;
    task.value = 0;
    task_state = 0;
  }
#elif defined(ESP8266) || defined(ESP8285)
  tick.detach();
#endif
}
