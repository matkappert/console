#pragma once
//   #define CONFIGURATION_H_VERSION 010107

/**
 * Serial baudrate :[2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000]
 */
#define BAUDRATE (int)115200

#define DEFAULT_VERBOSITY_LEVEL 3

#define USE_PLOT false
#define DEFAULT_INTERVAL (uint16_t) 30

/**
 *  Use non-volatile storage (NVS, flash) to save settings
 */
#define USE_NVS      true
#define USE_MENU     true
#define MENU_PADDING (uint8_t)4
#define MENU_OFFSET  (uint8_t)30
#define DEFAULT_MENU_ECHO true

/**
 *
 */
#if defined(ESP8266) || defined(ESP8285) || defined(ESP32)

  #define USE_WIFI true

  /**
   *  Use WiFi in station mode (connect to a network) [true]
   *  Or,
   *  use access point mode (create network) [false]
   */
  #define DEFAULT_WIFI_MODE_STATION (boolean) false

// 19.5dBm [78]
// 19dBm [76]
// 18.5dBm [74]
// 17dBm [68]
// 15dBm [60]
// 13dBm [52]
// 11dBm [44]
// 8.5dBm [34]
// 7dBm [28]
// 5dBm [20]
// 2dBm [8]
// -1dBm [-4]
  #define WIFI_MODEM_TX_POWER (int8_t) 78
  #define WIFI_MODEM_SLEEP (boolean) false
  #define WIFI_LONGRANG_MODE (boolean) false

  #define WIFI_CONNECTION_RETRIES 8

  /**
   *  Configures static IP address
   *  Automatic [false]
   *  Static [true]
   */
  #define DEFAULT_WIFI_IS_STATIC_IP (boolean) false

  // clang-format off
  #define  DEFAULT_WIFI_LOCAL_IP_ADDRESS (uint8_t[4]) { 10,0,0,150 }
  #define  DEFAULT_WIFI_GATEWAY_IP_ADDRESS (uint8_t[4]) { 10,0,0,1 }
  #define  DEFAULT_WIFI_SUBNET_MASK (uint8_t[4]) { 255,255,0,0 }

  /** optional */
  #define  DEFAULT_WIFI_PRIMARY_DNS (uint8_t[4]) { 1,1,1,1 }
  #define  DEFAULT_WIFI_SECONDARY_DNS  (uint8_t[4]) { 8,8,8,8 }

  #define DEFAULT_WIFI_SOFTAP_SSID  "EXPRESS"
  #define DEFAULT_WIFI_SOFTAP_PASSWORD  "12345678"
  #define  DEFAULT_WIFI_SOFTAP_LOCAL_IP_ADDRESS (uint8_t[4]) { 10,0,0,1 }
  #define  DEFAULT_WIFI_SOFTAP_GATEWAY_IP_ADDRESS (uint8_t[4]) { 10,0,0,1 }
  #define  DEFAULT_WIFI_SOFTAP_SUBNET_MASK (char[4]) { 255,255,255,0 }

  #define DEFAULT_WIFI_HOST_NAME  "express"
  // clang-format on

#endif
