/*
   THANKS TO:
   advancedSerial Library - Basic Example
   https://github.com/klenov/advancedSerial
*/

#include <express_console.h>
express_console console = express_console();

int counter = 0;

void setup() {
  Serial.begin(115200);

  console.init(Serial);
  /* Uncomment the following line to disable the output. By defalut the ouput is on. */
  // console.off();
}

void loop() {

  // you can chain print() and println() methods
  console.print("counter = ").println(counter);

  // also short method names are available
  console.p("counter = ").pln(counter);

  console.pln().pln();

  counter++;
  delay(3000);
}
