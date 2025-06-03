#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pin definitions
#define RAIN_SENSOR_PIN A2
#define SHOCK_SENSOR_PIN A3
#define GAS_SENSOR_PIN A1
#define PANIC_BUTTON_PIN 7
#define LED_PIN 3
#define BUZZER_PIN 6
#define SERVO_PIN 9
#define SPEED_SENSOR_PIN 10
#define GPS_RX_PIN 0
#define GPS_TX_PIN 1
#define PULSE_RATE_SENSOR_PIN A0

// Define other constants
#define SERVO_ANGLE_RAIN_STOP 90
#define PANIC_BUTTON_DELAY 5000  // 5 seconds
#define GAS_THRESHOLD 300
#define PULSE_UPDATE_INTERVAL 1000
#define SPEED_UPDATE_INTERVAL 500

// Initialize objects
Servo rainServo;
Adafruit_SSD1306 display(128, 64, &Wire);
TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

// Variables for pulse rate calculation
unsigned long lastPulseUpdate = 0;
unsigned long lastSpeedUpdate = 0;
int pulseCount = 0;
bool pulseState = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize sensors and actuators
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(SHOCK_SENSOR_PIN, INPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(PANIC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  rainServo.attach(SERVO_PIN);
  
  pinMode(SPEED_SENSOR_PIN, INPUT);
  pinMode(PULSE_RATE_SENSOR_PIN, INPUT);

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("System Initialized");
  display.display();
  delay(1000);

  // Initialize GPS
  gpsSerial.begin(9600);
}

void loop() {
  // Rain sensor
  if (digitalRead(RAIN_SENSOR_PIN) == HIGH) {
    rainServo.write(SERVO_ANGLE_RAIN_STOP);
    displayAlert("Rain Detected!");
  } else {
    rainServo.write(90); // Adjust as needed
  }

  // Panic button
  if (digitalRead(PANIC_BUTTON_PIN) == LOW) {
    activatePanicMode();
  }

  // Shock sensor
  if (digitalRead(SHOCK_SENSOR_PIN) == HIGH) {
    activateShockAlarm();
  }

  // Gas sensor
  int gasValue = analogRead(GAS_SENSOR_PIN);
  if (gasValue > GAS_THRESHOLD) {
    activateGasAlarm();
    displayAlert("Gas Leak Detected!");
  }

  // Speed sensor
  if (millis() - lastSpeedUpdate > SPEED_UPDATE_INTERVAL) {
    int speed = calculateSpeed();
    displaySpeedOnOLED(speed);
    lastSpeedUpdate = millis();
  }

  // GPS module
  readGPSData();
  displayGPSLocationOnSerialMonitor();

  // Pulse rate sensor
  if (millis() - lastPulseUpdate > PULSE_UPDATE_INTERVAL) {
    int pulseRate = calculatePulseRate();
    displayPulseRateOnOLED(pulseRate);
    lastPulseUpdate = millis();
  }
}

void activatePanicMode() {
  displayAlert("PANIC MODE ACTIVATED!");
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
}

void activateShockAlarm() {
  displayAlert("Shock Detected!");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void activateGasAlarm() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000, 200);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  delay(200);
}

void displaySpeedOnOLED(int speed) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Vehicle Monitoring");
  display.println("------------------");
  display.print("Speed: ");
  display.print(speed);
  display.println(" km/h");
  display.display();
}

void readGPSData() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // Successfully received GPS data
    }
  }
}

void displayGPSLocationOnSerialMonitor() {
  if (gps.location.isValid()) {
    Serial.print("Location: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Speed: ");
    Serial.print(gps.speed.kmph());
    Serial.println(" km/h");
  } else {
    Serial.println("GPS not available");
  }
}

void displayPulseRateOnOLED(int pulseRate) {
  display.setCursor(0, 24);
  display.print("Pulse: ");
  display.print(pulseRate);
  display.println(" BPM");
  display.display();
}

int calculateSpeed() {
  // This is a simplified implementation
  // In a real application, you would measure pulses over time
  // from a wheel speed sensor or similar
  static unsigned long lastPulseTime = 0;
  unsigned long currentTime = millis();
  unsigned long pulseInterval = currentTime - lastPulseTime;
  lastPulseTime = currentTime;
  
  if (pulseInterval == 0) return 0;
  
  // Assuming 1 pulse per revolution and wheel circumference of 2 meters
  // Convert to km/h: (2m * 3.6) / (pulseInterval in hours)
  float speed = (2 * 3.6) / (pulseInterval / 3600000.0);
  return (int)speed;
}

int calculatePulseRate() {
  // This is a simplified implementation
  // In a real application, you would use proper pulse detection
  int sensorValue = analogRead(PULSE_RATE_SENSOR_PIN);
  
  // Simple threshold detection
  if (sensorValue > 512 && !pulseState) {
    pulseState = true;
    pulseCount++;
  } else if (sensorValue <= 512 && pulseState) {
    pulseState = false;
  }
  
  // Calculate BPM based on counts over time
  unsigned long elapsed = millis() - lastPulseUpdate;
  if (elapsed >= 10000) { // Update every 10 seconds for better accuracy
    int bpm = (pulseCount * 60000) / elapsed;
    pulseCount = 0;
    return constrain(bpm, 40, 200); // Reasonable range for human pulse
  }
  return 0; // Return 0 if not enough time has passed
}

void displayAlert(const char* message) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("ALERT!");
  display.println(message);
  display.display();
}