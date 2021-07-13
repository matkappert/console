#ifndef __EXPRESS_CULEX_H
  #define __EXPRESS_CULEX_H
  /*
      @file       express_culex.h
      @author     matkappert
      @repo       github.com/matkappert/express
      @date       11/07/21
  */
  #define EXPRESS_CULEX_VER "1.0.0"
  /**
   * @ref https://pubsubclient.knolleary.net/api
   */
  #include <Arduino.h>

  #include "express_utilities.h"
  #include "settings.h"
  #define ARDUINOJSON_USE_LONG_LONG 1
  #include <ArduinoJson.h>

/*
 * Forward-Declarations
 */
struct express_culex;
extern express_culex eCulex;

  #if (USE_MENU == true)
    #include "express_console_menu.h"
  #endif

  // https://www.eclipse.org/tahu/spec/Sparkplug%20Topic%20Namespace%20and%20State%20ManagementV2.2-with%20appendix%20B%20format%20-%20Eclipse.pdf

  // https://www.dfrobot.com/blog-1161.html
  // https://www.dfrobot.com/blog-1177.html
  #include <PubSubClient.h>
  #include <WiFi.h>

enum CULEX_TOPICS_ENUM { NBIRTH = 0, NDEATH, DBIRTH, DDEATH, NDATA, DDATA, NCMD, DCMD, STATE, DATA };
static const char *CULEX_TOPICS_CHAR[] = {
    "NBIRTH",  // Birth certificate for MQTT EoN nodes.
    "NDEATH",  // Death certificate for MQTT EoN nodes.
    "DBIRTH",  // Birth certificate for Devices.
    "DDEATH",  // Death certificate for Devices.
    "NDATA",   // Node data message.
    "DDATA",   // Device data message.
    "NCMD",    // Node command message.
    "DCMD",    // Device command message.
    "STATE",   // Critical application state message.
    "DATA",    //  data message.
};
static const char *MQTT_CONNECTION_ERRORS_CHAR[] = {
    "MQTT_CONNECTION_TIMEOUT",       // [-4] the server didn't respond within the keepalive time
    "MQTT_CONNECTION_LOST",          // [-3] the network connection was broken
    "MQTT_CONNECT_FAILED",           // [-2] the network connection failed
    "MQTT_DISCONNECTED",             // [-1] the client is disconnected cleanly
    "MQTT_CONNECTED",                // [0] the client is connected
    "MQTT_CONNECT_BAD_PROTOCOL",     // [1] the server doesn't support the requested version of MQTT
    "MQTT_CONNECT_BAD_CLIENT_ID",    // [2] the server rejected the client identifier
    "MQTT_CONNECT_UNAVAILABLE",      // [3] the server was unable to accept the connection
    "MQTT_CONNECT_BAD_CREDENTIALS",  // [4] the username/password were rejected
    "MQTT_CONNECT_UNAUTHORIZED",     // [5] the client was not authorized to connect
};

struct CULEX_TRANSPORT;

struct express_culex {
 private:
  WiFiClass *_WiFi;
  WiFiClient _clientTcpCulex;
  PubSubClient culexClient = PubSubClient(_clientTcpCulex);
  // char payload_buf[CULEX_PAYLOAD_SIZE];

  // connection
  boolean isConnected              = false;
  uint32_t connection_timeout_last = 0;
  uint16_t connection_timeout_wait = CULEX_CONNECTION_WAIT;

 public:
  vector<CULEX_TRANSPORT *> CULEX_TRANSPORT_VECTORS;
  uint32_t timestamp = 0;

 public:
  void init(WiFiClass *WiFi);
  void update();
  boolean connect();
  void disconnect();
  //   void topic(char *buf, CULEX_TOPICS_ENUM topic);
  // void payload(char *buf);
  // void metric(char *buf, CULEX_TRANSPORT *transport);
  // void birth();
 public:
  StaticJsonDocument<CULEX_PAYLOAD_SIZE> doc;

 public:
  static void STATIC_DATA_EVENT(const char *topic, byte *payload, unsigned int length) {
    eCulex.DATA_EVENT(topic, payload, length);
  };
  void DATA_EVENT(const char *topic, byte *payload, unsigned int length);

  void generateTopics();
  // void generatePayload();
  void postTopics();
  //   String valueToBuffer(char *buffer, EXPRESS_TYPE_ENUM type, EXPRESS_TYPE_UNION *value);

  void typeToValue(EXPRESS_TYPE_UNION *value, const char *type, JsonObject objectValue, EXPRESS_TYPE_ENUM shouldBeType = Unknown, const char *key = "value");

};

/**
 * Culex transport templet.
 *
 * @callback sub //! update
 * @param name the topic name.
 * @param type the topic name.
 */
struct CULEX_TRANSPORT {
  const char *name;
  char topic[CULEX_TOPIC_SIZE];
  EXPRESS_TYPE_ENUM type;
  EXPRESS_TYPE_UNION value;
  boolean readOnly = false;
  //   boolean POST = false;

  CULEX_TRANSPORT(char *name, EXPRESS_TYPE_ENUM type) {
    this->name = name;
    this->type = type;
    eCulex.CULEX_TRANSPORT_VECTORS.push_back(this);
  }
  virtual void callback(JsonObject objectValue, const char * type) {}
  virtual void publish() {
    Serial.println("publish");

  }
};

#endif

// /*
//   Important to not set vTaskDelay to less then 10. Errors begin to develop with the MQTT and network connection.
//   makes the initial wifi/mqtt connection and works to keeps those connections open.
// */
// void MQTTkeepalive( void *pvParameters )
// {
//   sema_MQTT_KeepAlive   = xSemaphoreCreateBinary();
//   xSemaphoreGive( sema_MQTT_KeepAlive ); // found keep alive can mess with a publish, stop keep alive during publish
//   MQTTclient.setKeepAlive( 90 ); // setting keep alive to 90 seconds makes for a very reliable connection, must be set before the 1st connection is made.
//   TickType_t xLastWakeTime = xTaskGetTickCount();
//   const TickType_t xFrequency = 250; //delay for ms
//   for (;;)
//   {
//     //check for a is-connected and if the WiFi 'thinks' its connected, found checking on both is more realible than just a single check
//     if ( (wifiClient.connected()) && (WiFi.status() == WL_CONNECTED) )
//     {
//       xSemaphoreTake( sema_MQTT_KeepAlive, portMAX_DELAY ); // whiles MQTTlient.loop() is running no other mqtt operations should be in process
//       MQTTclient.loop();
//       xSemaphoreGive( sema_MQTT_KeepAlive );
//     }
//     else {
//       log_i( "MQTT keep alive found MQTT status % s WiFi status % s", String(wifiClient.connected()), String(WiFi.status()) );
//       if ( !(wifiClient.connected()) || !(WiFi.status() == WL_CONNECTED) )
//       {
//         connectToWiFi();
//       }
//       connectToMQTT();
//     }
//     xLastWakeTime = xTaskGetTickCount();
//     vTaskDelayUntil( &xLastWakeTime, xFrequency );
//   }
//   vTaskDelete ( NULL );
// }