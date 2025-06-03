##📌 Overview
An advanced vehicle safety system combining IoT sensors and embedded controls to monitor driver health, environmental conditions, and vehicle status in real-time. The system provides visual and audible alerts for potential hazards.

##✨ Key Features
Multi-sensor Integration: Rain, shock, gas, speed, and pulse rate monitoring
Emergency Panic Button: Immediate alert activation
Real-time GPS Tracking: Location and speed monitoring
OLED Display: Clear status visualization
Audible/Visual Alarms: Buzzer and LED indicators for warnings
![Uploading Prototype Model.png…]()

🛠 Hardware Components
Component	Pin Connection	Purpose
Rain Sensor	A2	Detect precipitation
Shock Sensor	A3	Detect collisions
MQ Gas Sensor	A1	Detect hazardous gases
Panic Button	7	Manual emergency trigger
LED Indicator	3	Visual alerts
Buzzer	6	Audible warnings
Servo Motor	9	Automatic rain response
Speed Sensor	10	Vehicle speed measurement
Pulse Sensor	A0	Driver heart rate monitoring
GPS Module	RX:0, TX:1	Location tracking
OLED Display	I2C	System status display

##🧠 System Architecture
```
// Core Safety Functions
void activatePanicMode() {
  // Visual/audible alarm pattern
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
}

// Environmental Monitoring
void checkRainSensor() {
  if (digitalRead(RAIN_SENSOR_PIN) {
    rainServo.write(90); // Activate rain mode
    displayAlert("Rain Detected!");
  }
}

// Driver Health Monitoring
int calculatePulseRate() {
  // Pulse detection algorithm
  return (pulseCount * 60000) / elapsed;
}
```
##⚙️ System Parameters
Parameter	Value	Description
Gas Threshold	300	Air quality alert level
Panic Button Delay	5000ms	Anti-glitch debounce
Pulse Update Interval	1000ms	Health monitoring frequency
Speed Update Interval	500ms	Velocity sampling rate
Servo Rain Position	90°	Wiper activation angle

##🚀 Getting Started
1. Hardware Setup
Connect all sensors to specified pins
Mount OLED display via I2C
Install GPS module with proper antenna placement
Position panic button within driver's reach
![System_Architecture](https://github.com/user-attachments/assets/01838c89-ccd9-42b3-91c5-8c1a1493802b)

2. Software Installation
```
# Required Libraries
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
```
3. System Operation
Upload Smart_Safety_Car.ino to Arduino
Monitor serial output at 9600 baud
System auto-initializes with OLED confirmation

Alert Patterns:
Rain: Servo activation + display alert
Shock: Triple flash/beep pattern
Gas: Continuous blinking + tone

🛡 Safety Protocols
1. Automatic Responses:
Rain detection → Wiper activation
Gas leak → Immediate alert
Shock impact → Emergency flashers

2. Manual Override:
5-second panic button hold
Dedicated alert sequence

##📋 Requirements
```
Hardware:
- Arduino Uno/Mega
- SSD1306 OLED (128x64)
- NEO-6M GPS Module
- Various sensors (see Hardware Components)

Software:
- Arduino IDE
- Adafruit_GFX Library
- Adafruit_SSD1306 Library
- TinyGPS++ Library
```

##🌟 Future Enhancements
Cloud connectivity for remote monitoring
Machine learning for predictive hazard detection
CAN bus integration for vehicle diagnostics
Mobile app companion for alerts



