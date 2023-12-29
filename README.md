# **Cara Membuat Sistem Parkir Menjadi *Online* Menggunakan Ultrasonik dan ESP D1 Mini (ESP8266)**

Tujuan proyek ini adalah untuk memenuhi tugas besar kelas IoT Universitas Telkom semester ganjil 2023. 

Kelompok: Penyolder

Anggota: 
    1. Galih Karya Gemilang 1103202098
    2. Rizky Ramadhani Syam 1103204086
    3. Yoandhika Surya Putra 1103204169
    4. Azam Aulyaa 1103204205

## Ringkasan Singkat Proyek
Proyek ini akan merangkai rangkaian sederhana arduino yang menggunakan *ESP D1 Mini* dan ultrasonik sebagai sensor jarak pada sisi node yang nantinya jika ultrasonik membaca jarak tertentu, pada sisi gateaway, menggunakan protokol *ESP-NOW*, indikator LED yang sudah dirangkai dengan *ESP D1 Mini* lainnya akan memberitahu apakah parkiran terisi atau kosong. Kemudian data tersebut dikirim ke *Broker*. Kemudian, data yang sudah berada di *Broker* akan di ambil lagi oleh *NodeRed* untuk kontrol jarak jauh. 

**Estimasi pengerjaan proyek (tidak termasuk bug fixing):** 
sekitar 1-2 jam

## Capaian
**Alasan pemilihan topik proyek:** 
Kelompok kami memilih topik ini karena terkadang ketika pergi ke suatu tempat tapi tidak yakin bahwa tempat tersebut masih memiliki parkiran yang kosong. Sehingga dengan mengambil topik ini tempat parkiran bisa dikontrol dari jarak jauh, sehingga kita bisa mengetahui parkiran tersebut masih ada tempat kosong atau tidak. 

**Tujuan proyek:** 
Tujuan utama dari proyek ini adalah untuk memahami tentang IoT, bagaimana cara merangkai alat-alat IoT satu dengan lainnya, dan mengontrol alat IoT tersebut dari jarak jauh. 

**Hal yang didapatkan dari proyek:** 
Proyek ini memberikan kami konsep dasar dari IoT, cara memprogramnya, dan merangkai rangkaian IoT. 

## Alat dan Bahan


| Komponen         | Fungsi                                                | Toko              | Harga    |
| ---------------- | ----------------------------------------------------- | ----------------- | -------- |
| ESP D1 Mini      | Mikrokontroler, pengambilan data, dan transfer data   | CNC Store Bandung | Rp.26900 |
| HC-SR04          | Ultrasonik: sensor jarak                              | CNC Store Bandung | Rp.12000 |
| Kabel Jumper Kit | Penghubung antar komponen IoT                         | CNC Store Bandung | Rp.36000 |
| Kabel USB Mikro  | Kabel penghubung / interface antara laptop dengan ESP | CNC Store Bandung | Rp.7500  |
| Breadboard       | Wadah rangkaian komponen IoT                          | CNC Store Bandung | Rp.6500  |

