#include "CO2Sensor.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <StorageIO.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling. 

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>

ESP8266WebServer webServer(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);
StorageIO storage;
CO2Sensor co2Sensor(A0, 0.99, 100);
String id = "CO2 HoT pro 1";
// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "1875f910-abd1-11e9-9636-f9904f7b864b";
char password[] = "1fad20bd3a148419aeb370a6b325437a507b6db7";
char clientID[] = "e05b7cc0-ac3f-11e9-9636-f9904f7b864b";

String ssidList;
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  Serial.println("");
  for (int i = 0; i < n; ++i) {
    ssidList += "<option value=\"";
    ssidList += WiFi.SSID(i);
    ssidList += "\">";
    ssidList += WiFi.SSID(i);
    ssidList += "</option>";
  }
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  WiFi.softAP(id.c_str());
  char * ssid = storage.readNextString();
  char * pass = storage.readNextString();
  WiFi.begin(ssid, pass);

  Serial.println("SSID:" + String(ssid));
  Serial.println("PASSWORD:" + String(pass));
  
  Serial.println("=== Initialized ===");
  co2Sensor.calibrate();
  
  startWebServer();
  Serial.println("SETUP: Complete");
}

void loop() {
  webServer.handleClient();
  int val = co2Sensor.read();
  Serial.print("CO2 value: ");
  Serial.println(val);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(String("CO2: ") + val + " ppm");
  if (WiFi.status() == WL_CONNECTED){
    lcd.setCursor(0,0);
    lcd.print("WiFi: Connected");
    Cayenne.begin(username, password, clientID);
    Cayenne.virtualWrite(0, val);   
  }else{
    lcd.setCursor(0,0);
    lcd.print("WiFi: Failed");
  }
  delay(5000);
}


void startWebServer() {
  // Main page --Directed at all illegal links
  webServer.onNotFound([]() {
    String s = "<h1>" + id + "</h1>";
    s += "<p><a href=\"/settings\">Wi-Fi Settings</a></p>";
    webServer.send(200, "text/html", makePage("AP mode", s));
  });

  // Display form
  webServer.on("/settings", []() {
    String s = "<h1>Wi-Fi Settings</h1><p>Please enter your password by selecting the SSID.</p>";
    s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
    s += ssidList;
    s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\"><input type=\"submit\"></form>";
    webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
  });
  webServer.on("/setap", []() {
    String ssid = urlDecode(webServer.arg("ssid"));
    String pass = urlDecode(webServer.arg("pass"));

    Serial.println("SSID:" + ssid);
    Serial.println("PASSWORD:" + pass);

    storage.reposition();
    storage.writeNextString(ssid);
    storage.writeNextString(pass);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Restart device");
    String s = "<h1>Setup complete.</h1>";
    s +="<p>device will be connected to ";
    s += ("\"" + ssid + "\"");
    s += " after the restart.</p>";
    webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
    delay(3000);
    ESP.restart();
  });
  webServer.begin();
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += contents;
  s += "</body></html>";
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}
