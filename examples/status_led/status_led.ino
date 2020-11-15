#include "express_status_led.h"
express_status_led status = express_status_led();

void statusCallback() {
	status.callback();
};


void setup() {


	/************************************************************
	    @brief Create an status_led instance

	    @param led_pin, Microcontroller GPIO pin (LED)
	    @param led_inverted, Invert the output pin
	    @param callback, used for the timmer trigger
	*************************************************************/
	#if defined(ESP32)
	status.init( 2, false, statusCallback);
	#else
	status.init( LED_BUILTIN, false, statusCallback);
	#endif


	/************************************************************
	    @brief Blink the output pin at a 50/50 ratio

	    @param value, Speed value in Hz [rang = 1-16]
	    @param detach_timer [default = true]
	*************************************************************/
	status.blink(1);
	delay(5000);
	status.blink(4);
	delay(5000);
	status.blink(8);
	delay(5000);
	status.blink(16);
	delay(5000);


	/************************************************************
	    @brief Blink an error code, pause & repeat

	    @param value, Error code, (n) time to blink [rang = 1-10]
	    @param detach_timer [default = true]
	*************************************************************/
	status.error(1);
	delay(5000);
	status.error(2);
	delay(5000);
	status.error(4);
	delay(5000);
	status.error(8);
	delay(5000);


	/************************************************************
	    @brief Add an LED effect to a task que, 0 = highest priority

	    @param priority, Que number [rang = 0-3]
	    @param effect, Use flash effect from TASK:: [OFF, BLINK, ERROR]
	    @param value, effect value settings
	*************************************************************/
	status.addTask( 0, TASK::BLINK, 1 );
	status.addTask( 1, TASK::BLINK, 4 );
	status.addTask( 2, TASK::BLINK, 8 );
	status.addTask( 3, TASK::BLINK, 16 );


	/************************************************************
	    @brief Remove the task and apply the next task 

	    @param priority, Que number [rang = 0-3]
	*************************************************************/
	delay(5000);
	status.removeTask( 0 );

	delay(5000);
	status.removeTask( 1 );

	delay(5000);
	status.removeTask( 2 );

	delay(5000);
	status.removeTask( 3 );


	/************************************************************
	    @brief Remove all the tasks and turn off

	*************************************************************/
	status.removeAllTasks();
}

void loop() {
	bool x = status.state;
	status.on();
	delay(1000);

	status.off();
	delay(1000);

	status.flip();
	delay(1000);
	status.flip();
	delay(1000);
}