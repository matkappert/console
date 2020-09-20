#include <console.h>




void callback(const char *cmd, const char *arg, const uint8_t length );
cmd_t commands[] = {
	{"1", "one", callback, "\tfirst menu item."},
};

void setup() {

	/***********************
	    Serial Console.
	************************/
	Serial.begin(115200);
	// console.version = version;
	console.setPrinter(Serial);
	console.setFilter(Level::vvv);  // The filtering threshold is set to Level::vv
	console.begin(commands, 1);

	console.includeWifi();
	delay(1000);


}

void loop() {
	console.update();
}


void callback( const char *cmd, const char *arg, const uint8_t length) {

}