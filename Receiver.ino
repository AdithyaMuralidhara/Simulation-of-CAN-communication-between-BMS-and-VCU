#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS_PIN 10
MCP_CAN CAN0(CAN_CS_PIN);

uint16_t voltage;
uint8_t temperature;
uint8_t soc;
uint8_t faultFlags;

void printFaults(uint8_t flags) {
  if (flags == 0) {
    Serial.println("No faults detected.");
    Serial.println();
    return;
  }
  Serial.print("FAULT ALERT: ");
  // Use bitwise AND (&) to check each flag individually
  if (flags & 0b00001) Serial.print("High Voltage ");
  if (flags & 0b00010) Serial.print("Low Voltage ");
  if (flags & 0b00100) Serial.print("High Temp ");
  if (flags & 0b01000) Serial.print("Low Temp ");
  if (flags & 0b10000) Serial.print("Low SOC ");
  Serial.println();
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN init fail, retrying...");
    delay(100);
  }
  CAN0.setMode(MCP_NORMAL);
  Serial.println("VCU Receiver ready (NORMAL mode)");
}

void loop() {
  if (CAN0.checkReceive() == CAN_MSGAVAIL) {
    long unsigned int rxId;
    byte len;
    byte buf[8];
    CAN0.readMsgBuf(&rxId, &len, buf);

    switch (rxId) {
      case 0x100: // Voltage
        voltage = (buf[0] << 8) | buf[1];
        Serial.print("Voltage: "); Serial.print(voltage); Serial.println(" mV");
        break;

      case 0x101: // Temperature
        temperature = buf[0];
        Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
        break;

      case 0x102: // SOC
        soc = buf[0];
        Serial.print("SOC: "); Serial.print(soc); Serial.println(" %");
        break;

      case 0x103: // Fault Flags
        faultFlags = buf[0];
        printFaults(faultFlags);
        break;

      default:
        Serial.print("Unknown ID: 0x"); Serial.println(rxId, HEX);
        break;
    }
  }
}
