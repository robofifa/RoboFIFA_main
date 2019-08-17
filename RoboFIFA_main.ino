#include <MX1508.h>
#include <RoboFIFA_communication.h>

// the numbers of the motor pins
const int M1A = 16;  // 16 corresponds to GPIO16
const int M1B = 17;  // 17 corresponds to GPIO17

// setting PWM properties
const int freq = 5000; // range at least 100-5000 Hz
const int ledChannel = 0; // 16 channels: 0-15, but every even channels shares its freq with the one before, thus 0&1, 2&3...
const int resolution = 8; // 1-16 bits
const int max_power = 100;
const int min_power = -100;

RoboFIFA_communication com("192.168.43.12");
MX1508 M1(M1A, M1B, freq, ledChannel, resolution);

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  int dutyCycle = M1.getMaxPwm() * messageTemp.toFloat();
  Serial.println(dutyCycle);
  M1.setDutyCycle(dutyCycle);
}

void setup() {
  
  Serial.begin(115200);
  const char* ssid = "miwifi";
  const char* password = "neeneenee";
  com.setup_wifi(ssid, password);
  com.setup_mqtt(callback);
  M1.init();
}


void loop() {
  static long lastMsg = 0;
  
  com.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    // Convert the value to a char array
    char tempString[32];
    dtostrf(lastMsg, 1, 2, tempString);
    com.publish(tempString);
  }
  
//  for (int dutyCycle = min_power; dutyCycle <= max_power; dutyCycle++) {
//    // changing the  PWM
//    M1.setDutyCycle(dutyCycle);
//    delay(15);
//  }
//
//  for (int dutyCycle = max_power; dutyCycle >= min_power; dutyCycle--) {
//    // changing the PWM
//    M1.setDutyCycle(dutyCycle);
//    delay(15);
//  }
}
