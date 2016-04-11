#include <ESP8266WiFi.h>
#include "PubSubClient.h"

#define WIFI_SSID "<replace_with_wifi_ssid>"
#define WIFI_PASSWORD "<replace_with_wifi_password>"

#define MQTT_HOST "otis.leandog.com"
#define MQTT_USERNAME "meetup"
#define MQTT_PASSWORD "<replace_with_password>"

#define UID "<replace_with_your_name_or_handle>"
#define ONLINE_TOPIC "/boat/meetup/" UID "/online"
#define STATUS_TOPIC "/boat/meetup/" UID "/status"

#define MESSAGE_TEMPLATE "%s is sending messages from ESP8266!"
#define PAYLOAD_TEMPLATE "{\"statusMessage\": \"%s\", \"lastUpdatedTicks\": %lu }"

#define ULONG_CHAR_LENGTH (10*sizeof(char))
#define NULL_CHAR_LENGTH (1*sizeof(char))


WiFiClient espClient;
PubSubClient mqttClient(espClient);

long lastMsg = 0;
char *payload;
char *message;
bool success = false;

void setup() {
  Serial.begin(115200);

  message = (char*)malloc(strlen(MESSAGE_TEMPLATE) + strlen(UID) + NULL_CHAR_LENGTH);
  sprintf(message, MESSAGE_TEMPLATE, UID);

  payload = (char*)malloc(strlen(PAYLOAD_TEMPLATE) + strlen(message) + ULONG_CHAR_LENGTH + NULL_CHAR_LENGTH);

  setup_wifi();
  mqttClient.setServer(MQTT_HOST, 52122);
  mqttClient.setCallback(onMessageReceived);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
    mqttClient.publish(ONLINE_TOPIC, "1", true);
    publishStatus();
    mqttClient.subscribe("/boat/meetup/#");
  }

  mqttClient.loop();
}

void onMessageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Got topic \"");
  Serial.print(topic);
  Serial.print("\" with payload \"");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("\"");
}

void publishStatus() {
  Serial.println("Building payload message");
  sprintf(payload, PAYLOAD_TEMPLATE, message, millis());
  Serial.println(payload);

  Serial.print("Publishing status to ");
  Serial.print(STATUS_TOPIC);
  Serial.print("...");
  
  success = mqttClient.publish(STATUS_TOPIC, payload, true);
  if(success) {
    Serial.println("published!");
  } else {
    Serial.println("failed! Make sure your message is under the max length.");
  }
}

void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.print("\nConnecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {

  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqttClient.connect(UID, MQTT_USERNAME, MQTT_PASSWORD, ONLINE_TOPIC, 1, true, "0")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

