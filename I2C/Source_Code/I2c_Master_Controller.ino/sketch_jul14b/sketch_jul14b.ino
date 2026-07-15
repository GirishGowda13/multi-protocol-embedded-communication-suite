#include <Wire.h>

#define SLAVE_ADDRESS 0x08

// Handshake Identification Bytes
#define SENSOR_HEADER 0xAA
#define COMMAND_HEADER 0xBB

// Logic Engine Parameters
const byte TRIP_TEMP = 38;     
const byte RECOVERY_TEMP = 35; 
bool systemTripped = false;    

// Struct structures must match the Uno perfectly
struct __attribute__((packed)) SensorPacket {
  byte header;       
  byte seqNum;       
  byte temperature;  
  byte relay1;       
  byte relay2;       
  byte checksum;     
};

struct __attribute__((packed)) CommandPacket {
  byte header;       
  byte seqNum;       
  byte relay1Cmd;    
  byte relay2Cmd;    
  byte checksum;     
};

// Communication Metrics
uint32_t totalPolls = 0;
uint32_t badHeaders = 0;
uint32_t badChecksums = 0;
uint32_t missedPackets = 0;
byte lastTxSeqNum = 0;
byte lastRxSeqNum = 255;

byte calculateChecksum(const byte* data, size_t length) {
  byte check = 0;
  for (size_t i = 0; i < length; i++) {
    check ^= data[i];
  }
  return check;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Start I2C using ESP32 pins SDA=21, SCL=22
  
  Serial.println("\n==============================================");
  Serial.println("[SYSTEM MASTER INITIALIZED] ESP32 Control Hub");
  Serial.println("Evaluating incoming I2C packets with Checksums...");
  Serial.println("==============================================\n");
}

void loop() {
  totalPolls++;
  SensorPacket telemetry;

  // 1. Request raw frame from Target
  byte bytesReceived = Wire.requestFrom(SLAVE_ADDRESS, sizeof(SensorPacket));
  
  if (bytesReceived == sizeof(SensorPacket)) {
    Wire.readBytes((byte*)&telemetry, sizeof(SensorPacket));

    // Validation 1: Header Check
    if (telemetry.header != SENSOR_HEADER) {
      badHeaders++;
      Serial.printf("[COMM ERROR] Invalid header detected! (0x%02X)\n", telemetry.header);
    } 
    // Validation 2: Checksum validation
    else if (calculateChecksum((byte*)&telemetry, sizeof(SensorPacket) - 1) != telemetry.checksum) {
      badChecksums++;
      Serial.println("[COMM ERROR] Telemetry frame CRC checksum corrupt!");
    } 
    // Data validated as safe to read!
    else {
      // Sequence tracking check
      byte expectedRxSeq = (byte)(lastRxSeqNum + 1);
      if (lastRxSeqNum != 255 && telemetry.seqNum != expectedRxSeq) {
        int missed = (telemetry.seqNum > expectedRxSeq) ? (telemetry.seqNum - expectedRxSeq) : (256 - expectedRxSeq + telemetry.seqNum);
        missedPackets += missed;
        Serial.printf("[ALERT] Detected %d dropped frame(s) on bus!\n", missed);
      }
      lastRxSeqNum = telemetry.seqNum;

      // 2. Evaluate environmental conditions (Hysteresis engine)
      if (!systemTripped && telemetry.temperature >= TRIP_TEMP) {
        systemTripped = true;
      } else if (systemTripped && telemetry.temperature <= RECOVERY_TEMP) {
        systemTripped = false;
      }

      // 3. Assemble and transmit the Command Packet
      CommandPacket cmd;
      cmd.header = COMMAND_HEADER;
      cmd.seqNum = lastTxSeqNum++;
      if (systemTripped) {
        cmd.relay1Cmd = LOW;  // Cut active load power
        cmd.relay2Cmd = HIGH; // Engage cooling loop
      } else {
        cmd.relay1Cmd = HIGH; // Load online
        cmd.relay2Cmd = LOW;  // Keep cooling silent
      }
      cmd.checksum = calculateChecksum((byte*)&cmd, sizeof(CommandPacket) - 1);

      Wire.beginTransmission(SLAVE_ADDRESS);
      Wire.write((byte*)&cmd, sizeof(CommandPacket));
      byte i2cStatus = Wire.endTransmission();

      // 4. Output rich system diagnostics
      if (i2cStatus == 0) {
        printTelemetryMonitor(telemetry);
      } else {
        Serial.printf("[I2C BUS ERROR] Transmit code failed: %d\n", i2cStatus);
      }
    }
  } else {
    Serial.println("[TIMEOUT ERROR] Failed to fetch telemetry frame from Target node.");
  }

  delay(1000); // Polling window
}

void printTelemetryMonitor(const SensorPacket& data) {
  Serial.printf("Telemetry | Temp: %d°C | R1 Pin: %s | R2 Pin: %s\n", 
                data.temperature, 
                data.relay1 == HIGH ? "ON" : "OFF", 
                data.relay2 == HIGH ? "ON" : "OFF");
  Serial.printf("Master State: %s | Polls: %d | Bad Chk: %d | Dropped: %d\n", 
                systemTripped ? "!!! TRIPPED (OVERHEAT) !!!" : "NOMINAL (OK)", 
                totalPolls, 
                badChecksums, 
                missedPackets);
  Serial.println("------------------------------------------------------------");
}