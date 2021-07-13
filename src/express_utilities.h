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

struct express_utilities {

  String valueToBuffer(EXPRESS_TYPE_ENUM type, EXPRESS_TYPE_UNION *value) {
    char buffer[(20*sizeof( char ))+1]; 
    if (type == Int8) {
      itoa(value->Int8, buffer, 10);
    } else if (type == Int16) {
      itoa(value->Int16, buffer, 10);
    } else if (type == Int32) {
      itoa(value->Int32, buffer, 10);
    // } else if (type == Int64) {
    //   itoa(value->Int64, buffer, 10);
    } else if (type == UInt8) {
      utoa(value->UInt8, buffer, 10);
    } else if (type == UInt16) {
      utoa(value->UInt16, buffer, 10);
    } else if (type == UInt32) {
      utoa(value->UInt32, buffer, 10);
    // } else if (type == UInt64) {
    //   utoa(value->UInt64, buffer, 10);
    } else if (type == Float) {
      dtostrf(value->Float, 6, 6, buffer);
    } else if (type == Double) {
      dtostrf(value->Double, 6, 6, buffer);
    } else if (type == Boolean) {
      sprintf(buffer, "%s", value->Boolean ? "TRUE" : "FALSE");
    } 
    return String(buffer);
  }
};

// express_utilities eUtil;

#endif