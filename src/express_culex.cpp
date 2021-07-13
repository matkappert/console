/*
    @file       express_culex.cpp
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       11/07/21
*/

#include "express_culex.h"

#include "Settings.h"

express_culex eCulex;  // global-scoped variable

// void express_culex::STATIC_DATA_EVENT(const char *topic, byte *payload, unsigned int length) {
//   eCulex.DATA_EVENT(topic, payload, length);
// }

void express_culex::typeToValue(EXPRESS_TYPE_UNION *value, const char *type, JsonObject objectValue, EXPRESS_TYPE_ENUM shouldBeType, const char *key) {
  if (shouldBeType != Unknown) {
    if (strcmp(type, EXPRESS_TYPE_CHAR[shouldBeType]) != 0) {
      eMenu.debug(__func__).p("unexpected type:").pln(type);
      return;
    }
  }

  if (strcmp(type, EXPRESS_TYPE_CHAR[UInt8]) == 0) {
    value->UInt8 = objectValue[key].as<uint8_t>();
  } else {
    eMenu.debug(__func__).p("unkown type:").pln(type);
  }
}

void express_culex::DATA_EVENT(const char *topic, byte *payload, unsigned int length) {
  DeserializationError error = deserializeJson(doc, (const byte *)payload, length);
  if (error) {
    eMenu.error(__func__).p("deserializeJson").pln(error.f_str());
    return;
  }
  // uint64_t timestamp = doc["timestamp"].as<unsigned long long>(); // TODO update system time with NTP
  JsonArray arr = doc["metrics"].as<JsonArray>();
  for (JsonVariant metric : arr) {
    JsonObject metricObject = metric.as<JsonObject>();
    const char *name        = metricObject["name"].as<const char *>();
    const char *type        = metricObject["dataType"].as<const char *>();

    eMenu.debug(__func__).p("searching for transport:").p(name).p(" ... ");
    for (auto &transport : CULEX_TRANSPORT_VECTORS) {
      if (strcmp(name, transport->name) == 0) {
        eMenu.vvvv().pln("FOUND");
        transport->callback(metricObject, type);
        break;
      }
    }
  }
}

// replace TEST
struct trans_TEST_t : CULEX_TRANSPORT {
  trans_TEST_t() : CULEX_TRANSPORT({(char *)"TEST", (EXPRESS_TYPE_ENUM)UInt8}) {}
  void callback(JsonObject object, const char *type) override {
    eCulex.typeToValue(&this->value, type, object, (EXPRESS_TYPE_ENUM)UInt8);
    eMenu.info(__func__).p("value:").pln(this->value.UInt8);
  }
} trans_TEST;

struct trans_DATA_t : CULEX_TRANSPORT {
  trans_DATA_t() : CULEX_TRANSPORT({(char *)"DATA", (EXPRESS_TYPE_ENUM)Boolean}) {}
} trans_DATA;

struct trans_bdSeq_t : CULEX_TRANSPORT {
  trans_bdSeq_t() : CULEX_TRANSPORT({(char *)"bdSeq", (EXPRESS_TYPE_ENUM)UInt8}) {}
  void callback(JsonObject objectValue, const char *type) override {
    eMenu.info(__func__).pln();
  }
} trans_bdSeq;

struct trans_system_coreVoltage_t : CULEX_TRANSPORT {
  trans_system_coreVoltage_t() : CULEX_TRANSPORT({(char *)"system/coreVoltage", (EXPRESS_TYPE_ENUM)Float}) {
    this->readOnly = true;
  }
} trans_system_coreVoltage;

void express_culex::init(WiFiClass *WiFi) {
  _WiFi = WiFi;
  generateTopics();
  postTopics();
}

void express_culex::generateTopics() {
  //*  [NAMESPACE]/[USER_TOKEN]/[DEVICE_ID]/#
  for (auto &transport : CULEX_TRANSPORT_VECTORS) {
    sprintf(transport->topic, "%s/%s/%s/%s", DEFAULT_CULEX_NAMESPACE, DEFAULT_CULEX_USER_TOKEN, DEFAULT_CULEX_DEVICE_ID, transport->name);
    eMenu.debug(__func__).p("name: ").p(transport->name).p(", topic: ").pln(transport->topic);
  }
}

