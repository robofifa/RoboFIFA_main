#include <Encoder.h>
#include <MX1508.h>
#include <RoboFIFA_communication.h>

// the numbers of the motor pins
const int M1A = 26;  // 26 corresponds to GPIO26
const int M1B = 18;  // ""
const int M2A = 19;  // ""
const int M2B = 23;  // ""

// setting PWM properties
const int freq = 5000; // range at least 100-5000 Hz
const int ledChannel1 = 0; // 16 channels: 0-15, but every even channels shares its freq with the one before, thus 0&1, 2&3...
const int ledChannel2 = 2; // 16 channels: 0-15, but every even channels shares its freq with the one before, thus 0&1, 2&3...
const int resolution = 8; // 1-16 bits
const int max_power = 100;
const int min_power = -100;

const char* server_ip = "192.168.43.128";

// Vars for the Encoders
Encoder *Left;
Encoder *Right;
const uint8_t L_CHAN_A = 16;
const uint8_t L_CHAN_B = 17;
const uint8_t R_CHAN_A = 21;
const uint8_t R_CHAN_B = 22;
const unsigned int FILTERLENGTH = 1023; 

RoboFIFA_communication com(server_ip);
MX1508 M1(M1A, M1B, freq, ledChannel2, resolution);
MX1508 M2(M2A, M2B, freq, ledChannel1, resolution);

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  RoboFIFA_communication::robot_msg* motorspeeds;
  motorspeeds = (RoboFIFA_communication::robot_msg*)message;
  
  int dutyCycle1 = M1.getMaxPwm() * motorspeeds->left;
//  Serial.println(dutyCycle1);
  M1.setDutyCycle(dutyCycle1);
  int dutyCycle2 = M2.getMaxPwm() * motorspeeds->right;
//  Serial.println(dutyCycle2);
  M2.setDutyCycle(dutyCycle2);
}

void setup() {
  
  Serial.begin(115200);
  const char* ssid = "miwifi";
  const char* password = "neeneenee";
  com.setup_wifi(ssid, password);
  com.setup_mqtt(callback);
  M1.init();
  M2.init();
  Left = new Encoder(PCNT_UNIT_0, L_CHAN_A, L_CHAN_B, Encoding::X4, FILTERLENGTH);
  Right = new Encoder(PCNT_UNIT_1, L_CHAN_A, L_CHAN_B, Encoding::X4, FILTERLENGTH);
}


unsigned long count = 0;

void loop() {
  static long lastMsg = 0;
  
  com.loop();

  long now = millis();
  if (now - lastMsg >= 5000) {
    lastMsg = now;
    
    // Convert the value to a char array
    char tempString[32];
    dtostrf(lastMsg, 1, 2, tempString);
    Serial.print("sending: ");
    Serial.println(tempString);
    com.publish(tempString);
  }
  if(count < (millis() - 100)){
    count = millis();
    Serial.print("Left ");
    Serial.println(Left->ReadAndReset());
    Serial.print("Right ");
    Serial.println(Right->ReadAndReset());
  }
}
