#include <SPI.h>
#include <TimeLib.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>
#include "SSD1306.h"

#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)
#define LED     2    // GPIO2  -- LED DOANG SIH

WiFiClient loraClient;
IPAddress server(192, 168, 1, 9);

int counter = 0;
long lastMsg = 0;
char msg[50];
int value = 0;

const char* ssid = "ssid";
const char* pass = "pass";
const char* mqtt = "serverip";

SSD1306 display(0x3c, 4, 15);
String rssi = "RSSI --";
String packSize = "--";
String packet;

void displayWifiStatus() {
  display.clear();
  display.drawString(0, 0, "SSID : ");
  display.drawString(38, 0, WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  display.drawString(0, 10, "IP Address : ");
  display.drawString(60, 10, String(ip));

  long wrssi = WiFi.RSSI();
  display.drawString(0, 20, "RSSI :");
  display.drawString(38, 20, String(wrssi));
  display.drawString(53, 20, " dBm");
  display.display();
  delay(1000);
  display.clear();
}

void callback(char* topic, byte* payload, unsigned int length) {
  display.drawString(0, 40, "Message arrived : ");
  display.drawString(85, 40, topic);
  //display.drawString(35, 30, ("] "));
  for (int i = 0; i < length; i++) {
    display.drawString(0, 50, (String(payload[i])));
  }
  //Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(LED, LOW);
    } else {
      digitalWrite(LED, HIGH);
    }
}

PubSubClient client(server, 1883, callback, loraClient);

void reconnect() {
  while (!client.connected()) {
    display.clear();
    display.drawString(0, 0, "Atempting MQTT connection...");
    String clientId = "brismaarsandi";
    clientId += String(random(0xffff), HEX);
    if (client.connect("brismaarsandi", "mentari", "qwer")) {
      display.drawString(0, 10, "connected");
      client.publish("test", "hello bukalapak");
      client.subscribe("testing");
      display.display();
      delay(2000);
      display.clear();
    } else {
      display.drawString(0, 10, "failed, rc=");
      display.drawString(0, 10, String(client.state()));
      //Serial.println(" try again in 5 seconds");
      display.display();
      delay(5000);
      display.clear();
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(16,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(LED,OUTPUT);

  digitalWrite(LED, HIGH);    // set GPIO16 low to reset OLED
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  digitalWrite(LED, LOW);    // set GPIO16 low to reset OLED
  
  SPI.begin(SCK,MISO,MOSI,SS);
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  //delay(1500);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.drawString(0, 3, "Connecting . . .");
    display.display();
  }
  display.clear();
  displayWifiStatus();
  //client.setServer(mqtt, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  display.clear();
  display.drawString( 0, 0, "Sending : ");
  //display.drawString(50, 0, String(counter));

  if(!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    snprintf (msg, 75, "Iot engineer #%d", counter);
    display.drawString(0, 10, "Publish message: ");
    display.drawString(0, 20, msg);
    client.publish("test",msg);
    counter++;
  }
  display.display();
  delay(2000);
}
