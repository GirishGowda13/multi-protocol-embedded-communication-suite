int packet = 1;
String message;

void printHeader();
void transmitPacket();

void setup() {
  Serial.begin(115200);

  Serial.println("UART TRANSMITTER");
  Serial.println("Type a message and press Enter.");
}

void loop() {

  if (Serial.available() > 0) {

    message = Serial.readString();
    message.trim();

    transmitPacket();

    packet++;
  }
}

void printHeader() {

  Serial.println("==============================");
  Serial.println("      UART TRANSMITTER");
  Serial.println("==============================");
}

void transmitPacket() {

  printHeader();

  Serial.print("Packet No : ");
  Serial.println(packet);

  Serial.print("Time      : ");
  Serial.print(millis());
  Serial.println(" ms");

  Serial.print("Length    : ");
  Serial.println(message.length());

  Serial.print("Message   : ");
  Serial.println(message);

  Serial.println();
}