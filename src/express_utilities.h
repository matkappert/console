#ifndef __EXPRESS_UTILITIES_H
#define __EXPRESS_UTILITIES_H
#include <Arduino.h>

struct express_utilities;
extern express_utilities eUtil;

enum EXPRESS_TYPE_ENUM { Unknown = 0, Int8, Int16, Int32, /*Int64,*/ UInt8, UInt16, UInt32, /*UInt64,*/ Float, Double, Boolean };
static const char *EXPRESS_TYPE_CHAR[12] = {
    "Unknown",
    "Int8",
    "Int16",
    "Int32",
    // "Int64",
    "UInt8",
    "UInt16",
    "UInt32",
    // "UInt64",
    "Float",
    "Double",
    "Boolean",
};
static union EXPRESS_TYPE_UNION {
  int8_t Int8;
  int16_t Int16;
  int32_t Int32;
  // int64_t Int64;
  uint8_t UInt8;
  uint16_t UInt16;
  uint32_t UInt32;
  // uint64_t UInt64;
  float Float;
  double Double;
  boolean Boolean;
} EXPRESS_TYPE_UNION_t;

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
int rom_phy_get_vdd33();
// float voltage = ((float)rom_phy_get_vdd33()) / 1000;
#ifdef __cplusplus
}
#endif

struct express_utilities {

  float getVoltage() {
    return ((float)rom_phy_get_vdd33()) / 1000;
  }
  
  float getTemperature(){
    return (temprature_sens_read() - 32) / 1.8;
  }
};

// express_utilities eUtil;

#endif