/**
    @file       express_status_led.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V1.0.0
    @date       28/09/20
    @format     C++=""-A14-xn-xc-xl-xk-xV-T4-C-N-xW-w-Y-p-xg-H-W0-xb-j-xf-xh-c-xp-xC200""
*/


#ifndef __EXPRESS_STATUS_LED_H
#define __EXPRESS_STATUS_LED_H


#include "arduino.h"

#if defined(ESP32)
extern "C" {
#include "esp_timer.h"
}
#elif defined(ESP8266) || defined(ESP8285)
#include <Ticker.h>
#endif



enum TASK {
	OFF,
	BLINK,
	ERROR
};

typedef struct {
	uint8_t mode;
	uint8_t value;
} task_t;

class express_status_led {
  private:

#define sizeof_tasks  4
	task_t tasks[sizeof_tasks];
	task_t task;
	uint8_t task_state;


  public:

	bool inverted;
	uint8_t pin;
	bool state;

	typedef void (*callback_t)(void);
	typedef void (*callback_with_arg_t)(void*);
	callback_t self;

	express_status_led(): inverted(false), pin(0xFF), state(false), self(nullptr) {};

	void init(uint8_t led_pin, bool led_inverted, callback_t callback);

	bool get();
	void flip(bool detach_timer = true);
	void on(bool detach_timer = true);
	void off(bool detach_timer = true);

	void callback();
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