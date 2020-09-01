/*
   Thanks to klenov
   advancedSerial Library - Advanced Example
   https://github.com/klenov/advancedSerial
*/

#include <console.h>

void printHelpCmd(const char *arg);
void toggle(const char *arg);
void set(const char *arg);

// --ssid="xxx" --password="xxx" --reboot
// -s xxx

#define menu_length 3
const cmd_t commands[] = {
  {"?", "help", printHelpCmd},
  {"t", "toggle", toggle},
  {"s", "set", set}
};



void setup() {
  Serial.begin(115200);
  delay(100);

  
  console.setPrinter(Serial);
  console.setFilter(Level::vvvv);
  /* Uncomment the following line to disable the output. By defalut the ouput is on. */
  // console.off();
  console.v().pln(".............");
  // console.vvvv().pln("Compiled: ").p(__FILE__).p(", ").pln(__VERSION__);
  console.p(__DATE__).p(" - ").pln(__TIME__);
  console.begin(commands, menu_length);


}

void loop() {
  // Update cmdline often to process commands written in the Serial port
  console.update();

}


void printHelpCmd(const char *arg) {
  console.v().pln("List of commands:");
  console.v().pln("help                    print this help");
  console.v().pln("setOutput HIGH|LOW      set Q0.0 to HIGH or LOW");
  console.v().pln("getInput                get I0.0 digital value");
}


void toggle(const char *arg) {
//  if (strcmp(arg, "HIGH") == 0) {
//    digitalWrite(Q0_0, HIGH);
//    Serial.println("Q0.0 set to HIGH");
//  } else if (strcmp(arg, "LOW") == 0) {
//    digitalWrite(Q0_0, LOW);
//    Serial.println("Q0.0 set to LOW");
//  }
console.v().p("void toggle(").p(arg).pln(")");
}


void set(const char *arg) {
//  int value = digitalRead(I0_0);
//  if (value == HIGH) {
//    Serial.println("I0.0 is HIGH");
//  } else {
   console.v().p("void set(").p(arg).pln(")");
//  }
}
