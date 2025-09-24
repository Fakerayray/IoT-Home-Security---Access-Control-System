#include <ESP8266WiFi.h>           // WiFi library for ESP8266
#include <ESP8266WebServer.h>      // Web server library for ESP8266
#include "DHT.h"                   // DHT sensor library for temperature and humidity

// ----------------------------- WiFi Setup -----------------------------
const char* ssid = "Yikes";        // WiFi network name (SSID)
const char* password = "12345678"; // WiFi password

ESP8266WebServer server(80);       // Create a web server object on port 80

// ----------------------------- Sensor & Component Pins -----------------------------
const int trigPin = D4;           // Ultrasonic sensor trigger pin
const int echoPin = D8;           // Ultrasonic sensor echo pin
const int temp_hum_pin = D6;      // DHT11 temperature and humidity sensor pin
const int soundPin = D2;          // Sound sensor pin
const int flamePin = A0;          // Flame sensor pin (analog)
const int red_led = D0;           // Red RGB LED pin (optional visual alert)
const int green_led = D3;         // Green RGB LED pin (optional visual status)
const int blue_led = D1;          // Blue RGB LED pin (optional visual status)

// ----------------------------- Variables -----------------------------
long duration;                    // Stores ultrasonic sensor pulse duration
int distance;                     // Calculated distance from ultrasonic sensor
int temperature;                  // Measured temperature
int humidity;                     // Measured humidity
int flameVal;                     // Analog value from flame sensor
bool intrusionEnabled = true;     // Toggle for enabling/disabling intrusion detection
bool motionAlert = false;         // Motion detection alert flag
bool windowAlert = false;         // Window intrusion alert flag
bool soundAlert = false;          // Sound detection alert flag
bool fireAlert = false;           // Fire detection alert flag
bool alertActive = false;         // Global flag to indicate an active alert
String doorStatus = "Closed";     // Status of the door (updated by Node 2)
String rfidMessage = "No Access Attempt";  // RFID access message (updated by Node 2)
String brightnessStatus = "Unknown";       // Brightness status from photoresistor (updated by Node 2)

DHT dht(temp_hum_pin, DHT11);      // Initialize DHT sensor

// ----------------------------- Setup Function -----------------------------
void setup() {
  WiFi.begin(ssid, password);     // Connect to WiFi network
  dht.begin();                    // Start DHT sensor

  // Set pin modes
  pinMode(trigPin, OUTPUT);       
  pinMode(echoPin, INPUT);
  pinMode(soundPin, INPUT);
  pinMode(flamePin, INPUT);
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);

  Serial.begin(9600);             // Start serial communication for debugging
  
  // Wait until connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to connect...");
  }

  // Print device IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Define server routes
  server.on("/", get_index);           // Route for main dashboard
  server.on("/toggle", toggleIntrusion); // Route to toggle intrusion detection
  server.on("/update", handleUpdate);    // Route to receive updates from Node 2
  server.on("/clear_alerts", clearAlerts); // Route to clear alerts

  server.begin();                    // Start web server
  Serial.println("Server listening");
}

// ----------------------------- Main Loop -----------------------------
void loop() {
  server.handleClient();             // Handle incoming web requests
  distanceCentimeter();              // Check for motion using ultrasonic sensor
  readTempHum();                     // Read temperature and humidity
  detectSound();                     // Detect sound
  detectFlame();                     // Detect flame
  delay(3000);                       // Delay to avoid excessive reads
}

// ----------------------------- Motion Detection -----------------------------
void distanceCentimeter() {
  digitalWrite(trigPin, LOW);        // Ensure trigger is LOW initially
  delayMicroseconds(2);              // Short delay for stable signal
  digitalWrite(trigPin, HIGH);       // Send ultrasonic pulse
  delayMicroseconds(10);             // Pulse duration
  digitalWrite(trigPin, LOW);        // End pulse

  duration = pulseIn(echoPin, HIGH); // Read echo duration
  distance = (duration * 0.034) / 2; // Convert duration to distance in cm

  Serial.print("Distance: ");
  Serial.println(distance);

  // If an object is within 5 cm, trigger motion alert
  if (distance <= 5) {
    motionAlert = true;
    alertActive = true;             // Keep alert active until cleared
  }
}

// ----------------------------- Temperature & Humidity -----------------------------
void readTempHum() {
  temperature = dht.readTemperature(); // Read temperature
  humidity = dht.readHumidity();       // Read humidity

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
}

