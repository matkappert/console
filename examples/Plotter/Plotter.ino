#include "express_console_menu.h"


float angle_counter    = 0;
int square_counter     = 0;
int triangular_counter = 0;

float sina       = 0.00;
float cosa       = 0.00;
uint8_t sawtooth   = 75.00;
uint8_t square     = 0.00;
uint8_t triangular = 0.00;

// struct plot_sina_t : PLOT_STRUCT {
//   plot_sina_t() : PLOT_STRUCT({(char *)"sina", (EXPRESS_TYPE_ENUM)Float}) {
//     this->value.Float = &sina;
//     this->offset      = -50;
//     this->multiplier  = 50;
//   }
// } plot_sina;

struct plot_cosa_t : PLOT_STRUCT {
  plot_cosa_t() : PLOT_STRUCT({(char *)"cosa", (EXPRESS_TYPE_ENUM)Float}) {
    this->value.Float = &cosa;
    this->offset      = -50;
    this->multiplier  = 50;
  }
} plot_cosa;

struct plot_sawtooth_t : PLOT_STRUCT {
  plot_sawtooth_t() : PLOT_STRUCT({(char *)"sawtooth", (EXPRESS_TYPE_ENUM)UInt8}) {
    this->value.UInt8 = &sawtooth;
  }
} plot_sawtooth;

struct plot_square_t : PLOT_STRUCT {
  plot_square_t() : PLOT_STRUCT({(char *)"square", (EXPRESS_TYPE_ENUM)UInt8}) {
    this->value.UInt8 = &square;
    this->offset      = 100;
  }
} plot_square;

// struct plot_triangular_t : PLOT_STRUCT {
//   plot_triangular_t() : PLOT_STRUCT({(char *)"triangular", (EXPRESS_TYPE_ENUM)UInt8}) {
//     this->value.UInt8 = &triangular;
//     this->offset      = 200;
//   }
// } plot_triangular;

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
  if (square_counter < 100) {
    square_counter++;
    square = 0;
  } else if (square_counter < 200) {
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
  ePlot.init();

  // console.MENU_POINTER.assign(console.MENU_ITEMS.begin(),
  //                             console.MENU_ITEMS.end());

  ePlot.add("sina", &sina, -50.0, 50.0);
  // ePlot.add("cosa", &cosa, -50.0, 50.0);
  // ePlot.add("sawtooth", &sawtooth);
  // ePlot.add("square", &square, +100.0);
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