#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS_PIN 10
MCP_CAN CAN0(CAN_CS_PIN);

// Example test values
uint16_t voltage = 3700; // mV
uint8_t temperature = 25; // °C
uint8_t soc = 85;        // %
uint8_t faultFlags = 0;  // Bitfield

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);

  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN init fail, retrying...");
    delay(100);
  }
  CAN0.setMode(MCP_NORMAL);
  Serial.println("BMS Sender ready (NORMAL mode)");
}

void loop() {
  if (millis() - lastSend >= 1000) {
    lastSend = millis();

    // Simulate changing values
    voltage = random(3600, 4200);
    temperature = random(20, 50);
    soc = random(50, 100);

    // Simulate faults
    faultFlags = 0; // Start with no faults
    if (voltage > 4100) faultFlags |= 0b00001; // High voltage (Bit 0)
    if (voltage < 3500) faultFlags |= 0b00010; // Low voltage  (Bit 1)
    if (temperature > 45) faultFlags |= 0b00100; // High temp   (Bit 2)
    if (temperature < 15) faultFlags |= 0b01000; // Low temp    (Bit 3)
    if (soc < 20)         faultFlags |= 0b10000; // SOC Low     (Bit 4)
    // --- Send Voltage ---
    byte volt[2] = { (byte)(voltage >> 8), (byte)(voltage & 0xFF) };
    CAN0.sendMsgBuf(0x100, 0, 2, volt);

    // --- Send Temperature ---
    CAN0.sendMsgBuf(0x101, 0, 1, &temperature);

    // --- Send SOC ---
    CAN0.sendMsgBuf(0x102, 0, 1, &soc);

    // --- Send Fault Flags ---
    CAN0.sendMsgBuf(0x103, 0, 1, &faultFlags);

    Serial.println("Data sent: Voltage, Temp, SOC, Faults");
  }
}
