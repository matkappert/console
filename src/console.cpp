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
		if (_bufferLen == 0 && in == ' ')  {
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
		if (*ptr == ' ' || *ptr == '=') {
			*ptr++ = '\0';
			break;
		}
		++ptr;
	}

	// trim whitespace after command
	while (*ptr == ' ') {
		++ptr;
	}
	arg = ptr;
	if (_commands && (_num > 0)) {
		const cmd_t *command = _commands;
		for (size_t i = 0; i < _num; ++i) {
			if (strcmp(command->command, _buffer) == 0 || strcmp(command->shortcut, _buffer) == 0 ) {
				vvvv().p("# ").p(command->command).p(": \"").p(arg).pln("\"");

				
				// vvvv().p("x: ").pln(x);
				// while (*x == '\"') {
				// 	++x;
				// 	vvvv().p("x: ").pln(x);
				// }

//	args in quotemarks

if(arg[0] == '"'){
char *x = arg;
char y[20] ;

    while (*x != '\0') {
		if (*x == '\"') {
			*x++ = '\0';
			vvvv().p("x:").pln(x);
			break;
		}
		++x;
		p("output x: ").pln(x);
	}


	// while (true) {
	//     if (*x == '\"') {
	// 		// *x++ = '\0';
	// 		vvvv().p("y:").pln(y);
	// 		break;
	// 	}
	// 	y = *x;
	// }

int c = 0;
while (true) {
	if (*x == '\"') {
		p("y:").pln(y);
			break;
	}
	 p("y-").p(y).p(", x-").pln(x);
      y[c] = x;
      ++x;
      c++;
   }


   


p("x:").pln(x);


}
				 // uint16_t start_pos = arg.indexOf('="') || 0;
				 // uint16_t end_pos = arg.indexOf('"', start_pos) || 0;
				 // String value = arg.substring(start_pos + 2, posHour );
				 // vvvv().p("start: ").p(cstart_pos).p(", end: ").p(end_pos).p(", value: '").p(value).pln("'");




				command->action(arg);
				return;
			}
			++command;
		}
		v().p("\nERROR: '").p(_buffer).pln("' command not found.");
	}
}

void _console::prompt() {
	_bufferLen = 0;
	_printer->print("> ");
}

_console console = _console();