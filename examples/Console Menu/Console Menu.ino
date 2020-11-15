#include <express_console_menu.h>
_version version = {0,1,0};

void trigger(const char *arg);
void channel(const char *arg);

uint8_t menu_length = 1;

 cmd_t commands[9] = {
  {"f", "function", trigger, "Trigger the test function"}
};

char* channels[][2] = { 
  {"1", "ch:1"},
  {"2", "ch:2"},
  {"3", "ch:3"},
  {"4", "ch:4"},
  {"5", "ch:5"}
};




void setup() {
  Serial.begin(115200);
  delay(100);

  console.version = version;
  console.setPrinter(Serial);
  console.setFilter(Level::vvvv);
 
 for(uint8_t i=0; i<3; i++){
  append( channels[i][0], channels[i][1], channel, "Toggle output channel");
 }

  console.begin(commands, menu_length);
}

void loop() {
  console.update();
}

void push(char *shortcut, char *command, cmd_action_t action, char *description){
  for(uint8_t i=menu_length; i>0; i-- ){
    console.p(i).p(" = ").pln(i-1);
    commands[i] = commands[i-1];
  }
  menu_length++;
  commands[0] = {shortcut, command, action, description};
}

void append(char *shortcut, char *command, cmd_action_t action, char *description){
  commands[menu_length] = {shortcut, command, action, description};
  menu_length++;
}




void trigger(const char *arg) {
  console.v().p("void trigger(").p(arg).pln(")");
}


void channel(const char *arg) {
   console.v().p("void channel(").p(arg).pln(")");
}
