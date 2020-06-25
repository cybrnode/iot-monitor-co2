ESP8266WebServer webServer(80);


//LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
StorageIO storage;
Event dashboardUpdateEvent;

WiFiClient client;
IoTGuru iotGuru = IoTGuru(userShortId, deviceShortId, deviceKey);

float ppm;
String ssidList;
String WIFI_SSID; // read from eeprom later..
String WIFI_PASS;
bool wifi_btn_val = 0;
unsigned long pwm_value;
unsigned long previousMillis = 0;
