/*

#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x24, 0xD7, 0xEB, 0xC9, 0x23, 0x45};
const int triggerPin = D4;
const int echoPin = D3;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  char send_data[32];
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0)
  {
    Serial.println("Delivery success");
  }
  else
  {
    Serial.println("Delivery fail");
  }
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {

    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Set values to send
    char distance_str[8];
    // String distanceString = String(distance);
    // distanceString.toCharArray(distance_str, sizeof(distance_str));

    snprintf(distance_str, sizeof(distance_str), "%d", distance);

    strcpy(myData.send_data, distance_str);

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

    lastTime = millis();
  }
}
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Ticker.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x24, 0xD7, 0xEB, 0xC9, 0x23, 0x45};
const int triggerPin = D4;
const int echoPin = D3;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  char send_data[32];
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;
Ticker timer;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0)
  {
    Serial.println("Delivery success");
  }
  else
  {
    Serial.println("Delivery fail");
  }
}

void measureAndSendData()
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Set values to send
  char distance_str[8];
  snprintf(distance_str, sizeof(distance_str), "%d", distance);
  strcpy(myData.send_data, distance_str);

  // Send message via ESP-NOW
  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Set up the task scheduler to run measureAndSendData every 5000 milliseconds
  timer.attach_ms(5000, measureAndSendData);
}

void loop()
{
  // Loop is now empty or can be used for other non-time-sensitive tasks
}
