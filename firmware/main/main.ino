#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h> // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <StorageIO.h>
#include <Event.h> // https://github.com/zeeshan-iqbal-cs/arduino-event
#include <ESP8266WebServer.h>
#include <IoTGuru.h> // https://github.com/IoTGuruLive/theiotguru-arduino-library

#include "config.h"
#include "globalVars.h"
#include "WebServer.h"

volatile bool wifi_btn_pressed = false;

void ICACHE_RAM_ATTR wifi_btn_isr(){
    wifi_btn_pressed = true;
}

void start_ap(){
    if (WiFi.getMode() != WIFI_AP){
        lcd.setCursor(0,0);
        lcd.print("Please Wait...  ");
        scan_wifi_networks(); // this sets the mode to WIFI_STA
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_NAME);
        startWebServer();
    }
    else{
      Serial.println("Already in AP mode...");
    }
    wifi_btn_pressed = false;
}

void scan_wifi_networks(){
    // this function scans wifi and updates the global ssidList
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
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

}


void setup() {
  
  Serial.begin(9600);
  Serial.println("STARTING SETUP");


  storage.begin();
  dashboardUpdateEvent.begin(DASHBOARD_UPDATE_INTERVAL);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting Wifi");
  
  delay(1000);
  // 0      1       2     3 
  /*"NULL", "STA", "AP", "STA+AP" */
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  delay(100);

  Serial.println("reading ssid,pass from storage...");
  WIFI_SSID = storage.readNextString();
  WIFI_PASS = storage.readNextString();
  Serial.println(
    String("Connecting to :\n\t")+"SSID:" + String(WIFI_SSID) + "\n\t"+
    "PASSWORD:" + String(WIFI_PASS)
  );

    
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  delay(2000);

  // wait for 5 seconds or until wifi is connected...
  int x = 0;
  while ( x <= 5 ) {
    x++;
    if (WiFi.status() != WL_CONNECTED){
      delay(1000);
      Serial.print(".");
    } else {
      Serial.println();
      Serial.println("WiFi connected");
      Serial.println("IP address: "); Serial.println(WiFi.localIP());
      break;
    }
  }

  Serial.print("WiFi: ");
  Serial.println(update_wifi_status());

    // check the fingerprint of io.adafruit.com's SSL cert

  pinMode(WIFI_BTN_PIN, INPUT_PULLUP); // declare pushbutton as input
  pinMode(PWM_PIN, INPUT);

  lcd.clear(); lcd.setCursor(0,1); lcd.print("SETUP: Complete");
  Serial.println("SETUP: Complete");  

  attachInterrupt(digitalPinToInterrupt(WIFI_BTN_PIN), wifi_btn_isr, CHANGE);

  iotGuru.setDebugPrinter(&Serial);
  iotGuru.setNetworkClient(&client);
}

String update_wifi_status(){
  lcd.setCursor(0,0);
  if(WiFi.getMode() == WIFI_AP){
        lcd.print("WiFi: Hotspot   ");
        return "Hotspot";
  } else {
    if (WiFi.status() == WL_CONNECTED){
      lcd.print("WiFi: Connected ");
      return "Connected";
    } else {
      lcd.print("WiFi: Failed    ");
      return "Failed";
    }
  }
}


void lcd_print_ppm(float ppm){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(String("CO2: ") + ppm + " ppm");
}


void loop() {
  if(wifi_btn_pressed){
    start_ap();
    delay(1000);
  }
  
  pwm_value = pulseIn(PWM_PIN, HIGH);
  ppm = pwm_value / 200.0;
    
  Serial.println(ppm);

  lcd_print_ppm(ppm);
  update_wifi_status();

  if (WiFi.getMode() == WIFI_AP){
    webServer.handleClient();
    return;
  }

  if ( WiFi.getMode() != WIFI_STA)
    return;

  // if one minute passed since last update...update dashboard..
  if (dashboardUpdateEvent.trigger()){
    iotGuru.sendMqttValue(nodeShortId, fieldName, ppm);
    
    delay(200); 
    dashboardUpdateEvent.reset();
  }

}