![Screenshot 2023-12-25 124802](https://hackmd.io/_uploads/SJoTO9LDa.png)
Gambar 1: ESP D1 Mini

![Screenshot 2023-12-25 124312](https://hackmd.io/_uploads/BkspdcIwa.png)
Gambar 2: HC-SR04

![Screenshot 2023-12-25 124816](https://hackmd.io/_uploads/rJs6u98D6.png)
Gambar 3: Kabel Jumper

![Screenshot 2023-12-25 124744](https://hackmd.io/_uploads/Hyjp_c8DT.png)
Gambar 4: Kabel USB Mikro

![Screenshot 2023-12-25 124727](https://hackmd.io/_uploads/H19aO9Uv6.png)
Gambar 5: Kisi soket

## Pengaturan Komputer
Sistem operasi yang digunakan untuk mengembangkan alat IoT ini adalah Windows 11. Text editor yang digunakan adalah vscode dengan bantuan Platform IO untuk melakukan building dan uploading ke mikrokontroler. Namun perlu diketahui juga bahwa ketika memhubungkan ESP ke komputer, ESP membutuhkan driver CH340. Driver ini bisa langsung dicari di mesin pencarian. 

Untuk melakukan visualisasi dan kontrol data jarak jauh, dibutuhkan NodeRed. NodeRed merupakan salah satu platform anak dari NodeJs yang dapat diunduh di https://nodejs.org/en/. Atau tutorial menginstall NodeRed-nya dapat diliat di https://RandomNerdTutorials.com/esp-now-esp8266-nodemcu-arduino-ide/. 

## Merangkai Rangkaian
![rangkaian](https://hackmd.io/_uploads/Hy3MNiUvp.png)
Gambar 6: Rangkaian komponen IoT

Rangkaian yang kelompok kami susun kurang lebih dapat dilihat pada gambar 6. Rangkaian bisa menyesuaikan sesuai kebutuhan selagi pin sudah terhubung dengan benar. Pin pada HC-SR04 yang dihubungkan dengan ESP D1 Mini secara berurutan adalah VCC-5V, GND-G, TRIGGER-GPIO(D4), ECHO-GPIO(D3). 

## Pemilihan Platform
Paltform yang kelompok kami gunakan untuk proyek ini adalah NodeRed. NodeRed ini berjalan secara lokal dan gratis. Node-RED adalah platform pemrograman visual open-source yang dirancang untuk menghubungkan perangkat keras, perangkat lunak, dan layanan online dalam lingkungan Internet of Things (IoT). Platform ini memungkinkan pengguna untuk membuat alur kerja (flows) dengan cara yang intuitif melalui antarmuka grafis berbasis web.

## Kodingan
Pada proyek ini, terdapat dua kodingan utama. Kodingan untuk node dan untuk gateaway. 

Berikut adalah kodingan untuk sisi node. 
```cpp
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
```

Dan, berikut adalah kodingan untuk sisi gateaway. 
```cpp
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <espnow.h>

const int ledPin = D4; // pin LED

// WiFi
const char *ssid = "Galaxy A33 5G 8347";         // Enter your WiFi name
const char *password = "kedo9581"; // Enter WiFi password

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
```

## Pengiriman Data / Konektivitas
Pada proyek ini, pada sisi node, mikrokontroler mengambil data dari sensor setiap 5 detik sekali. Data sensor yang baru saja dibaca, dikirim ke sisi gateaway (ESP lainnya secara spesifik menggunakan MAC Address) menggunakan protokol ESP-NOW. Protokol ESP-NOW ini tidak membutuhkan koneksi internet, mirip seperti UDP. Setelah gateaway menerima data dari node, data dianalisis sehingga jika data yang dikirim jaraknya kurang dari 10 cm, LED mati yang artinya tempat parkiran terisi, sedangkan jika terdeteksi jaraknya lebih dari 10 cm, LED akan nyala yang artinya parkiran tersedia. 

Selanjutnya data dikirim ke broker. Pada tahap ini, gateaway membutuhkan koneksi ke internet karena broker MQTT ini bersifat *public*.

Setelah data sampai di broker, pengontrolan jarak jauh dapat dilakukan menggunakan dashboard NodeRed. 




![image](https://hackmd.io/_uploads/SJr_rRIDp.png)
Gambar 7: Flow Node NodeRed


## Menampilkan Data

![image](https://hackmd.io/_uploads/H1LJBCLPp.png)
Gambar 8: Visualisasi data menggunakan model *gauge*

Kelompok kami menggunakan model *gauge* untuk memvisualisasikan jumlah parkiran yang masih kosong. Selain itu juga kami menyediakan tombol *interupt* jika hendak mematikan indikator LED dari jarak jauh. 

## Finalisasi
![Screenshot 2023-12-26 203656](https://hackmd.io/_uploads/rkAmw8OwT.png)

Gambar 9: Parkiran kosong

![Screenshot 2023-12-26 203714](https://hackmd.io/_uploads/ByAmP8uDT.png)

Gambar 10: Parkiran terisi

**Pemikiran Akhir**
Hasil akhir dari proyek yang sudah kelompok kami kerjakan sedikit berbeda dengan apa yang kami ajukan di awal. Hal tersebut terjadi karena adanya *error* yang masih belum bisa kami perbaiki. Namun, secara umum kelompok kami sudah memahami konsep dasar dari IoT sendiri. 

Rekap dari yang sudah kelompok kami kerjakan, topologi yang digunakan adalah node -(ESP-NOW)- gateaway -(internet)- broker. Sensor biasa dipasang pada sisi node. Gateaway adalah tempat berkumpulnya data yang yang sudah dibaca oleh node-node. Kemudian gateaway mengirimkan data ke broker yang membutuhkan internet. Setelah data sampai di broker, data bisa dilakukan visualisasi, salah satunya menggunakan dashboard NodeRed. Pada dashboard juga kita bisa melakukan kontrol terhadap barang IoT yang dikembangkan. 
