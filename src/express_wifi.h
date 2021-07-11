#pragma once
/*
    @file       express_wifi.h
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       14/11/20
*/
#define EXPRESS_CONSOLE_WIFI_VER "2.3.0"

// https://techtutorialsx.com/2021/01/04/esp32-soft-ap-and-station-modes/
// https://randomnerdtutorials.com/solved-reconnect-esp32-to-wifi/
// https://randomnerdtutorials.com/esp32-set-custom-hostname-arduino/
// https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/#11
#include <Arduino.h>

#include "Settings.h"

/*
 * Forward-Declarations
 */
struct express_wifi;
extern express_wifi eWifi;

#ifndef USE_WIFI
  #define USE_WIFI true
#endif
#if (USE_WIFI == true)

  #if (USE_MENU == true)
    #include "express_console_menu.h"
  #endif
  // #include <unordered_map>
  #include <vector>
using std::vector;

  #include <WiFi.h>

  // #include "express_status_led.h"

  #if defined(ESP8266) || defined(ESP8285)
    #include <ESP8266WiFi.h>
  #elif defined(ESP32)
    #include <WiFi.h>
  #endif

const int MAX_SSID = 32;  // max number of characters in WiFi SSID
const int MAX_PWD  = 64;  // max number of characters in WiFi Password

// IPAddress local_IP( DEFAULT_WIFI_LOCAL_IP_ADDRESS[0],  DEFAULT_WIFI_LOCAL_IP_ADDRESS[1],  DEFAULT_WIFI_LOCAL_IP_ADDRESS[2],  DEFAULT_WIFI_LOCAL_IP_ADDRESS[3]);
// IPAddress gateway(DEFAULT_WIFI_GATEWAY_IP_ADDRESS[0], DEFAULT_WIFI_GATEWAY_IP_ADDRESS[1], DEFAULT_WIFI_GATEWAY_IP_ADDRESS[2], DEFAULT_WIFI_GATEWAY_IP_ADDRESS[3]);

// IPAddress subnet(255, 255, 0, 0);
// IPAddress primaryDNS(8, 8, 8, 8);    // optional
// IPAddress secondaryDNS(8, 8, 4, 4);  // optional

// Singleton design for C++ 11
// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
struct express_wifi {
//  public:
//   typedef void (*callback_t)(void);

//   typedef void (*event)(system_event_id_t event, system_event_info_t info);
//   // event EVENT_STA_CONNECTED;

//   static express_wifi &getInstance() {
//     static express_wifi instance;  // Guaranteed to be destroyed.
//     return instance;               // Instantiated on first use.
//   }

//  private:
//   express_wifi() {}  // Constructor? (the {} brackets) are needed here.
//  public:
//   express_wifi(express_wifi const &) = delete;
//   void operator=(express_wifi const &) = delete;

 private:
  typedef uint32_t nvs_handle_t;

 public:
  struct wifiData_t {
    char ssid[MAX_SSID + 1] = "\0";
    char pwd[MAX_PWD + 1]   = "\0";
    // Static STA
    IPAddress local_IP      = {DEFAULT_WIFI_LOCAL_IP_ADDRESS[0], DEFAULT_WIFI_LOCAL_IP_ADDRESS[1], DEFAULT_WIFI_LOCAL_IP_ADDRESS[2], DEFAULT_WIFI_LOCAL_IP_ADDRESS[3]};
    IPAddress gateway_IP    = {DEFAULT_WIFI_GATEWAY_IP_ADDRESS[0], DEFAULT_WIFI_GATEWAY_IP_ADDRESS[1], DEFAULT_WIFI_GATEWAY_IP_ADDRESS[2], DEFAULT_WIFI_GATEWAY_IP_ADDRESS[3]};
    IPAddress subnet_mask   = {DEFAULT_WIFI_SUBNET_MASK[0], DEFAULT_WIFI_SUBNET_MASK[1], DEFAULT_WIFI_SUBNET_MASK[2], DEFAULT_WIFI_SUBNET_MASK[3]};
    IPAddress primary_DNS   = {DEFAULT_WIFI_PRIMARY_DNS[0], DEFAULT_WIFI_PRIMARY_DNS[1], DEFAULT_WIFI_PRIMARY_DNS[2], DEFAULT_WIFI_PRIMARY_DNS[3]};
    IPAddress secondary_DNS = {DEFAULT_WIFI_SECONDARY_DNS[0], DEFAULT_WIFI_SECONDARY_DNS[1], DEFAULT_WIFI_SECONDARY_DNS[2], DEFAULT_WIFI_SECONDARY_DNS[3]};
    boolean is_static_IP    = DEFAULT_WIFI_IS_STATIC_IP;
    boolean is_station_mode = DEFAULT_WIFI_MODE_STATION;
    // Soft AP
    char softap_ssid[MAX_SSID + 1] = DEFAULT_WIFI_SOFTAP_SSID;
    char softap_pwd[MAX_PWD + 1]   = DEFAULT_WIFI_SOFTAP_PASSWORD;
    IPAddress softap_local_IP      = {DEFAULT_WIFI_SOFTAP_LOCAL_IP_ADDRESS[0], DEFAULT_WIFI_SOFTAP_LOCAL_IP_ADDRESS[1], DEFAULT_WIFI_SOFTAP_LOCAL_IP_ADDRESS[2], DEFAULT_WIFI_SOFTAP_LOCAL_IP_ADDRESS[3]};
    IPAddress softap_gateway_IP    = {DEFAULT_WIFI_SOFTAP_GATEWAY_IP_ADDRESS[0], DEFAULT_WIFI_SOFTAP_GATEWAY_IP_ADDRESS[1], DEFAULT_WIFI_SOFTAP_GATEWAY_IP_ADDRESS[2], DEFAULT_WIFI_SOFTAP_GATEWAY_IP_ADDRESS[3]};
    IPAddress softap_subnet_mask   = {DEFAULT_WIFI_SOFTAP_SUBNET_MASK[0], DEFAULT_WIFI_SOFTAP_SUBNET_MASK[1], DEFAULT_WIFI_SOFTAP_SUBNET_MASK[2], DEFAULT_WIFI_SOFTAP_SUBNET_MASK[3]};
    char host_name[MAX_SSID + 1]   = DEFAULT_WIFI_HOST_NAME;
  };
  wifiData_t wifiData;

