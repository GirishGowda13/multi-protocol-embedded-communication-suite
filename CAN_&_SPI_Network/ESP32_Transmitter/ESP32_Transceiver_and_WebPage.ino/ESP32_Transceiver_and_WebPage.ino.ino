#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define i2c_Address 0x3C 

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int SPI_CS_PIN = 5;
const int LM35_PIN = 34; 
MCP_CAN CAN(SPI_CS_PIN);

// --- Wi-Fi Network Credentials ---
const char* ssid = "Race_Dash";
const char* password = "password123"; // Must be at least 8 characters
WebServer server(80);

unsigned long lastUpdate = 0;
float engineTemp = 0.0;
int rpm = 1000;
int kph = 0;
char currentGear = 'N';
bool revvingUp = true; 

// --- HTML Dashboard Page (Stored in Flash Memory) ---
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Race Telemetry</title>
    <style>
        body { font-family: 'Arial', sans-serif; background: #0e1013; color: white; text-align: center; margin: 0; padding: 20px; }
        .container { max-width: 500px; margin: auto; background: #1c1f26; padding: 20px; border-radius: 15px; box-shadow: 0 10px 20px rgba(0,0,0,0.5); }
        h1 { color: #00e676; font-size: 1.8rem; margin-bottom: 20px; text-transform: uppercase; letter-spacing: 2px; }
        .data-box { background: #262a34; padding: 15px; margin: 10px 0; border-radius: 10px; border-left: 5px solid #00e676; }
        .label { font-size: 0.9rem; color: #8a92a3; text-transform: uppercase; }
        .value { font-size: 2.2rem; font-weight: bold; font-family: monospace; }
        .unit { font-size: 1rem; color: #00e676; }
    </style>
</head>
<body>
    <div class='container'>
        <h1>Wireless ECU HUD</h1>
        <div class='data-box' style='border-left-color: #ff1744;'>
            <div class='label'>Engine Speed</div>
            <div class='value'><span id='rpm'>0</span> <span class='unit'>RPM</span></div>
        </div>
        <div class='data-box'>
            <div class='label'>Vehicle Velocity</div>
            <div class='value'><span id='kph'>0</span> <span class='unit'>KPH</span></div>
        </div>
        <div class='data-box' style='border-left-color: #29b6f6;'>
            <div class='label'>Active Transmission Gear</div>
            <div class='value'><span id='gear'>N</span></div>
        </div>
        <div class='data-box' style='border-left-color: #ff9100;'>
            <div class='label'>Engine Coolant Temp</div>
            <div class='value'><span id='temp'>0.0</span> <span class='unit'>&deg;C</span></div>
        </div>
    </div>
    <script>
        // High-speed async pull request every 200ms to fetch dynamic json telemetry
        setInterval(function() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('rpm').innerText = data.rpm;
                    document.getElementById('kph').innerText = data.kph;
                    document.getElementById('gear').innerText = data.gear;
                    document.getElementById('temp').innerText = data.temp;
                });
        }, 200);
    </script>
</body>
</html>
)rawliteral";

// HTTP Handler: Serves the beautiful static visual interface frame
void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

// HTTP Handler: Serves raw dynamic diagnostic parameters in standard JSON string format
void handleData() {
  String json = "{";
  json += "\"rpm\":" + String(rpm) + ",";
  json += "\"kph\":" + String(kph) + ",";
  json += "\"gear\":\"" + String(currentGear) + "\",";
  json += "\"temp\":" + String(engineTemp, 1);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22); 
  Wire.setClock(400000); 

  if(!display.begin(i2c_Address, true)) { for(;;); }
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  // --- Initialize Wi-Fi Hotspot ---
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Hotspot Active. IP Address: ");
  Serial.println(IP); // This will typically output 192.168.4.1

  // Configure URL endpoints
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  while (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) { delay(500); }
  CAN.setMode(MCP_NORMAL);
}

void loop() {
  // Always let the webserver handle incoming requests from client browsers
  server.handleClient();

  if (millis() - lastUpdate > 100) { 
    lastUpdate = millis();

    // 1. Read Physical Sensor (Temperature)
    int rawADC = analogRead(LM35_PIN);
    float voltage = (rawADC * 3.3) / 4095.0;
    engineTemp = voltage * 100.0;

    // 2. Run Simulation Physics
    if (revvingUp) {
      rpm += 250;
      if (rpm >= 7000) revvingUp = false; 
    } else {
      rpm -= 350;
      if (rpm <= 1200) revvingUp = true;  
    }

    if (rpm < 1500) { currentGear = '1'; kph = rpm / 60; }
    else if (rpm < 3000) { currentGear = '2'; kph = rpm / 45; }
    else if (rpm < 4500) { currentGear = '3'; kph = rpm / 35; }
    else if (rpm < 6000) { currentGear = '4'; kph = rpm / 28; }
    else { currentGear = '5'; kph = rpm / 22; }

    // 3. Broadcast CAN Data Frame
    unsigned char canPayload[8] = {0};
    memcpy(&canPayload[0], &engineTemp, sizeof(engineTemp));
    canPayload[6] = (rpm >> 8) & 0xFF;
    canPayload[7] = rpm & 0xFF;
    CAN.sendMsgBuf(0x400, 0, 8, canPayload);

    // 4. Update local hardware screen
    display.clearDisplay();
    display.drawRect(0, 0, 128, 8, SH110X_WHITE);
    int rpmBarWidth = map(rpm, 1000, 7000, 0, 124);
    display.fillRect(2, 2, constrain(rpmBarWidth, 0, 124), 4, SH110X_WHITE);
    display.setCursor(0, 14); display.setTextSize(2); display.print(kph); display.setTextSize(1); display.print(" KPH");
    display.setCursor(0, 34); display.print("RPM: "); display.print(rpm);
    display.drawRect(68, 14, 28, 28, SH110X_WHITE); display.setCursor(76, 20); display.setTextSize(2); display.print(currentGear);
    display.drawFastHLine(0, 53, 128, SH110X_WHITE);
    display.setCursor(0, 56); display.setTextSize(1); display.print("ECT: "); display.print(engineTemp, 1); display.print("C");
    display.setCursor(75, 56); display.print("IP: .4.1");
    display.display();
  }
}