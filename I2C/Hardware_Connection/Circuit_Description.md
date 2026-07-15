# Hardware Connections

This document describes the complete hardware wiring required for the Intelligent I²C Sensor & Control Network.

---

# System Hardware

- ESP32 DevKit V1
- Arduino Uno
- LM35 Temperature Sensor
- 2-Channel Relay Module
- Breadboard
- Jumper Wires

---

# ESP32 ↔ Arduino Uno (I²C Bus)

| ESP32 Pin | Arduino Uno Pin | Signal | Description             |
|-----------|-----------------|--------|-------------------------|
| GPIO21    | A4              | SDA    | Serial Data Line        |
| GPIO22    | A5              | SCL    | Serial Clock Line       |
| GND       | GND             | Ground | Common Ground Reference |

---

# LM35 Temperature Sensor

| LM35 Pin  | Arduino Uno Pin | Description               |
|-----------|-----------------|---------------------------|
| VCC       | 5V              | Power Supply              |
| OUT       | A0              | Analog Temperature Output |
| GND       | GND             | Ground                    |

---

# 2-Channel Relay Module

| Relay Pin | Arduino Uno Pin | Description             |
|-----------|-----------------|-------------------------|
| VCC       | 5V              | Relay Module Power      |
| GND       | GND             | Common Ground           |
| IN1       | D7              | Relay Channel 1 Control |
| IN2       | D8              | Relay Channel 2 Control |

---

# Power Distribution

| Source      | Destination |
|-------------|-------------|
| Arduino 5V  | LM35 VCC    |
| Arduino 5V  | Relay VCC   |
| Arduino GND | LM35 GND    |
| Arduino GND | Relay GND   |
| Arduino GND | ESP32 GND   |

---

# Communication Architecture

```

\&#x20;          LM35

\&#x20;            │

\&#x20;            ▼

\&#x20;       Arduino Uno

\&#x20;       (I²C Slave)

\&#x20;            ▲

\&#x20;         SDA / SCL

\&#x20;            ▼

\&#x20;          ESP32

\&#x20;       (I²C Master)

\&#x20;            │

\&#x20;            ▼

\&#x20;     Relay Commands

\&#x20;            │

\&#x20;            ▼

\&#x20;     Arduino Relays

```

---

# I²C Bus Configuration

| Parameter          | Value        |
|--------------------|--------------|
| Master Device      | ESP32        |
| Slave Device       | Arduino Uno  |
| Slave Address      | 0x08         |
| SDA Line           | GPIO21 ↔ A4  |
| SCL Line           | GPIO22 ↔ A5  |
| Communication Mode | Master–Slave |
| Bus Type           | Two-Wire I²C |

---

# Notes

- Ensure a **common ground** between the ESP32 and Arduino Uno.
- Use short jumper wires to minimize signal noise.
- Keep the SDA and SCL connections secure to ensure reliable communication.
- The Arduino Uno operates as the I²C slave, while the ESP32 initiates all communication as the master.
- If your Arduino Uno board does not already provide them, use **4.7 kΩ pull-up resistors** from SDA to 5 V and SCL to 5 V. Many I²C      modules include these pull-ups, so avoid adding extra ones unless needed.

---

# Project Functionality

1. Arduino Uno continuously measures temperature using the LM35 sensor.
2. The ESP32 periodically requests telemetry over the I²C bus.
3. The Arduino responds with a structured sensor data packet.
4. The ESP32 evaluates the received data using hysteresis logic.
5. Based on the evaluation, the ESP32 sends relay control commands back to the Arduino.
6. The Arduino updates the relay outputs and reports the current system status.
