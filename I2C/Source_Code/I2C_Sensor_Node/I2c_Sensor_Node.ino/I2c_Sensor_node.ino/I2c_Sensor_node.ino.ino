#include <Wire.h>

#define SLAVE_ADDRESS 0x08

// Hardware Interfaces
#define LM35_PIN A0
#define RELAY1_PIN 7
#define RELAY2_PIN 8

// Handshake Identification Bytes
#define SENSOR_HEADER 0xAA
#define COMMAND_HEADER 0xBB

// Force byte alignment so memory is mapped identically across platforms
struct __attribute__((packed)) SensorPacket {
  byte header;       // Fixed identifier: 0xAA
  byte seqNum;       // Increments on each I2C read request
  byte temperature;  // Temperature in Celsius
  byte relay1;       // Actual pin state of Relay 1
  byte relay2;       // Actual pin state of Relay 2
  byte checksum;     // XOR check of bytes 0 through 4
};

struct __attribute__((packed)) CommandPacket {
  byte header;       // Fixed identifier: 0xBB
  byte seqNum;       // Incremented by the Master (ESP32)
  byte relay1Cmd;    // Intended state for Relay 1
  byte relay2Cmd;    // Intended state for Relay 2
  byte checksum;     // XOR check of bytes 0 through 3
};

// Global Tracking States
volatile byte relay1State = LOW;
volatile byte relay2State = LOW;
float currentTemperature = 0.0;

// Communication & Performance Metrics
uint32_t totalPacketsReceived = 0;
uint32_t badChecksumsDetected = 0;
uint32_t badHeadersDetected = 0;
uint32_t droppedPackets = 0;
byte lastRxSeqNum = 255; 
byte txSeqNum = 0;

// Double-Buffered I2C Variables
volatile CommandPacket rawIncomingBuffer;
volatile bool newCommandPending = false;
SensorPacket outboundPacket;

// Calculate a fast, lightweight XOR Checksum
byte calculateChecksum(const byte* data, size_t length) {
  byte check = 0;
  for (size_t i = 0; i < length; i++) {
    check ^= data[i];
  }
  return check;
}

float readTemperature() {
  int adc = analogRead(LM35_PIN);
  float voltage = adc * (5.0 / 1023.0);
  return voltage * 100.0; 
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.println("\n[SYSTEM INITIALIZED] Arduino Uno Target (10/10 Standard)");
  Serial.println("Monitoring LM35 and I2C command packet stream...\n");
}

void loop() {
  // 1. Gather Analog telemetry data
  currentTemperature = readTemperature();

  // 2. Safely package outgoing telemetry data
  noInterrupts();
  outboundPacket.header = SENSOR_HEADER;
  outboundPacket.seqNum = txSeqNum;
  outboundPacket.temperature = (byte)currentTemperature;
  outboundPacket.relay1 = relay1State;
  outboundPacket.relay2 = relay2State;
  outboundPacket.checksum = calculateChecksum((byte*)&outboundPacket, sizeof(SensorPacket) - 1);
  interrupts();

  // 3. Process new I2C commands safely in the main program execution
  if (newCommandPending) {
    CommandPacket localCommand;
    
    // Create an isolated copy of the incoming buffer
    noInterrupts();
    memcpy(&localCommand, (const void*)&rawIncomingBuffer, sizeof(CommandPacket));
    newCommandPending = false;
    interrupts();

    totalPacketsReceived++;

    // Validation Check 1: Header integrity
    if (localCommand.header != COMMAND_HEADER) {
      badHeadersDetected++;
      printError("HEADER MISMATCH", localCommand.header, COMMAND_HEADER);
    } 
    // Validation Check 2: Checksum integrity
    else if (calculateChecksum((byte*)&localCommand, sizeof(CommandPacket) - 1) != localCommand.checksum) {
      badChecksumsDetected++;
      printError("CHECKSUM CORRUPTED", localCommand.checksum, calculateChecksum((byte*)&localCommand, sizeof(CommandPacket) - 1));
    } 
    // Data is verified as safe!
    else {
      // Sequence & Dropped Packet Tracking
      byte expectedSeq = (byte)(lastRxSeqNum + 1);
      if (lastRxSeqNum != 255 && localCommand.seqNum != expectedSeq) {
        int lost = (localCommand.seqNum > expectedSeq) ? (localCommand.seqNum - expectedSeq) : (256 - expectedSeq + localCommand.seqNum);
        droppedPackets += lost;
        Serial.print("[WARNING] Drop detected! Missed frame count: ");
        Serial.println(lost);
      }
      lastRxSeqNum = localCommand.seqNum;

      // Update local state variables
      relay1State = localCommand.relay1Cmd;
      relay2State = localCommand.relay2Cmd;
      
      // Control actuators
      digitalWrite(RELAY1_PIN, relay1State);
      digitalWrite(RELAY2_PIN, relay2State);

      printSystemDashboard();
    }
  }
  
  delay(200); 
}

// Low-latency Master Data Response interrupt handler
void requestEvent() {
  Wire.write((byte*)&outboundPacket, sizeof(SensorPacket));
  txSeqNum++; // Increment the sequence number for the next check
}

// Low-latency Data Receiver interrupt handler
void receiveEvent(int bytes) {
  if (bytes == sizeof(CommandPacket)) {
    // Read directly into memory buffer without executing block-heavy commands
    Wire.readBytes((byte*)&rawIncomingBuffer, sizeof(CommandPacket));
    newCommandPending = true;
  } else {
    // Flush the I2C bus if a misaligned packet structure arrives
    while (Wire.available()) {
      Wire.read();
    }
  }
}

void printError(const char* msg, byte received, byte expected) {
  Serial.print("\n>>> [X] ");
  Serial.print(msg);
  Serial.print(" | Rec: 0x");
  Serial.print(received, HEX);
  Serial.print(" | Exp: 0x");
  Serial.println(expected, HEX);
}

void printSystemDashboard() {
  Serial.print("Local Temp: "); Serial.print(currentTemperature, 1); Serial.print(" C | ");
  Serial.print("R1: "); Serial.print(relay1State == HIGH ? "ON " : "OFF"); Serial.print(" | ");
  Serial.print("R2: "); Serial.print(relay2State == HIGH ? "ON " : "OFF"); Serial.print(" | ");
  Serial.print("Total Rx: "); Serial.print(totalPacketsReceived); Serial.print(" | ");
  Serial.print("Bad Packets [H/C/D]: "); 
  Serial.print(badHeadersDetected); Serial.print("/"); 
  Serial.print(badChecksumsDetected); Serial.print("/"); 
  Serial.println(droppedPackets);
}