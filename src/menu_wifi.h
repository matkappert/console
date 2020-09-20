#ifndef __MENU_WIFI_H
#define __MENU_WIFI_H

// #include "console.h"
#include <EEPROM.h>


// class _MENUwifi : public _console {
class _MENUwifi {
public:
	_MENUwifi(){};

#define __MENU_WIFI_INDEX 0
#define __MENU_WIFI_DATA_SIZE 40
	bool wifi_params = true;


	
	bool begin();
	bool processCommand(const char *cmd, const char *arg, const uint8_t length);
	void update();

	void printHelp();

	struct wifi_data_struct {
		char ssid[__MENU_WIFI_DATA_SIZE];
		char password[__MENU_WIFI_DATA_SIZE];
	} wifi_data;
};


// extern _MENUwifi MENUwifi;
#endif