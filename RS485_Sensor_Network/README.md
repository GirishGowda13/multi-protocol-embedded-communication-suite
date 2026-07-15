# RS485 Sensor Network using ESP32 and Arduino UNO

## Overview

This project demonstrates RS485-based serial communication between an ESP32 (Master) and an Arduino UNO (Slave). The Arduino acquires sensor data from the LM35 temperature sensor and INA219 voltage/current sensor and transmits it to the ESP32 through MAX485 transceivers. The ESP32 receives the packet, extracts the sensor values, and displays them on the Serial Monitor.

---

## Features

- ESP32 as RS485 Master
- Arduino UNO as RS485 Slave
- UART communication using MAX485 modules
- LM35 temperature measurement
- INA219 voltage, current, and power monitoring
- Structured packet-based communication
- Sensor data parsing on ESP32

---

## Hardware Used

- ESP32 Dev Module
- Arduino UNO
- 2 × MAX485 RS485 Modules
- LM35 Temperature Sensor
- INA219 Current & Voltage Sensor
- Breadboard
- Jumper Wires

---

## Software Used

- Arduino IDE
- ESP32 Arduino Core

---

## Folder Structure

```
RS485-Sensor-Network/
│
├── ESP32_Master/
├── Arduino_Slave/
├── images/
├── Circuit_Description/
├── README.md


---

## Communication Packet

Example packet transmitted from Arduino to ESP32:

```
<TEMP:28.5,VOLT:12.10,CURR:245.30,POWER:2968.50>
```

---

## Project Workflow

1. ESP32 sends a data request over RS485.
2. Arduino receives the request.
3. Arduino reads the connected sensors.
4. Sensor values are packed into a formatted string.
5. The packet is transmitted through RS485.
6. ESP32 receives the packet.
7. ESP32 extracts and displays the sensor values.


## Future Improvements

- Modbus RTU implementation
- CRC error checking
- OLED display
- SD card data logging
- IoT dashboard integration
- MQTT support
