#include <ESP8266WiFi.h>         // Library for Wi-Fi connection
#include <ESP8266HTTPClient.h>   // Library to handle HTTP requests
#include <Servo.h>               // Library to control the servo motor
#include <MFRC522.h>             // Library for RFID reader
#include <SPI.h>                 // Library for SPI communication (required for RFID)

// Wi-Fi credentials
const char* ssid = "Yikes";
const char* password = "12345678";

// IP address of Node 1 for sending updates
const char* node1_ip = "http://192.168.32.138/update";

// RFID setup
#define SS_PIN D8  // Slave Select pin for RFID
#define RST_PIN D4 // Reset pin for RFID
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create RFID instance

// Servo setup
Servo myServo; // Create servo object

// Sensor pin assignments
const int touchSensor = D1;      // Touch sensor pin for window intrusion detection
const int servoPin = D2;         // Servo motor control pin
const int photoResistor = A0;    // Photoresistor pin for brightness detection

// Flags for status tracking
bool intrusionSent = false;      // Prevents repeated intrusion alerts
bool doorOpen = false;          // Tracks door status

// Wi-Fi client for HTTP requests
WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(9600);            // Initialize serial communication for debugging
  WiFi.begin(ssid, password);    // Connect to Wi-Fi

  // Initialize RFID reader
  SPI.begin();                   // Start SPI bus
  mfrc522.PCD_Init();            // Initialize RFID module

  // Initialize servo and set to locked position
  myServo.attach(servoPin);
  myServo.write(0);              // Door locked

  // Set sensor pin modes
  pinMode(touchSensor, INPUT);
  pinMode(photoResistor, INPUT);

  // Wait until Wi-Fi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting...");
  }

  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP()); // Print local IP address
}

void loop() {
  checkRFID();            // Check for RFID card presence and handle door unlocking
  checkTouchSensor();     // Check touch sensor for window intrusion
  sendBrightnessStatus(); // Read and send room brightness to Node 1
  delay(2000);            // Wait 2 seconds before next iteration
}

// Function to check RFID tag and unlock door if authorized
void checkRFID() {
  // Check for new RFID card
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  String tagID = "";
  // Read UID from RFID card
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tagID += String(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.print("Scanned Tag: ");
  Serial.println(tagID);

  // Check if the scanned RFID tag is authorized
  if (tagID == "8edffb3") {
    Serial.println("Access Granted!");
    sendData("rfid=Access Granted"); // Notify Node 1 of granted access
    unlockDoor();                    // Unlock door with servo
  } else {
    Serial.println("Access Denied!");
    sendData("rfid=Access Denied");  // Notify Node 1 of denied access
  }
}

// Unlocks the door and re-locks after a delay
void unlockDoor() {
  myServo.write(90);                 // Rotate servo to unlock door
  doorOpen = true;                   // Update door status
  sendData("door=Opened");           // Notify Node 1 door is opened
  delay(5000);                       // Keep door open for 5 seconds
  myServo.write(0);                  // Rotate servo to lock door
  doorOpen = false;                  // Update door status
  sendData("door=Closed");           // Notify Node 1 door is closed
}

// Detects window intrusion via touch sensor
void checkTouchSensor() {
  int touchState = digitalRead(touchSensor); // Read touch sensor state

  // If intrusion detected and not previously sent, send alert
  if (touchState == HIGH && !intrusionSent) {
    Serial.println("Window Intrusion Detected!");
    sendData("intrusion=touch");     // Notify Node 1 of window intrusion
    intrusionSent = true;            // Prevent repeated alerts
  } 
  // Reset intrusion flag when no touch detected
  else if (touchState == LOW) {
    intrusionSent = false;
    sendData("intrusion=none");      // Notify Node 1 intrusion cleared
  }
}

// Reads photoresistor and sends brightness status to Node 1
void sendBrightnessStatus() {
  int lightValue = analogRead(photoResistor); // Read brightness level
  String brightness = (lightValue > 500) ? "Bright" : "Dark"; // Determine brightness state
  sendData("brightness=" + brightness); // Send brightness info to Node 1
}

// Sends HTTP POST request to Node 1 with data
void sendData(String data) {
  if (WiFi.status() == WL_CONNECTED) {          // Check Wi-Fi connection
    http.begin(client, node1_ip);              // Begin HTTP connection
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Set header
    int httpResponseCode = http.POST(data);    // Send POST request with data
    http.end();                                // End HTTP request
    
    // Debugging output
    Serial.print("Sent: ");
    Serial.println(data);
    Serial.print("Response Code: ");
    Serial.println(httpResponseCode);          // Display response code
  }
}
