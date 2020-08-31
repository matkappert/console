/*
   advancedSerial Library
   https://github.com/klenov/advancedSerial
*/

#include "console.h"

// All implementation in .h because avr goes crazy with templates in .cpp


void _console::begin(const cmd_t *commands, size_t num) {
	_bufferLen = 0;
	_commands = commands;
	_num = num;

	prompt();
}

void _console::update() {
	while (_stream->available()) {
		char in = _stream->read();
		_printer->write(in);
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
				processCommand();
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

void _console::processCommand() {
	char *ptr = _buffer;
	char *arg = nullptr;
	while (*ptr != '\0') {
		if (*ptr == ' ') {
			*ptr++ = '\0';
			break;
		}
		++ptr;
	}
	while (*ptr == ' ') {
		++ptr;
	}
	arg = ptr;
	if (_commands && (_num > 0)) {
		const cmd_t *command = _commands;
		for (size_t i = 0; i < _num; ++i) {
			if (strcmp(command->command, _buffer) == 0) {
				command->action(arg);
				return;
			}
			++command;
		}
		_printer->print("\nERROR: \"");
		_printer->print(_buffer);
		_printer->println("\" command not found");
	}
}

void _console::prompt() {
	_bufferLen = 0;
	_printer->print("-> ");
}

_console console = _console();