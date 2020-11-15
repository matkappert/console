/*
    @file       express_wifi.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V1.0.0
    @date       14/11/20
    @format     C++=""-A14-xn-xc-xl-xk-xV-T4-C-N-xW-w-Y-p-xg-H-W0-xb-j-xf-xh-c-xp-xC200""
*/

#ifndef __EXPRESS_WIFI_H
#define __EXPRESS_WIFI_H

#include "arduino.h"

#include "express_console_menu.h"
#include "express_status_led.h"

#if defined(ESP8266) || defined(ESP8285)
	#include <ESP8266WiFi.h>
#elif defined(ESP32)
#endif

class express_wifi  {

  private:
	struct wifi_data_struct {
		char* ssid;
		char* pwd;

		uint16_t CRC = 0;
	} wifi_data;

	typedef void (*callback_t)(void);
	typedef void (*callback_with_arg_t)(void*);
	callback_t self;
	express_console_menu *console;
	express_status_led *status;


  public:

	express_wifi(): self(nullptr) {};


	void init(express_console_menu &_console, express_status_led &_status, callback_t _callback);
	void update();

	void callback();

	void connect();
	void connect(char* ssid, char* pwd);



};





#endif