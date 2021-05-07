/*
    @file       express_wifi.cpp
    @author     matkappert
    @repo       github.com/matkappert/express_wifi
    @version    V1.0.0
    @date       14/11/20
    @format     C++=""-A14-xn-xc-xl-xk-xV-T4-C-N-xW-w-Y-p-xg-H-W0-xb-j-xf-xh-c-xp-xC200""
*/

#include "express_wifi.h"


/************************************************************
    @brief Create an express_wifi instance

    @param callback, used for the triggers
*************************************************************/
void express_wifi::init() {
	// self = _callback;
	if (console) {
		console->vvvv().p("wifi init...");
		console->vvvv().pln("done");
	}
	readSettings();
}

void express_wifi::update(){

}

// void express_wifi::ssid(char* _ssid) {
//  wifi_data.ssid = _ssid;
// }
void express_wifi::connect(char* ssid, char* pwd) {
	wifi_data.ssid = ssid;
	wifi_data.pwd = pwd;
	connect();
}

void express_wifi::connect() {

	// if (!wifi_params) {
	//  console.v().pln("\n#SYSTEM HALTED! No WiFi settings.");
	//  while (true) {
	//      console.update();
	//  }
	// } else {
	console->vvv().pln().p("Connecting to SSID: '").p((char *)wifi_data.ssid).pln("'");
	WiFi.mode(WIFI_STA);

	status->addTask( 0, TASK::BLINK, 10 );
	status->blink(10 );

	WiFi.begin((char *)wifi_data.ssid, (char *)wifi_data.pwd);
	// Loop until we're connected
	while (WiFi.status() != WL_CONNECTED) {
		console->update();
		delay(250);
		console->vvvv().p(".");
		// @TODO: add timeout to reboot
	}
	status->removeTask( 0 );
	status->off();
	console->vvvv().pln();
	console->vvv().pln("WiFi connected.").p("IP address: ").pln(WiFi.localIP()).pln();

}
