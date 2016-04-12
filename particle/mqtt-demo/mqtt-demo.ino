#include "MQTT/MQTT.h"

#define MQTT_HOST "otis.leandog.com"
#define MQTT_PORT 52122
#define MQTT_USERNAME "meetup"
#define MQTT_PASSWORD "<replace_with_password>"

#define UID "<replace_with_your_name_or_handle>"
#define ONLINE_TOPIC "/boat/meetup/" UID "/online"
#define STATUS_TOPIC "/boat/meetup/" UID "/status"

#define MESSAGE_TEMPLATE "%s is sending messages from Particle!"
#define PAYLOAD_TEMPLATE "{\"statusMessage\": \"%s\", \"lastUpdatedTicks\": %lu }"

#define ULONG_CHAR_LENGTH (10*sizeof(char))
#define NULL_CHAR_LENGTH (1*sizeof(char))


void callback(char* topic, byte* payload, unsigned int length);

MQTT mqttClient(MQTT_HOST, MQTT_PORT, onMessageReceived);

long lastMsg = 0;
char *payload;
char *message;
bool success = false;

void setup() {
  Serial.begin(115200);

  message = (char*)malloc(strlen(MESSAGE_TEMPLATE) + strlen(UID) + NULL_CHAR_LENGTH);
  sprintf(message, MESSAGE_TEMPLATE, UID);

  payload = (char*)malloc(strlen(PAYLOAD_TEMPLATE) + strlen(message) + ULONG_CHAR_LENGTH + NULL_CHAR_LENGTH);
}

void loop() {
  while(!Particle.connected()) {
    Serial.println("Waiting for internet connection, try again in 5 seconds");
    delay(5000);
  }
  
  if (!mqttClient.isConnected()) {
    reconnect();
    
    char *online = "1";
    mqttClient.publish(ONLINE_TOPIC, (uint8_t*)online, strlen("1"), true);
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
  
  success = mqttClient.publish(STATUS_TOPIC, (uint8_t*)payload, strlen(payload), true);
  if(success) {
    Serial.println("published!");
  } else {
    Serial.println("failed! Make sure your message is under the max length.");
  }
}

void reconnect() {

  // Loop until we're reconnected
  while (!mqttClient.isConnected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    //if (mqttClient.connect(UID, MQTT_USERNAME, MQTT_PASSWORD)) {
    if (mqttClient.connect(UID, MQTT_USERNAME, MQTT_PASSWORD, ONLINE_TOPIC, MQTT::QOS1, true, "0")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
