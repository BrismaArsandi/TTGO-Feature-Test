#include <SPI.h>
#include <LoRa.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include <Wire.h>
#include <WiFi.h>
#include "SSD1306.h"
//#include "images.h"

#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)

#define BAND    433E6
#define PABOOST true

int counter = 0;
int8_t timeZone = 7;
int8_t minutesTimeZone = 0;
bool wifiFirstConnected = false;

const char* ssid = "ssid";
const char* pass = "pass";

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
  wifiFirstConnected = true;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(16,OUTPUT);
  pinMode(25,OUTPUT);
  
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high

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
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI00);
  
  if (!LoRa.begin(BAND,PABOOST))
  {
    display.drawString(0, 0, "Starting LoRa failed!");
    display.display();
    while (1);
  }
  display.drawString(0, 0, "LoRa Initial success!");
  display.display();
  delay(1000);

  NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
  NTP.setInterval(63);
}

void loop() {
  //displayWifiStatus();
  if (wifiFirstConnected) {
      wifiFirstConnected = false;
      NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
      NTP.setInterval (63);
  }
  // put your main code here, to run repeatedly:
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  display.drawString( 0, 0, "Sending : ");
  display.drawString(50, 0, String(counter));
  display.drawString( 0, 10, NTP.getTimeDateString ());
  display.display();

  LoRa.beginPacket();
  LoRa.print(counter);
  LoRa.print(" ");
  LoRa.print(NTP.getTimeDateString ());
  LoRa.endPacket();

  counter++;
  delay(1000);
}
