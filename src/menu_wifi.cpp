#include "menu_wifi.h"
#include "console.h"
#include "EEPROM.h"

#ifdef ESP8266
	#include <ESP8266WiFi.h>
#elif ESP32
	#include <WiFi.h>
#endif


bool _MENUwifi::begin() {
#ifdef ESP32
	if (!EEPROM.begin(512)) {
		console.v().pln("ERROR! failed to initialise EEPROM");
		while (true) {
			delay(500);
		}
	}
#elif ESP8266
	EEPROM.begin(512);
#endif

	EEPROM.get(__MENU_WIFI_INDEX, wifi_data);

	if (strlen(wifi_data.ssid) <= 0 ) {
		console.vv().pln("No WiFi SSID set");
		wifi_params = false;
	}
	if (strlen(wifi_data.password) <= 0 ) {
		console.vv().pln("No WiFi password set");
		wifi_params = false;
	}

	update();
}

bool _MENUwifi::processCommand(const char *cmd, const char *arg, const uint8_t length) {
	if (strcmp("wifi_ssid", cmd) == 0 || strcmp("wifi_name", cmd) == 0 ) {
		if (length && length <= __MENU_WIFI_DATA_SIZE) {
			strcpy (wifi_data.ssid, arg);
			EEPROM.put(__MENU_WIFI_INDEX, wifi_data);
#if defined(ESP8266) || defined(ESP32)
			EEPROM.commit();
#endif
			console.vvv().pln("Reboot system to apply WiFi settings.");
			return true;
		} else if (length == 0) {
			EEPROM.get(__MENU_WIFI_INDEX, wifi_data);
			console.vvv().p("WiFi Name: ").pln(wifi_data.ssid);
			return true;
		}
		console.v().p("\n#ERROR! ").p(__PRETTY_FUNCTION__).p(":").pln(__LINE__).p("*cmd:").pln(cmd).p("*arg:").pln(arg).p("length:").pln(length);
		return false;
	} else if (strcmp("wifi_password", cmd) == 0 || strcmp("wifi_pwd", cmd) == 0) {
		if (length && length <= __MENU_WIFI_DATA_SIZE) {
			strcpy (wifi_data.password, arg);
			EEPROM.put(__MENU_WIFI_INDEX, wifi_data);
#if defined(ESP8266) || defined(ESP32)
			EEPROM.commit();
#endif
			console.vvv().pln("Reboot system to apply WiFi settings.");
			return true;
		} else if (length == 0) {
			EEPROM.get(__MENU_WIFI_INDEX, wifi_data);
			console.vvv().p("WiFi Password: ").pln(wifi_data.password);
			return true;
		}
		console.v().p("\n#ERROR! ").p(__PRETTY_FUNCTION__).p(":").pln(__LINE__).p("*cmd:").pln(cmd).p("*arg:").pln(arg).p("length:").pln(length);
		return false;
	} else {
		return false;
	}
}

void _MENUwifi::update() {
	if (!wifi_params) {
		console.v().pln("\n#SYSTEM HALTED! No WiFi settings.");
		while (true) {
			console.update();
		}
	} else {
		console.vvv().pln().p("Connecting to SSID: '").p((char *)wifi_data.ssid).pln("'");
		WiFi.mode(WIFI_STA);
		WiFi.begin((char *)wifi_data.ssid, (char *)wifi_data.password);
		// Loop until we're connected
		while (WiFi.status() != WL_CONNECTED) {
			console.update();
			delay(250);
			console.vvvv().p(".");
			// @TODO: add timeout to reboot
		}
		console.vvvv().pln();
		console.vvv().pln("WiFi connected.").p("IP address: ").pln(WiFi.localIP()).pln();
#ifdef LED_BUILTIN
		statusLedStop();
		statusLedSlowBlink();
#endif
	}
}

void _MENUwifi::printHelp(){
	console.v().pln("   WiFi:").pln();
}

_MENUwifi MENUwifi = _MENUwifi();




