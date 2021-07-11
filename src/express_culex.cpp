/*
    @file       express_culex.cpp
    @author     matkappert
    @repo       github.com/matkappert/express
    @date       11/07/21
*/

#include "express_culex.h"

#include "Settings.h"

express_culex eCulex;  // global-scoped variable

void express_culex::init(WiFiClass *WiFi) {
  _WiFi = WiFi;
}

void express_culex::update() {
  if (!culexClient.loop() && _WiFi->isConnected()) {  // if culex is NOT connected && wifi is connected
    if (millis() >= connection_timeout_last + connection_timeout_wait) {
      if (!connect()) {  // if FAILED to connect
        connection_timeout_last = millis();
        if (connection_timeout_wait <= 32000) {
          //  multiply connection timeout eg. 500ms, 1000ms, 2000ms ... 32000ms, 64000ms.
          connection_timeout_wait = connection_timeout_wait * 2;
        }
        eMenu.info("Culex connection timeout: ").pln(connection_timeout_wait);
      }
    }
  } else if (isConnected && (culexClient.state() != 0 || !_WiFi->isConnected())) {  // if culex is NOT connected && wifi is NOT connected
    isConnected = false;
    eMenu.error("Culex connection terminated!").pln();
  }
}

void express_culex::topic(char *buf, CULEX_TOPICS_ENUM topic) {
  sprintf(buf, "%s/%s/%s/%s", DEFAULT_CULEX_NAMESPACE, DEFAULT_CULEX_GROUP_ID, CULEX_TOPICS_CHAR[topic], DEFAULT_CULEX_EDGE_NODE_ID);
  eMenu.debug("Culex topic: ").pln(buf);
}

void express_culex::payload(char *buf) {
  timestamp    = millis();
  uint8_t size = CULEX_TRANSPORT_VECTORS.size();
  eMenu.debug("Culex transport vectors size: ").pln(size);
  sprintf(buf, "{\"timestamp\":%u,\"metrics\":[", timestamp);
  uint8_t i = 0;
  for (auto &transport : CULEX_TRANSPORT_VECTORS) {
    if (transport->POST == true) {
      metric(buf, transport);
      if (i != size - 1) {
        strncat(buf, ",", 1);
      }
    }
    transport->POST = false;
    i++;
  }
  strncat(buf, "]}", 2);
  eMenu.debug("payload: ").pln(buf);
}

void express_culex::metric(char *buf, CULEX_TRANSPORT *transport) {
  char metric_buf[128];
  metric_buf[0] = '\0';
  uint8_t type  = transport->type;
  if (type == Int8) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.Int8);
  } else if (type == Int16) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.Int16);
  } else if (type == Int32) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.Int32);
  } else if (type == UInt8) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.UInt8);
  } else if (type == UInt16) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.UInt16);
  } else if (type == UInt32) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.UInt32);
  } else if (type == Float) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.Float);
  } else if (type == Double) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%u}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.Double);
  } else if (type == Boolean) {
    sprintf(metric_buf, "{\"name\":\"%s\",\"timestamp\":%u,\"dataType\":\"%s\",\"value\":%s}", transport->name, timestamp, EXPRESS_TYPE_CHAR[type], transport->value.Boolean ? "true" : "false");
  }
  strncat(buf, metric_buf, sizeof metric_buf);
}

void express_culex::disconnect() {
  eMenu.error("Culex disconnected!").pln();
  culexClient.disconnect();
}

boolean express_culex::connect() {
  if (culexClient.state() != 0) {  // if NOT connected
    culexClient.disconnect();
    culexClient.setBufferSize((uint16_t)DEFAULT_CULEX_BUFFER_SIZE);
    culexClient.setServer(DEFAULT_CULEX_SEVER_IP_ADDRESS, DEFAULT_CULEX_SERVER_PORT);
    culexClient.setKeepAlive(DEFAULT_CULEX_KEEPALIVE);
    culexClient.setSocketTimeout(DEFAULT_CULEX_SET_SOCKET_TIMEOUT);

    eMenu.debug("Generatating the last will and testament ...").pln();
    char topic_buf[128];
    char payload_buf[256];
    topic(topic_buf, NDEATH);
    payload(payload_buf);
    //   trans_bdSeq.value.UInt8++;

    if (culexClient.connect(PROJECT_NAME, DEFAULT_CULEX_USERNAME, DEFAULT_CULEX_PASSWORD, topic_buf, CULEX_LWT_QOS, CULEX_LWT_RETAIN, payload_buf, CULEX_LWT_CLEAN_SESSION)) {
      isConnected = true;
      eMenu.info("Culex connected").pln();
      connection_timeout_wait = CULEX_CONNECTION_WAIT;
      connection_timeout_last = 0;
      // culexClient.publish("outTopic", "hello world");
      // culexClient.subscribe("inTopic");
      birth();
      return true;
    } else {
      eMenu.error("Culex connection failed! ").p(culexClient.state()).p(" ").pln(MQTT_CONNECTION_ERRORS_CHAR[culexClient.state() + 4]);
      return false;
    }
    return false;
  }
}

void express_culex::birth() {
  char topic_buf[128];
  char payload_buf[256];
  for (auto &transport : CULEX_TRANSPORT_VECTORS) {
    transport->POST = true;
  }
  topic(topic_buf, NBIRTH);
  payload(payload_buf);

  culexClient.publish(topic_buf, payload_buf);
}
