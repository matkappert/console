/**
    @file       express_console_menu.cpp
    @author     matkappert
    @repo       github.com/matkappert/express
    @version    V2.0.0
    @date       26/09/20
    @format     C++=""-A14-xn-xc-xl-xk-xV-T4-C-N-xW-w-Y-p-xg-H-W0-xb-j-xf-xh-c-xp-xC200""
*/


#include "express_console_menu.h"
#include "EEPROM.h"

// #include "express_wifi.h"


void express_console_menu::init(Print &printer, const cmd_t* commands, size_t num, bool _prompt, bool eeprom) {
	_bufferLen = 0;
	_commands = commands;
	_num = num;
	_eeprom = eeprom;
	_printer = &printer;
	_stream = (Stream *)&printer;
	p("\n\n\n");

	if (_eeprom) {
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
		EEPROM.get(__EXPRESS_CONSOLE_MENU_LEVEL_INDEX, menu_data);
		if (menu_data.CRC != crc16()) {
			v().pln("ERROR: EEPROM corrupt, loading defaults");
			defaultSettings();
		} else {
			p("level: ").pln(menu_data.level);
			setLevel( (uint8_t) menu_data.level);
		}
	}

	if (_prompt) {
		prompt();
	}
}



void express_console_menu::setLevel(uint8_t level) {
	switch (level) {
	case 1:
		_filter_level = Level::v;
		menu_data.level = 1;
		break;
	case 2:
		_filter_level = Level::vv;
		menu_data.level = 2;
		break;
	case 3:
		_filter_level = Level::vvv;
		menu_data.level = 3;
		break;
	case 4:
		_filter_level = Level::vvvv;
		menu_data.level = 4;
		break;
	default:
		_filter_level = Level::vvvv;
		menu_data.level = 4;
		break;
	}
}

void express_console_menu::update(bool _echo ) {
	while (_stream->available()) {
		char in = _stream->read();
		if (_echo) {
			_printer->write(in);
		}
		if (_bufferLen == 0 && in == ' ') {
			// trim
		} else if (in == '\r' || in == '\n') {
			if (in == '\r') {
				_printer->write('\n');
			} else if (_bufferLen == 0) {
				// received "\n" alone: ignore it
				break;
			}

			// finish command
			_buffer[_bufferLen] = '\0';

			// process it
			if (_bufferLen > 0) {
				processCommand(_buffer);
			}

			// start again
			prompt();
			break;
		} else if ((in == 0x7F) || (in == '\b')) {
			// backspace
			if (_bufferLen > 0) {
				--_bufferLen;
				_printer->write("\b \b", 3);
			}
		} else if (_bufferLen < CONSOLE_BUFFER_SIZE - 1) {
			_buffer[_bufferLen++] = in;
		}
	}
}

void express_console_menu::processCommand(char* _ptr) {
	char* ptr = _ptr;
	char* arg = nullptr;
	while (*ptr != '\0') {
		if (*ptr == ' ' || *ptr == '=') {
			*ptr++ = '\0';
			break;
		}
		++ptr;
	}
	// trim whitespace after command
	while (*ptr == ' ' || *ptr == '=') {
		++ptr;
	}

	arg = ptr;
	if (arg[0] == '"') {
		String str = arg;
		uint16_t start_pos = str.indexOf('"') + 1;
		uint16_t end_pos = str.indexOf('"', start_pos);
		String value = str.substring(start_pos, end_pos);
		String then = str.substring(end_pos + 1, -1);
		then.trim();
		processFunction(_ptr, value.c_str(), strlen(value.c_str()) );
		if (strlen(then.c_str()) > 0) {
			processCommand((char*)then.c_str());
		} else {
			return;
		}
	} else {
		processFunction(_ptr, arg, strlen(arg));
	}
}

void express_console_menu::processFunction( const char *cmd, const char *arg, const uint8_t length) {
	vvvv().p("arg: ").pln(arg);
	vvvv().p("length: ").pln(length);
	vvvv().p("cmd: ").pln(cmd);
	if (strcmp("help", cmd) == 0 || strcmp("?", cmd) == 0) {
		printHelp();
	} else if (strcmp("info", cmd) == 0 || strcmp("i", cmd) == 0) {
		printInfo();
	} else if (strcmp("reboot", cmd) == 0) {
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)
		ESP.restart();
#endif
	} else if (strcmp("verbose", cmd) == 0 || strcmp("v", cmd) == 0) {
		if (length) {
			uint8_t level = strtol(arg, nullptr, 10);
			if (level > 0 && level < 5 ) {
				setLevel(level);
				saveSettings();
				vvv().p("verbose set: ").pln(level);
			} else {
				v().pln("ERROR: unkown level, pick a rang from 1-4");
			}
		} else {
			v().p("verbose: ").pln(menu_data.level);
		}
	} else {
		// wifi::test();
		if (_commands && (_num > 0)) {
			const cmd_t* command = _commands;
			for (size_t i = 0; i < _num; ++i) {
				if (strcmp(command->command, cmd) == 0 || strcmp(command->shortcut, cmd) == 0) {
					command->action(cmd, arg, length);
					return;
				}
				++command;
			}
			v().p("\nERROR: invalid option: ").pln(cmd);
			printHelp();
		}
	}
}



void express_console_menu::prompt() {
	_bufferLen = 0;
	_printer->print("~ $ ");
}