# IoT Home Security & Access Control System

[cite\_start]A two-node, ESP8266-based home security system featuring real-time monitoring, intrusion detection, and RFID access control managed via a web dashboard[cite: 109, 110].

| Node 1 - Main Server & Monitoring | Node 2 - Access Control |
| :---: | :---: |
|  |  |

-----

## Features

  * [cite\_start]**Web Dashboard:** Real-time dashboard for monitoring sensor data and security status[cite: 142].
  * [cite\_start]**Intrusion Detection:** Monitors motion (ultrasonic), loud sounds, fire (flame sensor), and window tampering (touch sensor)[cite: 111, 112, 144].
  * [cite\_start]**Access Control:** Uses an MFRC522 RFID reader to grant or deny access, controlling a servo motor lock[cite: 112, 143].
  * [cite\_start]**Environmental Monitoring:** Tracks ambient temperature, humidity, and room brightness[cite: 111, 113, 147].
  * [cite\_start]**Remote Control:** Enable, disable, and clear alerts directly from the web interface[cite: 51, 52, 65, 67].

## System Architecture

[cite\_start]The project uses two ESP8266 nodes that communicate over WiFi[cite: 110].

  * [cite\_start]**Node 1 (`node1.ino`):** Acts as the web server and central controller[cite: 111, 153]. It collects data from its sensors and receives updates from Node 2 to display on the dashboard.
  * [cite\_start]**Node 2 (`node2.ino`):** Manages door access and window security[cite: 112, 154]. [cite\_start]It sends HTTP requests to Node 1 whenever an event (like an RFID scan or touch detection) occurs[cite: 163].

## Hardware & Wiring

### Node 1 (Server)

| Component | Pin |
|---|---|
| Ultrasonic (Trig / Echo) | [cite\_start]D4 / D8 [cite: 4] |
| Sound Sensor (DO) | [cite\_start]D2 [cite: 5] |
| Flame Sensor (A0) | [cite\_start]A0 [cite: 6] |
| DHT11 Sensor | [cite\_start]D6 [cite: 5] |
| RGB LED (R,G,B) | [cite\_start]D0, D3, D1 [cite: 6, 7, 8] |

### Node 2 (Access Control)

| Component | Pin |
|---|---|
| RFID (SDA, SCK, MOSI, MISO, RST) | [cite\_start]D8, D5, D7, D6, D4 [cite: 202] |
| Servo Motor | [cite\_start]D2 [cite: 74, 203] |
| Touch Sensor | [cite\_start]D1 [cite: 73, 204] |
| Photoresistor (LDR) | [cite\_start]A0 [cite: 74, 205] |

## Software & Setup

### Prerequisites

  * [Arduino IDE](https://www.arduino.cc/en/software)
  * ESP8266 Board Manager for Arduino IDE

### Required Libraries

**Node 1:**

  * `ESP8266WiFi`
  * `ESP8266WebServer`
  * `DHT sensor library`

**Node 2:**

  * `ESP8266WiFi`
  * `ESP8266HTTPClient`
  * `Servo`
  * `MFRC522` (by GithubCommunity)
  * `SPI`

### Installation Guide

1.  **Assemble the Hardware:** Wire both nodes as specified in the [Hardware & Wiring](https://www.google.com/search?q=%23hardware--wiring) section.

2.  **Configure Network:** In both `node1.ino` and `node2.ino`, update the WiFi credentials.

    ```cpp
    const char* ssid = "Yikes"; // <-- YOUR WIFI NAME
    const char* password = "12345678"; // <-- YOUR WIFI PASSWORD
    ```

3.  **Upload to Node 1:**

      * Upload `node1.ino` to the first ESP8266.
      * Open the Arduino Serial Monitor (Baud Rate: 9600).
      * [cite\_start]Wait for it to connect to your WiFi and note the IP address it prints[cite: 21].

4.  **Configure and Upload to Node 2:**

      * [cite\_start]In `node2.ino`, update the `node1_ip` variable with the IP address from the previous step[cite: 71].

    <!-- end list -->

    ```cpp
    const char* node1_ip = "http://192.168.32.138/update"; // <-- USE NODE 1's IP
    ```

      * [cite\_start]In `node2.ino`, find the `checkRFID()` function and change the authorized tag ID to match your RFID card/tag's UID[cite: 87, 312].

    <!-- end list -->

    ```cpp
    if (tagID == "8edffb3") { // <-- REPLACE WITH YOUR RFID TAG's UID
      // ...
    }
    ```

      * Upload `node2.ino` to the second ESP8266.

## Usage

Once both nodes are running, open a web browser and navigate to the IP address of Node 1. The dashboard will display all sensor readings and status updates. [cite\_start]You can toggle the security system on/off or clear active alerts using the buttons at the bottom of the page[cite: 65, 67].