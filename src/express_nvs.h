#ifndef __EXPRESS_NVS_H
  #define __EXPRESS_NVS_H
/*
    @file       express_nvs.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       20/06/21
*/
  #define EXPRESS_NVS_VER "1.0.0"

  #include <Arduino.h>

  #include "Configuration.h"

  #ifndef USE_NVS
    #define USE_NVS true
  #endif
  #if (USE_NVS == true)

    #include <string>

    #include "nvs.h"
    #include "nvs_flash.h"
    #define STORAGE_NAMESPACE "storage"

// Singleton design for C++ 11
// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
struct express_nvs {
 public:
  static express_nvs &getInstance() {
    static express_nvs instance;  // Guaranteed to be destroyed.
    return instance;              // Instantiated on first use.
  }

 private:
  express_nvs() {}  // Constructor? (the {} brackets) are needed here.
 public:
  express_nvs(express_nvs const &) = delete;
  void operator=(express_nvs const &) = delete;

  const String version = EXPRESS_NVS_VER;

 private:
  typedef uint32_t nvs_handle_t;


 public:
  void init();

  esp_err_t open(nvs_handle_t *my_handle, esp_err_t *err, const char *key);
  esp_err_t readError(const char *key, esp_err_t *err);
  esp_err_t close(nvs_handle_t *my_handle, esp_err_t *err);

  //   esp_err_t get(const char *key, int16_t *data);
  //   esp_err_t set(const char *key, int16_t *data);

  esp_err_t erase_all();

  esp_err_t get(const char *key, void *data, unsigned int *length);
  esp_err_t set(const char *key, const void *data, unsigned int length);

  esp_err_t get(const char *key, int32_t *data);
  esp_err_t set(const char *key, int32_t *data);
  esp_err_t get(const char *key, uint32_t *data);
  esp_err_t set(const char *key, uint32_t *data);

  esp_err_t get(const char *key, int16_t *data);
  esp_err_t set(const char *key, int16_t *data);
  esp_err_t get(const char *key, uint16_t *data);
  esp_err_t set(const char *key, uint16_t *data);

  esp_err_t get(const char *key, int8_t *data);
  esp_err_t set(const char *key, int8_t *data);
  esp_err_t get(const char *key, uint8_t *data);
  esp_err_t set(const char *key, uint8_t *data);
};


  #endif
#endif

// esp_err_t nvs_get_i8(nvs_handle handle, const char* key, int8_t* out_value);
// esp_err_t nvs_get_u8(nvs_handle handle, const char* key, uint8_t* out_value);
// *esp_err_t nvs_get_i16(nvs_handle handle, const char* key, int16_t* out_value);
// esp_err_t nvs_get_u16(nvs_handle handle, const char* key, uint16_t* out_value);
// *esp_err_t nvs_get_i32(nvs_handle handle, const char* key, int32_t* out_value);
// esp_err_t nvs_get_u32(nvs_handle handle, const char* key, uint32_t* out_value);
// esp_err_t nvs_get_i64(nvs_handle handle, const char* key, int64_t* out_value);
// esp_err_t nvs_get_u64(nvs_handle handle, const char* key, uint64_t* out_value);