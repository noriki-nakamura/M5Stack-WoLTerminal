#include <WakeOnLan.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <WebServer.h>

#include "define.h"
#include "config.h" //Please prepare config.h file ref: config.h.sample

WiFiUDP wifiUdp; 
WakeOnLan WOL(wifiUdp);

WebServer server(80);

unsigned long lastActiveTime = 0;
const int SLEEP_TIMEOUT = 30000;

void setup() {
  Serial.begin(115200);
  M5.begin(true,true,false);
  M5.Power.begin();
  WiFi.begin(ssid, password);

  M5.Lcd.setBrightness(200);
  M5.Lcd.println("M5stack Wake on Lan Terminal");
  
  M5.Lcd.print("Connect Status: ");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    M5.Lcd.print('.');
  }
  M5.Lcd.println(" Connect!");
  M5.Lcd.print("IP address: ");
  M5.Lcd.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/A", handleA);
  server.begin();
  M5.Lcd.println("HTTP server started!");
}

void loop() {
  M5.update();
  server.handleClient();

  if(M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()){
    lastActiveTime = millis();
    M5.Lcd.wakeup();
    M5.Lcd.setBrightness(200);
  }
  if (millis() - lastActiveTime > SLEEP_TIMEOUT){
    M5.Lcd.sleep();
    M5.Lcd.setBrightness(0);
  }

  if(M5.BtnA.wasReleased()){
    sendWoL();
  }
}

void sendWoL() {
  M5.Lcd.print("Send start to: ");
  M5.Lcd.print(MACAddress);
  WOL.sendMagicPacket(MACAddress);
  M5.Lcd.println(" Complete!");
}

void handleRoot() {
  char* data;
  data = \
    "<html>\
      <head><title>Wake on Lan terminal</title></head>\
      <body>Target MAC address: xxx <button onclick=\"location.href='/A'\">Wake</button></body>\
    </html>";

  server.send(200, "text/html", data);
}

void handleA() {
  sendWoL();
  server.send(200, "text/plain", "OK");
}