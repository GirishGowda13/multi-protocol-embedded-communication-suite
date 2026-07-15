# ESP32 UART Transmitter

This project demonstrates UART transmission using an ESP32. Messages entered through the Serial Monitor are formatted with a packet number, timestamp, and message length before being transmitted over the UART interface.

## Features

- UART Data Transmission
- Packet Number Tracking
- Transmission Timestamp
- Message Length Calculation
- Formatted Serial Output

## Requirements

- ESP32 Development Board
- Arduino IDE
- Baud Rate: **115200**

## Working

1. Enter a message in the Serial Monitor.
2. Press **Enter**.
3. The ESP32 formats the message into a packet.
4. The packet is transmitted via UART.

## Concepts Demonstrated

- UART Communication
- Serial Data Transmission
- Packet Formatting
- String Processing
- Embedded Timing (`millis()`)