void express_culex::postTopics() {
  for (auto &transport : CULEX_TRANSPORT_VECTORS) {
    StaticJsonDocument<200> JSON;
    JSON["name"]      = transport->name;
    JSON["timestamp"] = millis();
    JSON["dataType"]  = EXPRESS_TYPE_CHAR[transport->type];
    JSON["value"]     = eUtil.valueToBuffer(transport->type, &transport->value);
    String payload;
    serializeJson(JSON, payload);
    eMenu.debug(__func__).p("name: ").p(transport->name).p(", payload: ").pln(payload);
  }
}

void express_culex::update() {
  // TODO if ( (wifiClient.connected()) && (WiFi.status() == WL_CONNECTED) )
  if (!culexClient.loop() && _WiFi->isConnected()) {  // if culex is NOT connected && wifi is connected
    if (millis() >= connection_timeout_last + connection_timeout_wait) {
      if (!connect()) {  // if FAILED to connect
        connection_timeout_last = millis();
        if (connection_timeout_wait <= 32000) {
          //  multiply connection timeout eg. 500ms, 1000ms, 2000ms ... 32000ms, 64000ms.
          connection_timeout_wait = connection_timeout_wait * 2;
        }
        eMenu.info(__func__).p("connection timeout: ").pln(connection_timeout_wait);
      }
    }
  } else if (isConnected && (culexClient.state() != 0 || !_WiFi->isConnected())) {  // if culex is NOT connected && wifi is NOT connected
    isConnected = false;
   eMenu.error(__func__).p("connection terminated!").pln();
  }
}

void express_culex::disconnect() {
  eMenu.error(__func__).pln();
  culexClient.disconnect();
}

boolean express_culex::connect() {
  if (culexClient.state() != 0) {  // if NOT connected
    culexClient.disconnect();
    culexClient.setBufferSize((uint16_t)CULEX_PAYLOAD_SIZE);
    culexClient.setServer(DEFAULT_CULEX_SEVER_IP_ADDRESS, DEFAULT_CULEX_SERVER_PORT);
    culexClient.setKeepAlive(DEFAULT_CULEX_KEEPALIVE);
    culexClient.setSocketTimeout(DEFAULT_CULEX_SET_SOCKET_TIMEOUT);
    culexClient.setCallback(eCulex.STATIC_DATA_EVENT);

    // eMenu.debug("Generatating the last will and testament ...").pln();
    // trans_bdSeq.POST = true;
    // char topic_buf[128];
    // char payload_buf[256];
    // topic(topic_buf, NDEATH);
    // payload(payload_buf);
    //   trans_bdSeq.value.UInt8++;

    if (culexClient.connect(PROJECT_NAME, DEFAULT_CULEX_USERNAME, DEFAULT_CULEX_PASSWORD, trans_bdSeq.topic, CULEX_LWT_QOS, CULEX_LWT_RETAIN, "{\"birth\":false}", CULEX_LWT_CLEAN_SESSION)) {
      isConnected = true;
      eMenu.info(__func__).p("connected").pln();
      connection_timeout_wait = CULEX_CONNECTION_WAIT;
      connection_timeout_last = 0;
      //? https://arduinojson.org/v6/how-to/use-arduinojson-with-pubsubclient/
      //   char buffer[256];
      //   size_t n = serializeJson(doc, buffer);
      //   client.publish("outTopic", buffer, n);

      // culexClient.publish("outTopic", "hello world");
      // culexClient.subscribe("inTopic");
      // birth();
      trans_bdSeq.value.UInt8++;
      eMenu.debug(__func__).p("subscribing to: ").pln(trans_DATA.topic);
      culexClient.subscribe(trans_DATA.topic);
      return true;
    } else {
      eMenu.error(__func__).p("connection failed! ").p(culexClient.state()).p(" ").pln(MQTT_CONNECTION_ERRORS_CHAR[culexClient.state() + 4]);
      return false;
    }
    return false;
  }
}

// void express_culex::birth() {
//   char topic_buf[128];
//   char payload_buf[256];
//   //   for (auto &transport : CULEX_TRANSPORT_VECTORS) {
//   //     transport->POST = true;
//   //   }
//   //   topic(topic_buf, NBIRTH);
// //   payload(payload_buf);

//   culexClient.publish(topic_buf, payload_buf);

//   //   culexClient.setCallback(eCulex.STATIC_DATA_EVENT);
//     topic(topic_buf, DATA);
//   culexClient.subscribe(trans_DATA_t.topic);
// }

// void express_culex::DATA_EVENT(const char topic, byte* payload, unsigned int length){
//     eMenu.debug("Culex DATA topic:").p(topic).p(", length:").p(length).pln(", payload...");
//     eMenu.debug(payload).pln();
// }
