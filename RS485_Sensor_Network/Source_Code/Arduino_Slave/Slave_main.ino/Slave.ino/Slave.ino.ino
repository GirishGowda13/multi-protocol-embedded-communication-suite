#include <SoftwareSerial.h>

#define RX_PIN 10      // MAX485 RO
#define TX_PIN 11      // MAX485 DI
#define DE_RE 4        // MAX485 DE & RE

#define LM35_PIN A0

SoftwareSerial RS485(RX_PIN, TX_PIN);

void setup() {

  Serial.begin(9600);
  RS485.begin(9600);

  pinMode(DE_RE, OUTPUT);

  digitalWrite(DE_RE, LOW);   // Receive Mode

  Serial.println("Arduino RS485 Slave Ready");
}

void loop() {

  if (RS485.available()) {

    String command = RS485.readStringUntil('\n');

    command.trim();

    if (command == "GET_DATA") {

      // ---------- LM35 ----------
      int adc = analogRead(LM35_PIN);

      float voltageLM35 = adc * (5.0 / 1023.0);

      float temperature = voltageLM35 * 100.0;

      // ---------- Simulated INA219 ----------
      float voltage = 12.10;
      float current = 245.30;
      float power = 2968.50;

      // ---------- Send Packet ----------
      digitalWrite(DE_RE, HIGH);

      RS485.print("<TEMP:");
      RS485.print(temperature, 1);

      RS485.print(",VOLT:");
      RS485.print(voltage, 2);

      RS485.print(",CURR:");
      RS485.print(current, 2);

      RS485.print(",POWER:");
      RS485.print(power, 2);

      RS485.println(">");

      RS485.flush();

      digitalWrite(DE_RE, LOW);

      Serial.println("Packet Sent");
    }
  }
}