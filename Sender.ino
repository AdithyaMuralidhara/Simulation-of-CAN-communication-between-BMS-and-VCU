#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS_PIN 10
MCP_CAN CAN0(CAN_CS_PIN);

uint16_t voltage = 3700;
uint8_t temperature = 25;
uint8_t soc = 85;
uint8_t faultFlags = 0;

uint32_t lastSend = 0; //To keep track of time

void setup() {
  Serial.begin(115200);

  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ)!= CAN_OK) { //Speed is set to 500Kbps
    Serial.println("CAN init fail, retrying...");
    delay(100);
  }
  CAN0.setMode(MCP_NORMAL); //Set to normal mode, Loopback mode can be used to verify data being sent
  Serial.println("BMS Sender ready (NORMAL mode)");
}

void loop() {
  if (millis()-lastSend>=1000) {
    lastSend = millis(); // To send data every second

    // Simulate changing values
    voltage= random(3600,4200); //Random value for voltage generated from 3600mV to 4200mV
    temperature= random(20,50); //Random value for temperature is generated from 20C to 50C
    soc = random(0,100); //Random value for Stae of charging is generated from 0% to 100%

    // Simulate faults
    faultFlags = 0; // Start with no faults
    if (voltage > 4100) faultFlags |= 0b00001; // High voltage (Bit 0)
    if (voltage < 3500) faultFlags |= 0b00010; // Low voltage  (Bit 1)
    if (temperature > 45) faultFlags |= 0b00100; // High temp   (Bit 2)
    if (temperature < 15) faultFlags |= 0b01000; // Low temp    (Bit 3)
    if (soc < 20) faultFlags |= 0b10000; // SOC Low     (Bit 4)

    uint8_t volt[2] = { (uint8_t)(voltage >> 8), (uint8_t)(voltage & 0xFF) }; //Send two bytes of Voltage seperately, MSB first.
    CAN0.sendMsgBuf(0x100, 0, 2, volt); //CANID for voltage is set to 0x100 

    // Send Temperature 
    CAN0.sendMsgBuf(0x101, 0, 1, &temperature); //CANID for temperature is set to 0x101

    // Send SOC
    CAN0.sendMsgBuf(0x102, 0, 1, &soc); //CANID for SOC is set to 0x102

    // Send Fault Flags
    CAN0.sendMsgBuf(0x103, 0, 1, &faultFlags); //CANID for Fault code is set to 0x103

    Serial.println("Data sent: Voltage, Temp, SOC, Faults");
  }
}
