# I²C Sensor & Control Network

A dual-node embedded communication system demonstrating the implementation of the **I²C protocol** using an **ESP32** (Master) and an **Arduino Uno** (Slave). The project features bidirectional communication, real-time temperature monitoring, intelligent relay control, packet-based communication, and fault detection mechanisms.

---

# System Overview

## ESP32 Master

- Operates as the I²C Master
- Requests telemetry from the Arduino Uno
- Processes temperature data
- Implements hysteresis-based control
- Sends relay commands back to the slave
- Displays live system diagnostics

## Arduino Uno Slave

- Operates as the I²C Slave
- Reads temperature from the LM35 sensor
- Packages telemetry into structured packets
- Responds to master requests
- Controls two relay outputs
- Reports local system status

---

# Communication Protocol

## Sensor Packet (Arduino → ESP32)

| Byte | Field           | Description              |
|------|-----------------|--------------------------|
| 0    | Header          | Packet Identifier (0xAA) |
| 1    | Sequence Number | Packet Counter           |
| 2    | Temperature     | Temperature (°C)         |
| 3    | Relay 1 State   | Current Relay Status     |
| 4    | Relay 2 State   | Current Relay Status     |
| 5    | Checksum        | XOR Error Detection      |

---

## Command Packet (ESP32 → Arduino)

| Byte | Field           | Description              |
|------|-----------------|--------------------------|
| 0    | Header          | Packet Identifier (0xBB) |
| 1    | Sequence Number | Packet Counter           |
| 2    | Relay 1 Command | ON / OFF                 |
| 3    | Relay 2 Command | ON / OFF                 |
| 4    | Checksum        | XOR Error Detection      |

---

# Protection Logic

| Parameter                  | Value |
|----------------------------|------:|
| High Temperature Threshold | 38°C  |
| Recovery Threshold         | 35°C  |
| Hysteresis Window          | 3°C   |

The hysteresis algorithm prevents relay chatter by introducing a recovery threshold before switching the system back to its normal operating state.

---

# Features

- I²C Master-Slave Communication
- Bidirectional Data Exchange
- Structured Packet-Based Communication
- Packet Header Validation
- XOR Checksum Verification
- Sequence Number Tracking
- Lost Packet Detection
- Hysteresis-Based Relay Control
- Real-Time Temperature Monitoring
- Dual Relay Control
- Modular Embedded Firmware
- Serial Monitor Diagnostics

---

# Getting Started

## Required Library

- Wire (Built-in Arduino Library)

---

## Execution Steps

1. Upload the Arduino Uno slave firmware.
2. Upload the ESP32 master firmware.
3. Open both Serial Monitors at **115200 baud**.
4. Observe:
   - Live temperature readings
   - Relay state changes
   - Packet sequence numbers
   - Communication diagnostics

---

# Repository Structure

```

**I2C\_Sensor\_Network**

│

├── Source\_Code

├── Hardware\_Connections

├── Images

└── README.md

```

---

# Project Outputs

- Hardware Setup
- ESP32 Serial Monitor
- Arduino Serial Monitor
- Relay Operation
- Temperature Monitoring

---

# Concepts Demonstrated

- I²C Communication
- Master-Slave Architecture
- Embedded Networking
- Packet Framing
- Error Detection
- Data Integrity
- Sensor Interfacing
- Relay Control
- State Machine Design
- Real-Time Embedded Systems

---

## Future Improvements

- Multi-Slave I²C Network
- OLED Status Display
- EEPROM Data Logging
- CRC Error Detection
- FreeRTOS Integration
- IoT Cloud Monitoring
