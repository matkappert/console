/*
    @file       express_console_menu.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V2.0.0
    @date       26/09/20
    @format     C++=""-A14-xn-xc-xl-xk-xV-T4-C-N-xW-w-Y-p-xg-H-W0-xb-j-xf-xh-c-xp-xC200""
*/


#ifndef __EXPRESS_CONSOLE_MENU_H
#define __EXPRESS_CONSOLE_MENU_H

#include "express_console.h"
#include "arduino.h"

#include "EEPROM.h"
#define __EXPRESS_CONSOLE_MENU_LEVEL_INDEX 0
#include "./CRCx/CRCx.h"

// #if defined(ESP8266) || defined(ESP8285)	
// #endif

#define CONSOLE_BUFFER_SIZE 100
typedef void (*cmd_action_t)(const char *, const char *, uint8_t);
typedef struct {
	const char *shortcut;
	const char *command;
	cmd_action_t action;
	const char *description;
} cmd_t;

typedef struct {
	uint8_t major;
	uint8_t minor;
	uint8_t patch;
} Version;

typedef void (*print_help_callback)();


class express_console_menu : public  express_console {
  private:
	bool  _eeprom = false;


	struct menu_data_struct {
		uint8_t level;
		uint16_t CRC = 0;
	} menu_data;


  public:
	size_t _num;
	size_t _bufferLen;
	char _buffer[CONSOLE_BUFFER_SIZE];
	Version version = {0, 0, 0};
	print_help_callback printHelpCallback = nullptr;

	// express_wifi *_wifi;

	void init(Print &printer, const cmd_t* commands, size_t num, bool _prompt = true, bool _eeprom = true);

	uint16_t crc16() {
		return crcx::crc16((uint8_t *)&menu_data, sizeof(menu_data) - sizeof(menu_data.CRC) );
	}

	void defaultSettings() {
		setLevel(3);
	}

	void saveSettings() {
		if (_eeprom) {
			menu_data.CRC = crc16();
			EEPROM.put(__EXPRESS_CONSOLE_MENU_LEVEL_INDEX, menu_data);
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
			EEPROM.commit();
#endif
		}
	}

	void update(bool _echo = true);

	void setLevel(uint8_t level);

	// String messageLevel() {
	//  switch (_filter_level) {
	//  case Level::v:
	//      return "Level: 1";
	//      break;
	//  case Level::vv:
	//      return "Level: 2";
	//      break;
	//  case Level::vvv:
	//      return "Level: 3";
	//      break;
	//  case Level::vvvv:
	//      return "Level: 4";
	//      break;
	//  default:
	//      return "Level: UNKNOWN";
	//      break;
	//  }
	// }


	void printHelp() {
		const cmd_t *command = _commands;
		printBox(true);
		v().pln("   OPTIONS:").pln();
		v().p("   ?, help\t\t").pln("Displays a list of the available commands.");
		v().p("   i, info\t\t").pln("Displays firmware info.");
		v().p("   v, verbose\t\t").p("Sets the message verbosity level. (Level: ").p(menu_data.level).pln(")");
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
		v().pln("      reboot\t\tReboot system.");
#endif
		v().pln();
		for (size_t i = 0; i < _num; ++i) {
			v().p("   ");
			strlen(command->shortcut) ? p(command->shortcut).p(", ") : p("   ");
			p(command->command).p("\t").p(command->description).pln();
			++command;
		}
		if (printHelpCallback) {
			printHelpCallback();
		}
		printBox(false);
	}

	void printInfo() {
		printBox(true);
		v().pln("   INFO:").pln();
		v().p("   ").p("FW version").p(":\t\t").p(version.major).p(".").p(version.minor).p(".").p(version.patch).pln();
		v().p("   ").p("Build date").p(":\t\t").pln(__TIMESTAMP__);
		v().p("   ").p("GCC version").p(":\t\t").pln(__VERSION__);

#if defined(ESP8266) || defined(ESP8285)
		v().pln().pln("   ESP8266:");
		uint32_t realSize = ESP.getFlashChipRealSize();
		uint32_t ideSize = ESP.getFlashChipSize();
		FlashMode_t ideMode = ESP.getFlashChipMode();
		v().p("   ").p("Flash ID").p(":\t\t").pln(ESP.getFlashChipId());
		v().p("   ").p("Flash size").p(":\t\t").pln(realSize);
		v().p("   ").p("Flash state").p(":\t\t").pln(ideSize != realSize ? "ERROR: Wrong configuration!" : "OKAY");
		v().p("   ").p("IDE size").p(":\t\t").pln(ideSize);
		v().p("   ").p("IDE speed").p(":\t\t").pln(ESP.getFlashChipSpeed());
		v().p("   ").p("IDE mode").p(":\t\t").pln(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");
#endif

		printBox(false);
	}
	void printBox(bool isStart) {
		v().p(isStart ? "\n┌" : "└");
		for (uint8_t i = 0; i < 113; ++i) {
			v().p("─");
		}
		v().pln(isStart ? "┐" : "┘");
	}



  private:
	Stream *_stream;
	cmd_t *_addedCommands[9];

	const cmd_t *_commands;

	void processCommand(char* _ptr);
	void processFunction( const char *cmd, const char *arg, const uint8_t length);
	void prompt() ;


};
#endif