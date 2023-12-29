#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <espnow.h>

const int ledPin = D4; // pin LED

// WiFi
const char *ssid = "SGTA";         // Enter your WiFi name
const char *password = "bsry4421"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic_mqtt = "kelasiotesp/PENYOLDER/distance: ";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

int jarak;
int status;

// ESP-NOW
typedef struct struct_message
{
  char receive_data[32];
} struct_message;

struct_message myData;

WiFiClient espClient;
PubSubClient client(espClient);
char str[80];

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("distance: ");
  Serial.print(myData.receive_data);
  Serial.println(" cm");

  // Mengirim data ke MQTT broker setelah menerima data ESP-NOW
  client.publish(topic_mqtt, myData.receive_data);
}

void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Pesan baru dari topik: ");
  Serial.println(topic);

  if (strcmp(topic, "kelasiotesp/PENYOLDER/distance: ") == 0)
  {
    payload[length] = '\0';        // Tambahkan null terminator untuk mengonversi ke string
    jarak = atoi((char *)payload); // Konversi string menjadi integer
    Serial.print("Jarak: ");
    Serial.println(jarak);
  }

  if (strcmp(topic, "kelasiotesp/PENYOLDER/interrupt:") == 0)
  {
    payload[length] = '\0';        // Tambahkan null terminator untuk mengonversi ke string
    status = atoi((char *)payload); // Konversi string menjadi integer
    Serial.print("Status: ");
    Serial.println(status);
  }
  if (status == 1) digitalWrite(ledPin, HIGH);
  else if (status == 0)
  {
    if (jarak <= 10) digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // Connecting to an MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe("kelasiotesp/PENYOLDER/distance: ");
  client.subscribe("kelasiotesp/PENYOLDER/interrupt:");

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packet info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  client.loop();
}