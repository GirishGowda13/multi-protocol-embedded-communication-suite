#  Hardware Connections

This document describes the complete hardware wiring for the CAN-Bus Telemetry Dashboard.

---

# ESP32 Node (Transmitter)

The ESP32 collects temperature data from the LM35 sensor, displays telemetry on the OLED, and communicates with the MCP2515 CAN controller over SPI.

## ESP32 Connections

| ESP32 Pin | Connected Device | Device Pin | Function     |
|-----------|------------------|------------|--------------|
| VIN       | MCP2515          | VCC        | Power        |
| VIN       | OLED             | VCC        | Power        |
| VIN       | LM35             | VCC        | Power        |
| GND       | MCP2515          | GND        | Ground       |
| GND       | OLED             | GND        | Ground       |
| GND       | LM35             | GND        | Ground       |
| GPIO23    | MCP2515          | MOSI (SI)  | SPI MOSI     |
| GPIO19    | MCP2515          | MISO (SO)  | SPI MISO     |
| GPIO18    | MCP2515          | SCK        | SPI Clock    |
| GPIO5     | MCP2515          | CS         | Chip Select  |
| GPIO21    | OLED             | SDA        | I²C Data     |
| GPIO22    | OLED             | SCL        | I²C Clock    |
| GPIO34    | LM35             | OUT        | Analog Input |

---

# Arduino Uno Node (Receiver)

The Arduino Uno receives CAN frames through the MCP2515 and displays the decoded telemetry via the Serial Monitor.

## Arduino Connections

| Arduino Pin | MCP2515 Pin | Function    |
|-------------|-------------|-------------|
| 5V          | VCC         | Power       |
| GND         | GND         | Ground      |
| D11         | MOSI (SI)   | SPI MOSI    |
| D12         | MISO (SO)   | SPI MISO    |
| D13         | SCK         | SPI Clock   |
| D10         | CS          | Chip Select |
| D2          | INT         | Interrupt   |

---

# CAN Bus Connections

| Node 1  | Node 2  |
|---------|---------|
| CAN_H   | CAN_H   |
| CAN_L   | CAN_L   |
| GND     | GND     |

---

# Important Notes

- Enable the **J1 jumper** on both MCP2515 modules to activate the **120 Ω termination resistors**.
- Ensure all devices share a common ground.
- Verify that both MCP2515 modules use the same CAN bitrate (500 kbps).
- The MCP2515 modules communicate with the microcontrollers using the **SPI protocol**, while data exchange between the two nodes occurs over the **CAN bus**.
