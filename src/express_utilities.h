#ifndef __EXPRESS_UTILITIES_H
#define __EXPRESS_UTILITIES_H

 enum EXPRESS_TYPE_ENUM { Unknown = 0, Int8 = 1, Int16 = 2, Int32 = 3, Int64 = 4, UInt8 = 5, UInt16 = 6, UInt32 = 7, UInt64 = 8, Float = 9, Double = 10, Boolean = 11 };
  static const char *EXPRESS_TYPE_CHAR[12] = {
      "Unknown",
      "Int8",
      "Int16",
      "Int32",
      "Int64",
      "UInt8",
      "UInt16",
      "UInt32",
      "UInt64",
      "Float",
      "Double",
      "Boolean",
  };

  static union EXPRESS_TYPE_UNION {
    int8_t Int8;
    int16_t Int16;
    int32_t Int32;
    int64_t Int64;
    uint8_t UInt8;
    uint16_t UInt16;
    uint32_t UInt32;
    uint64_t UInt64;
    float Float;
    double Double;
    boolean Boolean;
  } EXPRESS_TYPE_UNION_t;


#endif