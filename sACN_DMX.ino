// Firmware for Teensy 4.1 with PJRC Ethernet Kit
// Receives sACN and sends 8 universes out via DMX UARTs
// Universe base is set via 8 DIP switches and rechecked every second
// Each universe has an LED indicator when it sends DMX
// A global sACN receive LED blinks on any valid sACN packet

#include <QNEthernet.h>
#include <NativeEthernetUdp.h>
#include <E131.h>  // https://github.com/hh100/e131

using namespace qindesign::network;

#define NUM_UNIVERSES 8
#define DMX_CHANNELS 512
#define SACN_PORT 5568
#define DIP_PINS_COUNT 8

// DIP switch pins (8-bit universe base selector)
const uint8_t dipPins[DIP_PINS_COUNT] = {2, 3, 4, 5, 6, 7, 14, 15};
uint8_t lastDipValue = 0;
unsigned long lastDipCheck = 0;
const unsigned long DIP_CHECK_INTERVAL = 1000;

// LED pins
const uint8_t universeLEDs[NUM_UNIVERSES] = {22, 23, 24, 25, 28, 29, 30, 31};
const uint8_t sacnLED = 13;

// UARTs for DMX
HardwareSerial* uarts[NUM_UNIVERSES] = {
  &Serial1, &Serial2, &Serial3, &Serial4,
  &Serial5, &Serial6, &Serial7, &Serial8
};
const uint8_t uartTXPins[NUM_UNIVERSES] = {1, 10, 8, 17, 21, 26, 32, 34};

// Buffers for DMX output
uint8_t dmxBuffers[NUM_UNIVERSES][DMX_CHANNELS];
bool universeUpdated[NUM_UNIVERSES] = {false};

// E1.31 (sACN) Receiver
E131 e131;
uint16_t baseUniverse = 1;

void setup() {
  // DIP switch inputs
  for (int i = 0; i < DIP_PINS_COUNT; i++) {
    pinMode(dipPins[i], INPUT_PULLDOWN);
  }

  // LED outputs
  pinMode(sacnLED, OUTPUT);
  digitalWrite(sacnLED, LOW);
  for (int i = 0; i < NUM_UNIVERSES; i++) {
    pinMode(universeLEDs[i], OUTPUT);
    digitalWrite(universeLEDs[i], LOW);
  }

  // UARTs for DMX output
  for (int i = 0; i < NUM_UNIVERSES; i++) {
    uarts[i]->begin(250000);
  }

  // Ethernet setup
  Ethernet.begin();
  e131.begin(E131_UNICAST);  // or E131_MULTICAST if desired

  // Initialize base universe
  baseUniverse = readDipSwitch();
  lastDipValue = baseUniverse;
  lastDipCheck = millis();
}

void loop() {
  // Check DIP switch every second
  if (millis() - lastDipCheck >= DIP_CHECK_INTERVAL) {
    uint8_t newVal = readDipSwitch();
    if (newVal != lastDipValue) {
      baseUniverse = newVal;
      lastDipValue = newVal;
    }
    lastDipCheck = millis();
  }

  // Check for sACN packet
  if (e131.isPacketAvailable()) {
    e131_packet_t packet;
    e131.pull(&packet);

    digitalWrite(sacnLED, HIGH);
    delayMicroseconds(50);
    digitalWrite(sacnLED, LOW);

    uint16_t universe = htons(packet.universe);
    for (int i = 0; i < NUM_UNIVERSES; i++) {
      if (universe == baseUniverse + i) {
        memcpy(dmxBuffers[i], packet.property_values + 1, DMX_CHANNELS);
        universeUpdated[i] = true;
      }
    }
  }

  // Continuously output DMX
  for (int i = 0; i < NUM_UNIVERSES; i++) {
    sendDMX(uarts[i], uartTXPins[i], dmxBuffers[i]);
    if (universeUpdated[i]) {
      digitalWrite(universeLEDs[i], HIGH);
      delayMicroseconds(50);
      digitalWrite(universeLEDs[i], LOW);
      universeUpdated[i] = false;
    }
  }
}

// Read 8-bit value from DIP switches
uint8_t readDipSwitch() {
  uint8_t val = 0;
  for (int i = 0; i < DIP_PINS_COUNT; i++) {
    val |= (digitalRead(dipPins[i]) << i);
  }
  return val;
}

void sendDMX(HardwareSerial* uart, uint8_t txPin, uint8_t* data) {
  uart->end();
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, LOW);
  delayMicroseconds(100);  // Break
  digitalWrite(txPin, HIGH);
  delayMicroseconds(12);   // MAB
  uart->begin(250000);
  uart->write(0);  // Start code
  uart->write(data, DMX_CHANNELS);
  uart->flush();
}
