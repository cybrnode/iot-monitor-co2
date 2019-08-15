class SensorManager{
private:
  // Constant veriables
  const char * DEBUG_TAG = "SENSOR";
  int sensorPin;


public:
  void  begin(Device & device);
  float getPpmCo2();
};


void SensorManager::begin(Device & device) {

  sensorPin = device.CO_SENSOR_PIN;
  
}
