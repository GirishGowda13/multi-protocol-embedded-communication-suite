#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int packet = 1;
int total_bytes = 0;
int longest_packet = 0;
int shortest_packet = 1000;

String message;
int length = 0;

float average_length = 0.0;

// Function Prototypes
void printHeader();
void printPacketInformation();
void updateStatistics();
void printStatistics();
void printBinary(char c);
void printCharacterTable();
void updateOLED();

void setup() {

  Serial.begin(115200);

  if (!display.begin(0x3C, true)) {
    Serial.println("OLED not found");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.println("UART Analyzer");
  display.display();
}

void loop() {

  if (Serial.available() > 0) {

    message = Serial.readString();
    message.trim();

    length = message.length();

    printHeader();

    printPacketInformation();

    updateStatistics();

    printStatistics();

    printCharacterTable();

    updateOLED();

    Serial.println();

    packet++;
  }
}

//--------------------------------------------------------

void printHeader() {

  Serial.println("==============================================");
  Serial.println("           UART PACKET ANALYZER");
  Serial.println("==============================================");
}

//--------------------------------------------------------

void printPacketInformation() {

  Serial.print("Packet No : ");
  Serial.println(packet);

  Serial.print("Time      : ");
  Serial.print(millis());
  Serial.println(" ms");

  Serial.print("Message   : ");
  Serial.println(message);

  Serial.print("Length    : ");
  Serial.println(length);

  Serial.println();
}

//--------------------------------------------------------

void updateStatistics() {

  if (length > longest_packet)
    longest_packet = length;

  if (length < shortest_packet)
    shortest_packet = length;

  total_bytes += length;

  average_length = (float)total_bytes / packet;
}

//--------------------------------------------------------

void printStatistics() {

  Serial.println("============= STATISTICS =============");

  Serial.print("Total Bytes     : ");
  Serial.println(total_bytes);

  Serial.print("Average Length  : ");
  Serial.printf("%.2f\n", average_length);

  Serial.print("Longest Packet  : ");
  Serial.println(longest_packet);

  Serial.print("Shortest Packet : ");
  Serial.println(shortest_packet);

  Serial.println();
}

//--------------------------------------------------------

void printBinary(char c) {

  for (int bit = 7; bit >= 0; bit--) {

    if (c & (1 << bit))
      Serial.print("1");
    else
      Serial.print("0");
  }
}

//--------------------------------------------------------

void printCharacterTable() {

  Serial.println("Character   ASCII      HEX       BINARY");
  Serial.println("-----------------------------------------------");

  for (int i = 0; i < length; i++) {

    Serial.printf("%-11c %-10d %-9X",
                  message[i],
                  (int)message[i],
                  (int)message[i]);

    printBinary(message[i]);

    Serial.println();
  }
}

//--------------------------------------------------------

void updateOLED() {

  display.clearDisplay();

  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);

  display.drawRect(0, 0, 128, 64, SH110X_WHITE);

  display.setCursor(18, 5);
  display.println("UART ANALYZER");

  display.drawLine(0, 18, 127, 18, SH110X_WHITE);

  display.setCursor(5, 24);
  display.print("Packet : ");
  display.println(packet);

  display.setCursor(5, 36);
  display.print("Length : ");
  display.println(length);

  display.setCursor(5, 48);
  display.print("Msg: ");

  // Prevent OLED overflow
  if (message.length() > 12)
    display.println(message.substring(0, 12));
  else
    display.println(message);

  display.display();
}