CAN-Bus Telemetry Dashboard

A dual-node automotive telemetry system using ESP32 and Arduino Uno over a CAN-Bus network. The ESP32 simulates engine data, hosts a web dashboard, and sends telemetry via CAN, while the Arduino Uno receives and displays the data through the Serial Monitor.

📌 System Overview

ESP32 (Transmitter)

Reads LM35 temperature sensor
Simulates engine RPM
Displays data on an SH1106 OLED
Hosts a Wi-Fi web dashboard
Sends telemetry over CAN-Bus (500 kbps)

Arduino Uno (Receiver)

Receives CAN messages
Reconstructs RPM data
Displays live telemetry at 115200 baud
🛠️ Hardware
ESP32 DevKit V1 ×1
Arduino Uno ×1
MCP2515 CAN Module ×2
LM35 Temperature Sensor ×1
SH1106/SSD1306 OLED Display ×1
🔌 Wiring
ESP32 Connections
ESP32	Device
VIN	MCP2515 VCC, OLED VCC, LM35 VCC
GND	All GND
GPIO23	MCP2515 SI (MOSI)
GPIO19	MCP2515 SO (MISO)
GPIO18	MCP2515 SCK
GPIO5	MCP2515 CS
GPIO21	OLED SDA
GPIO22	OLED SCL
GPIO34	LM35 OUT
Arduino Uno Connections
Arduino	MCP2515
5V	VCC
GND	GND
Pin 11	SI (MOSI)
Pin 12	SO (MISO)
Pin 13	SCK
Pin 10	CS
Pin 2	INT
CAN Bus
CAN_H ↔ CAN_H
CAN_L ↔ CAN_L
GND ↔ GND

Keep the 120 Ω termination jumpers enabled on both MCP2515 modules.

📦 CAN Frame (ID: 0x400)
Bytes	Data
0–3	Temperature (float)
4–5	Reserved
6–7	RPM (uint16_t)
🚀 Getting Started

Install these libraries:

mcp_can
Adafruit SH110X (or SSD1306)
Adafruit GFX

Upload the ESP32 transmitter code and Arduino Uno receiver code, then:

Open both Serial Monitors at 115200 baud.
Connect to the ESP32 Wi-Fi Access Point.
Open the ESP32 IP address (typically 192.168.4.1) in a browser.
Monitor live telemetry on the web dashboard, OLED display, and Arduino Serial Monitor.
