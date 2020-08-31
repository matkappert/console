/*
   Thanks to klenov
   advancedSerial Library - Advanced Example
   https://github.com/klenov/advancedSerial
*/

#include <console.h>

void printHelpCmd(const char *arg);
void setOutputCmd(const char *arg);
void getInputCmd(const char *arg);

const cmd_t commands[] = {
  {"help", printHelpCmd},
  {"?", printHelpCmd},
  {"setOutput", setOutputCmd},
  {"1", setOutputCmd},
  {"getInput", getInputCmd},
};



void setup() {
  Serial.begin(115200);

  console.setPrinter(Serial);
  console.setFilter(Level::vv);
  /* Uncomment the following line to disable the output. By defalut the ouput is on. */
  // console.off();

  console.begin(commands, sizeof(commands));
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


void setOutputCmd(const char *arg) {
//  if (strcmp(arg, "HIGH") == 0) {
//    digitalWrite(Q0_0, HIGH);
//    Serial.println("Q0.0 set to HIGH");
//  } else if (strcmp(arg, "LOW") == 0) {
//    digitalWrite(Q0_0, LOW);
//    Serial.println("Q0.0 set to LOW");
//  }
console.v().p("setOutputCmd").pln(arg);
}


void getInputCmd(const char *arg) {
//  int value = digitalRead(I0_0);
//  if (value == HIGH) {
//    Serial.println("I0.0 is HIGH");
//  } else {
   console.v().p("getInputCmd").pln(arg);
//  }
}
