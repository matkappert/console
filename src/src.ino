#include "console_menu.h"
console_menu console = console_menu();


#include "status_led.h"
status_led status = status_led();


void statusCallback() {
	status.callback();
};


void stateCallback( const char *cmd, const char *arg, const uint8_t length) {
	console.v().p("State: ").pln(status.state);
};
void flipCallback( const char *cmd, const char *arg, const uint8_t length) {
	status.flip();
};
void onCallback( const char *cmd, const char *arg, const uint8_t length) {
	status.on();
};
void offCallback( const char *cmd, const char *arg, const uint8_t length) {
	status.off();
};

void blinkCallback( const char *cmd, const char *arg, const uint8_t length) {
	uint8_t value = strtol(arg, nullptr, 10);
	status.blink(value);
};

void errorCallback( const char *cmd, const char *arg, const uint8_t length) {
	uint8_t value = strtol(arg, nullptr, 10);
	status.error(value);
};


cmd_t commands[] = {
	{"g", "get", stateCallback, "\tRead LED state"},
	{"f", "flip", flipCallback, "\tFlip status LED"},
	{"1", "on", onCallback, "\tTurn on status LED"},
	{"0", "off", offCallback, "\tTurn off status LED"},
	{"", "blink", blinkCallback, "\tBlink status LED (n) times a sec [1-12]"},
	{"", "error", errorCallback, "\tFlash error code (n) times [1-10]"},
};


void printHelp() {

}


void setup() {
	Serial.begin(115200);
	delay(500);
	console.begin(Serial, commands, sizeof(commands) / sizeof(cmd_t), /*PROMPT*/true);

	console.version = {1, 2, 3};
	// console.setFilter(Level::vvvv);
	console.printHelpCallback = printHelp;


	status.init( 2, true, statusCallback);
	status.blink(4);

	// status.addTask( 0, TASK::BLINK, 1 );
	// status.addTask( 1, TASK::BLINK, 4 );
	// status.addTask( 2, TASK::BLINK, 8 );
	// status.addTask( 3, TASK::BLINK, 16 );

	// delay(5000);
	// status.removeTask( 0 );

	// delay(5000);
	// status.removeTask( 1 );

	// delay(5000);
	// status.removeTask( 2 );

	// delay(5000);
	// status.addTask( 0, TASK::BLINK, 1 );
	// delay(5000);
	// status.removeTask( 0 );

	// status.removeTask( 3 );

	// status.removeAllTasks();

}

void loop() {
	console.update(/*ECHO*/true);
}