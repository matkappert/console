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
#include "EEPROM.h"
#define __EXPRESS_WIFI_LEVEL_INDEX 5
#include "./CRCx/CRCx.h"

#include "express_console_menu.h"
#include "express_status_led.h"

#if defined(ESP8266) || defined(ESP8285)
	#include <ESP8266WiFi.h>
#elif defined(ESP32)
#endif

class express_wifi  {

  private:
	struct wifi_data_struct {
		char *ssid;
		char *pwd;

		uint16_t CRC = 0;
	} wifi_data;

	// typedef void (*callback_t)(void);
	// typedef void (*callback_with_arg_t)(void*);
	// callback_t self;
  public:

	express_console_menu *console = nullptr;
	express_status_led *status = nullptr;

	express_wifi() {};


	void init();
	void update();

	void saveSettings() {
		// if (_eeprom) {
		wifi_data.CRC = crcx::crc16((uint8_t *)&wifi_data, sizeof(wifi_data) - sizeof(wifi_data.CRC) );
		EEPROM.put(__EXPRESS_WIFI_LEVEL_INDEX, wifi_data);
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
		EEPROM.commit();
#endif
		// }
	}

	void readSettings() {
		// if (_eeprom) {
#ifdef ESP32
		if (!EEPROM.begin(512)) {
			v().pln("ERROR! failed to initialise EEPROM");
			while (true) {
				delay(500);
			}
		}
#elif defined(ESP8266) || defined(ESP8285)
		EEPROM.begin(512);
#endif

		EEPROM.get(__EXPRESS_WIFI_LEVEL_INDEX, wifi_data);
		uint16_t crc16  = crcx::crc16((uint8_t *)&wifi_data, sizeof(wifi_data) - sizeof(wifi_data.CRC) );

		if (wifi_data.CRC != crc16) {
			console->v().pln("ERROR: Wifi EEPROM corrupt, loading defaults");
			console->vvvv().p("crc16:").p(crc16).p(", eeprom:").pln(wifi_data.CRC);
			defaultSettings();
		} else {
			console->vvvv().pln("loaded wifi eeprom data.");
			// p("level: ").pln(menu_data.level);
			// setLevel( (uint8_t) menu_data.level);
		}


		// }
	}

	void connect();
	void connect(char* ssid, char* pwd);

	void defaultSettings() {
		wifi_data.ssid = "wifi";
		wifi_data.pwd = "password";
		delay(500);
		saveSettings();
	}

	void printInfo() {
		console->v().p("wifi_info\t\t").pln("\t@TODO.");
	}

	void printHelp() {
		console->v().p("   ").p("wifi_ssid").p("\t\t").pln("Set WiFi SSID.");
		console->v().p("   ").p("wifi-password").p("\t").pln("Set WiFi password.");
		console->v().p("   ").p("wifi-default").p("\t\t").pln("reset WiFi settings.");
	}

};



#endif