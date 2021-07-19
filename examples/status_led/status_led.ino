#include <Arduino.h>
#define USE_MENU  false
#define USE_NVS   false
#define USE_WIFI  false
#define USE_CULEX false
#define USE_LED   true

#include "Settings.h"
#if (USE_MENU == true)
  #include "express_console_menu.h"
#else
  #include "express_console.h"
#endif
#if (USE_WIFI == true)
  #include "express_wifi.h"
#endif
#if (USE_CULEX == true)
  #include "express_culex.h"
#endif
#if (USE_PLOT == true)
  #include "express_plot.h"
#endif
#if (USE_LED == true)
  #include "express_status_led.h"
#endif

void setup() {

  eLED.init();

  /************************************************************
      @brief Blink the output pin at a 50/50 ratio

      @param value, Speed value in Hz [rang = 1-16]
      @param detach_timer [default = true]
  *************************************************************/
  eLED.blink(1);
  delay(5000);
  eLED.blink(4);
  delay(5000);
  eLED.blink(8);
  delay(5000);
  eLED.blink(16);
  delay(5000);

  /************************************************************
      @brief Blink an error code, pause & repeat

      @param value, Error code, (n) time to blink [rang = 1-10]
      @param detach_timer [default = true]
  *************************************************************/
  eLED.error(1);
  delay(5000);
  eLED.error(2);
  delay(5000);
  eLED.error(4);
  delay(5000);
  eLED.error(8);
  delay(5000);

  /************************************************************
      @brief Add an LED effect to a task que, 0 = highest priority

      @param priority, Que number [rang = 0-3]
      @param effect, Use flash effect from TASK:: [OFF, BLINK, ERROR]
      @param value, effect value settings
  *************************************************************/
  eLED.addTask(0, TASK::BLINK, 1);
  eLED.addTask(1, TASK::BLINK, 4);
  eLED.addTask(2, TASK::BLINK, 8);
  eLED.addTask(3, TASK::BLINK, 16);

  /************************************************************
      @brief Remove the task and apply the next task

      @param priority, Que number [rang = 0-3]
  *************************************************************/
  delay(5000);
  eLED.removeTask(0);

  delay(5000);
  eLED.removeTask(1);

  delay(5000);
  eLED.removeTask(2);

  delay(5000);
  eLED.removeTask(3);

  /************************************************************
      @brief Remove all the tasks and turn off

  *************************************************************/
  eLED.removeAllTasks();
}

void loop() {
  bool x = eLED.state;
  eLED.on();
  delay(1000);

  eLED.off();
  delay(1000);

  eLED.flip();
  delay(1000);
  eLED.flip();
  delay(1000);
}