/*
    @file       express_nvs.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       19/6/21
*/

#include "Settings.h"
#include "express_nvs.h"

#if (USE_NVS == true)
  #include "express_console_menu.h"

express_nvs eNvs;  // global-scoped variable

// #if defined(using_console_menu) || defined(using_console)
void express_nvs::init() {
  // extern express_console_menu console;
  // _console = &console;

  //   extern express_nvs nvs;
  //   _self = &nvs;

  eMenu.vvvv().pln("express_nvs::init()");

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    eMenu.v().pln("ERROR->express_nvs::init() NVS partition was truncated and needs to be erased");
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // isReady = true;
}

esp_err_t express_nvs::erase_all() {
  nvs_handle_t my_handle;
  esp_err_t err;

  err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_erase_all(my_handle);
  express_nvs::readError("erase_all", &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// Open
esp_err_t express_nvs::open(nvs_handle_t *my_handle, esp_err_t *err, const char *key) {
  *err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, my_handle);
  if (*err != ESP_OK) {
    eMenu.v().p("ERROR->express_nvs::open() ").pln(esp_err_to_name(*err));
  } else {
    eMenu.vvvv().p("express_nvs::open() ").pln(key);
  }
}
// Read Error
esp_err_t express_nvs::readError(const char *key, esp_err_t *err) {
  if (*err == ESP_OK) {
    *err = ESP_OK;
    return *err;
  } else if (*err == ESP_ERR_NVS_NOT_FOUND) {
    eMenu.v().p("ERROR->express_nvs::readError()->").p(key).pln(": The value is not initialized yet!");
    return *err;
  } else {
    eMenu.v().p("ERROR->express_nvs::readError()->").p(key).p(": ").pln(esp_err_to_name(*err));
    return *err;
  }
}

// Close
esp_err_t express_nvs::close(nvs_handle_t *my_handle, esp_err_t *err) {
  nvs_close(*my_handle);
  *err = ESP_OK;
}







// GET blob
esp_err_t express_nvs::get(const char *key, void *data, unsigned int *length) {
  nvs_handle_t my_handle;
  esp_err_t err;

  eMenu.vvvv().p("express_nvs::get(blob)->length:").pln((unsigned int)length);

  express_nvs::open(&my_handle, &err, key);

  // esp_err_t nvs_get_blob(nvs_handle handle, const char *key, void *out_value, size_t *length);
  err = nvs_get_blob(my_handle, key, data, length);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// SET blob
esp_err_t express_nvs::set(const char *key, const void *data, unsigned int length) {
  nvs_handle_t my_handle;
  esp_err_t err;

  eMenu.vvvv().p("express_nvs::set(blob)->length:").pln((unsigned int)length);

  express_nvs::open(&my_handle, &err, key);

  err = nvs_set_blob(my_handle, key, data, length);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// GET int32_t
esp_err_t express_nvs::get(const char *key, int32_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_get_i32(my_handle, key, data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// SET int32_t
esp_err_t express_nvs::set(const char *key, int32_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_set_i32(my_handle, key, *data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// GET uint32_t
esp_err_t express_nvs::get(const char *key, uint32_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_get_u32(my_handle, key, data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// SET uint32_t
esp_err_t express_nvs::set(const char *key, uint32_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_set_u32(my_handle, key, *data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// GET int16_t
esp_err_t express_nvs::get(const char *key, int16_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_get_i16(my_handle, key, data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// SET int16_t
esp_err_t express_nvs::set(const char *key, int16_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_set_i16(my_handle, key, *data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// GET uint16_t
esp_err_t express_nvs::get(const char *key, uint16_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_get_u16(my_handle, key, data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// SET uint16_t
esp_err_t express_nvs::set(const char *key, uint16_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_set_u16(my_handle, key, *data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// GET int8_t
esp_err_t express_nvs::get(const char *key, int8_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_get_i8(my_handle, key, data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// SET int8_t
esp_err_t express_nvs::set(const char *key, int8_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_set_i8(my_handle, key, *data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// GET uint8_t
esp_err_t express_nvs::get(const char *key, uint8_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_get_u8(my_handle, key, data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

// SET uint8_t
esp_err_t express_nvs::set(const char *key, uint8_t *data) {
  nvs_handle_t my_handle;
  esp_err_t err;

  express_nvs::open(&my_handle, &err, key);

  err = nvs_set_u8(my_handle, key, *data);
  express_nvs::readError(key, &err);
  if (err != ESP_OK) {
    return err;
  }

  express_nvs::close(&my_handle, &err);
  return err;
}

#endif