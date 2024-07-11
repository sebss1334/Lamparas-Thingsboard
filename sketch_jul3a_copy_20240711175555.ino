#include <WiFi.h>
#include <PubSubClient.h>

// Configuración WiFi
const char* ssid = "Sebss1334";
const char* password = "sebas123";

// Configuración de ThingsBoard
const char* mqtt_server = "thingsboard.cloud";
const int mqtt_port = 1883;
const char* token = "XPPSolzdNjo8aaSMwjLV";

WiFiClient espClient;
PubSubClient client(espClient);

// Pines para los LEDs
const int ledPin1 = 2;
const int ledPin2 = 42;

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar a ThingsBoard...");
    if (client.connect("ESP32Client", token, NULL)) {
      Serial.println("conectado");
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intenta de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
  
  // Procesa el mensaje para controlar los LEDs
  if (msg.indexOf("method") > 0) {
    if (msg.indexOf("setLED1") > 0) {
      if (msg.indexOf("true") > 0) {
        digitalWrite(ledPin1, HIGH);
      } else {
        digitalWrite(ledPin1, LOW);
      }
    } else if (msg.indexOf("setLED2") > 0) {
      if (msg.indexOf("true") > 0) {
        digitalWrite(ledPin2, HIGH);
      } else {
        digitalWrite(ledPin2, LOW);
      }
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
