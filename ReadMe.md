Of course. Here is the README file with all citations removed.

-----

# IoT Home Security & Access Control System

A two-node, ESP8266-based home security system featuring real-time monitoring, intrusion detection, and RFID access control managed via a web dashboard.

| Node 1 - Main Server & Monitoring | Node 2 - Access Control |
| :---: | :---: |
|  |  |

-----

## Features

  * **Web Dashboard:** Real-time dashboard for monitoring sensor data and security status.
  * **Intrusion Detection:** Monitors motion (ultrasonic), loud sounds, fire (flame sensor), and window tampering (touch sensor).
  * **Access Control:** Uses an MFRC522 RFID reader to grant or deny access, controlling a servo motor lock.
  * **Environmental Monitoring:** Tracks ambient temperature, humidity, and room brightness.
  * **Remote Control:** Enable, disable, and clear alerts directly from the web interface.

## System Architecture

The project uses two ESP8266 nodes that communicate over WiFi.

  * **Node 1 (`node1.ino`):** Acts as the web server and central controller. It collects data from its sensors and receives updates from Node 2 to display on the dashboard.
  * **Node 2 (`node2.ino`):** Manages door access and window security. It sends HTTP requests to Node 1 whenever an event (like an RFID scan or touch detection) occurs.

## Hardware & Wiring

### Node 1 (Server)

| Component | Pin |
|---|---|
| Ultrasonic (Trig / Echo) | D4 / D8 |
| Sound Sensor (DO) | D2 |
| Flame Sensor (A0) | A0 |
| DHT11 Sensor | D6 |
| RGB LED (R,G,B) | D0, D3, D1 |

### Node 2 (Access Control)

| Component | Pin |
|---|---|
| RFID (SDA, SCK, MOSI, MISO, RST) | D8, D5, D7, D6, D4 |
| Servo Motor | D2 |
| Touch Sensor | D1 |
| Photoresistor (LDR) | A0 |

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
      * Wait for it to connect to your WiFi and note the IP address it prints.

4.  **Configure and Upload to Node 2:**

      * In `node2.ino`, update the `node1_ip` variable with the IP address from the previous step.

    <!-- end list -->

    ```cpp
    const char* node1_ip = "http://192.168.32.138/update"; // <-- USE NODE 1's IP
    ```

      * In `node2.ino`, find the `checkRFID()` function and change the authorized tag ID to match your RFID card/tag's UID.

    <!-- end list -->

    ```cpp
    if (tagID == "8edffb3") { // <-- REPLACE WITH YOUR RFID TAG's UID
      // ...
    }
    ```

      * Upload `node2.ino` to the second ESP8266.

## Usage

Once both nodes are running, open a web browser and navigate to the IP address of Node 1. The dashboard will display all sensor readings and status updates. You can toggle the security system on/off or clear active alerts using the buttons at the bottom of the page.