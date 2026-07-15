#include <HardwareSerial.h>

HardwareSerial RS485(2);

#define RXD2 16
#define TXD2 17
#define DE_RE 4

String packet = "";

void setup() {

  Serial.begin(115200);

  RS485.begin(9600, SERIAL_8N1, RXD2, TXD2);

  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW);

  Serial.println("=================================");
  Serial.println("ESP32 RS485 MASTER");
  Serial.println("=================================");
}

void loop() {

  // ---------- Send Request ----------
  digitalWrite(DE_RE, HIGH);

  RS485.println("GET_DATA");

  RS485.flush();

  digitalWrite(DE_RE, LOW);

  // ---------- Wait for Reply ----------
  packet = "";

  unsigned long start = millis();

  while (millis() - start < 1000) {

    while (RS485.available()) {

      char c = RS485.read();

      if (c == '\n')
        break;

      packet += c;
    }

    if (packet.length() > 0)
      break;
  }

  packet.trim();

  if (packet.length() > 0) {

    Serial.println("----------------------------------");
    Serial.println("Packet Received");
    Serial.println(packet);

    float temp = 0;
    float volt = 0;
    float curr = 0;
    float power = 0;

    sscanf(packet.c_str(),
           "<TEMP:%f,VOLT:%f,CURR:%f,POWER:%f>",
           &temp,
           &volt,
           &curr,
           &power);

    Serial.println();
    Serial.println("Decoded Values");
    Serial.print("Temperature : ");
    Serial.print(temp);
    Serial.println(" C");

    Serial.print("Voltage     : ");
    Serial.print(volt);
    Serial.println(" V");

    Serial.print("Current     : ");
    Serial.print(curr);
    Serial.println(" mA");

    Serial.print("Power       : ");
    Serial.print(power);
    Serial.println(" mW");

  } else {

    Serial.println("No response from slave.");
  }

  delay(1000);
}