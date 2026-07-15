# Circuit Description

## ESP32 (Master)

- GPIO17 → MAX485 DI
- GPIO16 → MAX485 RO
- GPIO4 → MAX485 DE & RE
- VIN → MAX485 VCC
- GND → MAX485 GND

## Arduino UNO (Slave)

- D11 → MAX485 DI
- D10 → MAX485 RO
- D4 → MAX485 DE & RE

## LM35

- VCC → 5V
- OUT → A0
- GND → GND

## INA219

- SDA → A4
- SCL → A5
- VCC → 5V
- GND → GND

## RS485 Bus

- A ↔ A
- B ↔ B
- Common GND
