ESP8266WebServer webServer(80);


//LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
StorageIO storage;
Event dashboardUpdateEvent;

/************ Global State (you don't need to change this!) ******************/

// WiFiFlientSecure for SSL/TLS support
WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish co2_monitor_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"FEED_NAME);

// io.adafruit.com SHA1 fingerprint
static const char *fingerprint PROGMEM = "77 00 54 2D DA E7 D8 03 27 31 23 99 EB 27 DB CB A5 4C 57 18";

float ppm;
String ssidList;
String WIFI_SSID; // read from eeprom later..
String WIFI_PASS;
bool wifi_btn_val = 0;
unsigned long pwm_value;
unsigned long previousMillis = 0;
