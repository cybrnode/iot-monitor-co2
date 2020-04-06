void setup() {
  Serial.begin(9600);

  while (!Serial) delay(25);

  Serial.println("Start!");

  pinMode(D5, INPUT);
}

void loop() {
  // https://www.arduino.cc/reference/en/language/functions/advanced-io/pulsein/
  long duration = pulseIn(D5, HIGH, 2000000);
  int value = duration / 200.0;

  Serial.println(value);
}
