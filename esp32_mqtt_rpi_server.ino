#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Replace with your Raspberry Pi's IP address and Wi-Fi credentials
const char* mqtt_server = "192.168.0.101";
const char* ssid = "TP-Link_A926";
const char* password = "61688247";

// Update this for each ESP32, e.g., ESP32Client_2, ESP32Client_3, etc.
const char* mqtt_client_id = "ESP32Client_1"; // Update this for each ESP32, e.g., ESP32Client_2, ESP32Client_3, etc.
int device_id = 1; // Update this for each ESP32, e.g., 2, 3, etc.
char mqtt_topic[30];

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1);
  }

  snprintf(mqtt_topic, sizeof(mqtt_topic), "esp32/temperature/%d", device_id);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = bmp.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  char temperature_str[10];
  snprintf(temperature_str, sizeof(temperature_str), "%.2f", temperature);
  client.publish(mqtt_topic, temperature_str);

  delay(5000); // Publish every 5 seconds
}
