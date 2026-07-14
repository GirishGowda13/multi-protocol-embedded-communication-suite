#include <SPI.h>
#include <mcp_can.h>

// --- Hardware Pin Configurations ---
const int SPI_CS_PIN = 10;      // Standard Chip Select pin for Uno CAN Shield
const int CAN_INT_PIN = 2;      // CAN Interrupt Pin connected to Digital Pin 2

MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  // Open the high-speed data pipeline to your computer
  Serial.begin(115200);
  pinMode(CAN_INT_PIN, INPUT);

  // Initialize MCP2515 CAN bus at 500kbps (Must match the ESP32)
  // Note: Most standard Uno shields use a 16MHz crystal. 
  while (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {
    Serial.println("Uno CAN Init Failure... Check your wiring and power!");
    delay(500);
  }
  Serial.println(">>> Uno CAN Network Online. Awaiting Telemetry... <<<");
  
  // Set to normal operation mode to listen to the bus traffic
  CAN.setMode(MCP_NORMAL);
}

void loop() {
  // Check if the MCP2515 INT pin has dropped LOW (signaling a new packet has arrived)
  if (!digitalRead(CAN_INT_PIN)) {
    long unsigned int rxId;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    // Pull the raw 8-byte message out of the hardware buffer
    CAN.readMsgBuf(&rxId, &len, rxBuf);

    // Filter for our exact engine telemetry packet ID
    if (rxId == 0x400) {
      
      // 1. Reconstruct the 32-bit Float (Engine Temperature)
      float receivedTemp;
      memcpy(&receivedTemp, &rxBuf[0], sizeof(receivedTemp));

      // 2. Bitwise Re-stitching: Recombine the two 8-bit pieces into one 16-bit Int (RPM)
      // Take Byte 6, shift it left 8 spaces, and combine it with Byte 7 using a bitwise OR (|)
      int receivedRPM = (rxBuf[6] << 8) | rxBuf[7];

      // 3. Print the formatted data stream to the Serial Monitor
      Serial.print("[ECU TELEMETRY] -> ");
      Serial.print("RPM: ");
      Serial.print(receivedRPM);
      Serial.print(" rpm  |  Coolant Temp: ");
      Serial.print(receivedTemp, 1);
      Serial.println(" °C");
    }
  }
}