 private:
  uint8_t connection_retries = 0;

 public:
  WiFiClass *_WiFi;
  const String version = EXPRESS_CONSOLE_WIFI_VER;

  vector<menu_item *> WIFI_MENU;

  char **ssidList = NULL;
  int numSSID;

  static void wifi_event(WiFiEvent_t event, WiFiEventInfo_t info);
  void wifi_event_disconnected();

  void init(WiFiClass *WiFi);

  void default_settings();
  void print_settings();

  void establish_connection();

  void save_settings();

  // scan for WiFi networks and save only those with unique SSIDs
  void scan();

  struct menu_wifi_ssid;
  struct menu_wifi_password;
  struct menu_wifi_info;
  struct menu_wifi_reconnect;
  struct menu_wifi_defaults;
  struct menu_wifi_sta;
  struct menu_wifi_static;
  struct menu_wifi_local_ip;
  struct menu_wifi_gatway_ip;
  struct menu_wifi_subnetmask;
  struct menu_wifi_primaryDNS;
  struct menu_wifi_secondaryDNS;
  struct menu_wifi_softap_local_ip;
  struct menu_wifi_softap_gatway_ip;
  struct menu_sub_wifi;




  const char *system_event_id_cstr[21] = {
      "WIFI_READY",             /**< ESP32 WiFi ready */
      "SCAN_DONE",              /**< ESP32 finish scanning AP */
      "STA_START",              /**< ESP32 station start */
      "STA_STOP",               /**< ESP32 station stop */
      "STA_CONNECTED",          /**< ESP32 station connected to AP */
      "STA_DISCONNECTED",       /**< ESP32 station disconnected from AP */
      "STA_AUTHMODE_CHANGE",    /**< the auth mode of AP connected by ESP32 station changed */
      "STA_GOT_IP",             /**< ESP32 station got IP from connected AP */
      "STA_LOST_IP",            /**< ESP32 station lost IP and the IP is reset to 0 */
      "STA_WPS_ER_SUCCESS",     /**< ESP32 station wps succeeds in enrollee mode */
      "STA_WPS_ER_FAILED",      /**< ESP32 station wps fails in enrollee mode */
      "STA_WPS_ER_TIMEOUT",     /**< ESP32 station wps timeout in enrollee mode */
      "STA_WPS_ER_PIN",         /**< ESP32 station wps pin code in enrollee mode */
      "STA_WPS_ER_PBC_OVERLAP", /*!< ESP32 station wps overlap in enrollee mode */
      "AP_START",               /**< ESP32 soft-AP start */
      "AP_STOP",                /**< ESP32 soft-AP stop */
      "AP_STACONNECTED",        /**< a station connected to ESP32 soft-AP */
      "AP_STADISCONNECTED",     /**< a station disconnected from ESP32 soft-AP */
      "AP_STAIPASSIGNED",       /**< ESP32 soft-AP assign an IP to a connected station */
      "AP_PROBEREQRECVED",      /**< Receive probe request packet in soft-AP interface */
      "GOT_IP6",                /**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
  };

  const char *wl_status_cstr[8] = {
      "WL_IDLE_STATUS", // = 0
      "WL_NO_SSID_AVAI",
      "WL_SCAN_COMPLETED",
      "WL_CONNECTED",
      "WL_CONNECT_FAILED",
      "WL_CONNECTION_LOST",
      "WL_DISCONNECTED",
      "WL_NO_SHIELD", // = 255
  };
};
#endif