// ----------------------------- Sound Detection -----------------------------
void detectSound() {
  int soundState = digitalRead(soundPin); // Read sound sensor state
  if (soundState == HIGH) {               // If sound is detected
    Serial.println("Sound detected!");
    soundAlert = true;                    // Set alert flag
    alertActive = true;                   // Keep alert active
  }
}

// ----------------------------- Flame Detection -----------------------------
void detectFlame() {
  flameVal = analogRead(flamePin);        // Read analog value from flame sensor
  Serial.print("Flame Value: ");
  Serial.println(flameVal);

  if (flameVal > 100) {                   // Threshold for detecting fire
    Serial.println("Fire detected in your house!");
    fireAlert = true;                     // Trigger fire alert
    alertActive = true;                   // Activate global alert
  }
}

// ----------------------------- Handle Updates from Node 2 -----------------------------
void handleUpdate() {
  // Update door status
  if (server.hasArg("door")) {
    doorStatus = server.arg("door");
  }

  // Update RFID message
  if (server.hasArg("rfid")) {
    rfidMessage = server.arg("rfid");
  }

  // Handle window intrusion (touch sensor)
  if (server.hasArg("intrusion")) {
    String intrusionType = server.arg("intrusion");
    if (intrusionType == "touch") {
      windowAlert = true;
      alertActive = true;
    }
  }

  // Update brightness status from Node 2
  if (server.hasArg("brightness")) {
    brightnessStatus = server.arg("brightness");
  }

  server.send(200, "text/plain", "Updated"); // Respond to Node 2
}

// ----------------------------- Clear Alerts -----------------------------
void clearAlerts() {
  motionAlert = false;          // Clear motion alert
  windowAlert = false;          // Clear window intrusion alert
  soundAlert = false;           // Clear sound alert
  fireAlert = false;            // Clear fire alert
  alertActive = false;          // Reset global alert flag

  // Redirect to homepage after clearing alerts
  server.sendHeader("Location", "/");
  server.send(303);
}

// ----------------------------- Toggle Intrusion Detection -----------------------------
void toggleIntrusion() {
  intrusionEnabled = !intrusionEnabled; // Toggle intrusion detection state
  Serial.print("Intrusion Detection: ");
  Serial.println(intrusionEnabled ? "Enabled" : "Disabled");

  // Redirect to homepage after toggling
  server.sendHeader("Location", "/");
  server.send(303);
}

// ----------------------------- Webpage Generation -----------------------------
void get_index() {
  // Set background color: red if alert is active and security is enabled, white otherwise
  String bgColor = (alertActive && intrusionEnabled) ? "red" : "white";  

  // Build HTML content for the dashboard
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta http-equiv='refresh' content='1'></head>"; // Auto-refresh page every second
  html += "<body style='background:" + bgColor + "; text-align:left; padding:20px; font-size:24px;'>";

  html += "<h1>Intrusion and Access Control Dashboard</h1>";
  html += "<p><strong>Temperature: </strong>" + String(temperature) + " °C</p>";
  html += "<p><strong>Humidity: </strong>" + String(humidity) + " %</p>";
  html += "<p><strong>Distance: </strong>" + String(distance) + " cm</p>";
  html += "<p><strong>Flame Value: </strong>" + String(flameVal) + "</p>";
  html += "<p><strong>Room Brightness: </strong>" + brightnessStatus + "</p>";
  html += "<p><strong>Door Status: </strong>" + doorStatus + "</p>";
  html += "<p><strong>RFID Access: </strong>" + rfidMessage + "</p>";

  // Show alerts if intrusion detection is enabled
  if (alertActive && intrusionEnabled) {
    html += "<div style='color:white; background:red; padding:10px; font-size:28px;'>";
    html += "<strong>ALERT! </strong>";

    // Display relevant alerts
    if (motionAlert) html += "Motion Detected! ";
    if (windowAlert) html += "Window Intrusion Detected! ";
    if (soundAlert) html += "Loud Sound Detected! ";
    if (fireAlert) html += "Fire Detected in Your House! ";

    html += "</div>";
    html += "<form action='/clear_alerts' method='GET'><button type='submit' style='font-size:50px;'>Clear Alerts</button></form>";
  } else if (!intrusionEnabled) {
    html += "<p style='color:green; font-size:28px;'>Security System is OFF</p>";
  }

  // Security system toggle button
  String buttonText = intrusionEnabled ? "Turn Off Security" : "Turn On Security";
  html += "<form action='/toggle' method='GET'><button type='submit' style='font-size:50px;'>" + buttonText + "</button></form>";

  html += "</body></html>";

  // Send the generated HTML page to the client
  server.send(200, "text/html", html);
}
