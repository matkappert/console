#include "express_console_menu.h"
// #include "express_plot.h"
// express_console_menu console = express_console_menu();
// express_plot plot            = express_plot();

float angle_counter    = 0;
float square_counter   = 25;
int triangular_counter = 50;

float sina       = 0.00;
float cosa       = 0.00;
float sawtooth   = 75.00;
float square     = 0.00;
float triangular = 0.00;

void print_plot() {
  if (angle_counter <= 100) {
    angle_counter += 0.1;
    sina = sin(angle_counter);
    cosa = cos(angle_counter);
  } else {
    angle_counter = 0;
  }

  // Sawtooth Wave
  if (sawtooth < 100) {
    sawtooth++;
  } else {
    sawtooth = 0;
  }

  // Square Wave
  if (square_counter <= 100) {
    square_counter++;
    square = 0;
  } else if (square_counter <= 200) {
    square_counter++;
    square = 100;
  } else {
    square_counter = 0;
  }

  //   Triangular wave
  if (triangular_counter < 100) {
    triangular_counter++;
    triangular++;
  } else if (triangular_counter < 200) {
    triangular_counter++;
    triangular--;
  } else {
    triangular_counter = 0;
    triangular         = 0;
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  eMenu.init(Serial);

  // console.init(console, Serial, nullptr, 0, /*PROMPT*/ true);
  // plot.init(plot, console, 1000);
  ePlot.init();

  // console.MENU_POINTER.assign(console.MENU_ITEMS.begin(),
  //                             console.MENU_ITEMS.end());
  

  ePlot.add("sina", &sina, -50.0, 50.0);
  ePlot.add("cosa", &cosa, -50.0, 50.0);
  ePlot.add("sawtooth", &sawtooth);
  ePlot.add("square", &square, +100.0);
  ePlot.add("triangular", &triangular, +200.0);
  ePlot.callback(print_plot);
  eMenu.MENU_POINTER.assign(eMenu.MENU_ITEMS.begin(), eMenu.MENU_ITEMS.end());
}

void loop() {
  eMenu.update();
  ePlot.update();
  if (triangular_counter == 0) {
    ePlot.interrupt();
  }
}