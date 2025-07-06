# Teensy 4.1 sACN to 8-Channel DMX Interface

This firmware transforms a Teensy 4.1 into a professional-grade sACN (E1.31) receiver with 8 independent DMX512 outputs. Each universe is transmitted from its own UART, with base universe determined by an 8-position DIP switch.

- ✅ Compatible with any E1.31-capable lighting software (e.g. QLC+, Madrix, LightKey)
- ✅ 8 buffered DMX outputs over RS485
- ✅ Real-time base universe switching via DIP
- ✅ LED status indicators for each universe + global sACN activity
- ✅ Continuous DMX output at ~44Hz

---

## 🔧 Hardware Required

| Component                       | Quantity | Notes                               |
| ------------------------------- | -------- | ----------------------------------- |
| Teensy 4.1                      | 1        | 3.3 V MCU, 8 UARTs, native Ethernet |
| PJRC Ethernet Kit (PHY adapter) | 1        | Provides Ethernet via RMII          |
| LAN8720 or DP83825I PHY Module  | 1        | Needed for Ethernet jack            |
| 3.3 V RS485 Driver Modules      | 8        | e.g., MAX3485, SN65HVD75            |
| 3-pin XLR connectors (male)     | 8        | Standard DMX out                    |
| 8-position DIP switch           | 1        | Sets base universe 0–255            |
| LEDs (5 mm or SMD)              | 9        | 8 for universes, 1 for sACN receive |
| 470Ω resistors                  | 9        | For LED current limiting            |
| Breadboard or custom PCB        | 1        | Recommended for permanent install   |
| 3.3V logic-level power supply   | 1        | If not powered over USB             |

---

## 🔌 Pin Assignments

### 🧠 UARTs (DMX Outputs)

| Universe | UART    | TX Pin | RS485 Driver DI  |
| -------- | ------- | ------ | ---------------- |
| 1        | Serial1 | 1      | TX1              |
| 2        | Serial2 | 10     | TX2              |
| 3        | Serial3 | 8      | TX3              |
| 4        | Serial4 | 17     | TX4              |
| 5        | Serial5 | 21     | TX5              |
| 6        | Serial6 | 26     | TX6              |
| 7        | Serial7 | 32     | TX7 _(SMD only)_ |
| 8        | Serial8 | 34     | TX8 _(SMD only)_ |

> RS485 A/B lines connect to XLR Pin 3 (Data–) and Pin 2 (Data+). GND connects to XLR Pin 1.

---

### 🎛 DIP Switch (Universe Base Selector)

| Bit | GPIO Pin |
| --- | -------- |
| 0   | 2        |
| 1   | 3        |
| 2   | 4        |
| 3   | 5        |
| 4   | 6        |
| 5   | 7        |
| 6   | 14       |
| 7   | 15       |

> Read at startup and continuously every second. Universe `n` = `DIP + n`

---

### 💡 LED Indicators

| Function       | Pin | Description          |
| -------------- | --- | -------------------- |
| sACN Activity  | 13  | Blinks on any packet |
| Universe 1 LED | 22  | Blinks on output     |
| Universe 2 LED | 23  | \"                   |
| Universe 3 LED | 24  | \"                   |
| Universe 4 LED | 25  | \"                   |
| Universe 5 LED | 28  | \"                   |
| Universe 6 LED | 29  | \"                   |
| Universe 7 LED | 30  | \"                   |
| Universe 8 LED | 31  | \"                   |

> Each LED is wired through a 470Ω resistor to GND.

---

## 🛠️ Wiring Guide

### RS485 Driver (per universe)

| RS485 Module Pin | Connects To            |
| ---------------- | ---------------------- |
| DI (TX input)    | Teensy TXn (see above) |
| DE/RE            | Tie HIGH (3.3V)        |
| VCC              | 3.3V                   |
| GND              | GND                    |
| A/B              | XLR Pins 3/2           |

> Add 120Ω termination resistor on A/B if this is the end of the DMX chain.

---

📋 Complete Teensy 4.1 Pin Assignment Table

| Pin # | Label       | Function                      | Used For                    |
| ----- | ----------- | ----------------------------- | --------------------------- |
| 0     | RX1         | UART1 RX                      | Unused                      |
| 1     | TX1         | UART1 TX                      | DMX Universe 1 TX           |
| 2     | D2          | GPIO                          | DIP Switch Bit 0            |
| 3     | D3          | GPIO                          | DIP Switch Bit 1            |
| 4     | D4          | GPIO                          | DIP Switch Bit 2            |
| 5     | D5          | GPIO                          | DIP Switch Bit 3            |
| 6     | D6          | GPIO                          | DIP Switch Bit 4            |
| 7     | D7          | GPIO                          | DIP Switch Bit 5            |
| 8     | TX3         | UART3 TX                      | DMX Universe 3 TX           |
| 9     | RX3         | UART3 RX                      | Unused                      |
| 10    | TX2         | UART2 TX                      | DMX Universe 2 TX           |
| 11    | MOSI        | SPI                           | Unused                      |
| 12    | MISO        | SPI                           | Unused                      |
| 13    | LED_BUILTIN | GPIO                          | sACN Activity Indicator LED |
| 14    | D14         | GPIO                          | DIP Switch Bit 6            |
| 15    | D15         | GPIO                          | DIP Switch Bit 7            |
| 16    | RX4         | UART4 RX                      | Unused                      |
| 17    | TX4         | UART4 TX                      | DMX Universe 4 TX           |
| 18    | SDA1        | I2C                           | Unused                      |
| 19    | SCL1        | I2C                           | Unused                      |
| 20    | A6 / RX5    | UART5 RX                      | Unused                      |
| 21    | A7 / TX5    | UART5 TX                      | DMX Universe 5 TX           |
| 22    | D22         | GPIO                          | Universe 1 LED              |
| 23    | D23         | GPIO                          | Universe 2 LED              |
| 24    | A10         | GPIO                          | Universe 3 LED              |
| 25    | A11         | GPIO                          | Universe 4 LED              |
| 26    | A12 / TX6   | UART6 TX                      | DMX Universe 6 TX           |
| 27    | A13 / RX6   | UART6 RX                      | Unused                      |
| 28    | A14         | GPIO                          | Universe 5 LED              |
| 29    | A15         | GPIO                          | Universe 6 LED              |
| 30    | A16         | GPIO                          | Universe 7 LED              |
| 31    | A17         | GPIO                          | Universe 8 LED              |
| 32    | TX7         | UART7 TX (SMD Pad)            | DMX Universe 7 TX           |
| 33    | RX7         | UART7 RX (SMD Pad)            | Unused                      |
| 34    | TX8         | UART8 TX (SMD Pad)            | DMX Universe 8 TX           |
| 35    | RX8         | UART8 RX (SMD Pad)            | Unused                      |
| 36–39 | Analog      | A18–A21                       | Unused                      |
| 40–41 | CAN1        | CAN TX/RX                     | Unused                      |
| 42–47 | SD Card     | Built-in SD                   | Unused                      |
| 48–49 | USB Host    | USB Host D+/D–                | Unused                      |
| 50+   | RMII        | Ethernet MAC (via PHY Module) | Used by PJRC Ethernet Kit   |

## 🚦 LED Wiring

Each LED should be connected:

- Anode → Teensy pin via **470Ω resistor**
- Cathode → GND

---

## 🧑‍💻 User Instructions

### 1. **Flashing the Firmware**

- Open `sACN_DMX.ino` in Arduino IDE
- Install **Teensyduino** and **QNEthernet + E1.31 libraries**
- Select **Board: Teensy 4.1**
- Upload the sketch

### 2. **DIP Switch Setup**

- Set base universe (e.g., DIP = 10 = Universes 10–17)
- You can change it anytime — the firmware rechecks every second

### 3. **Send sACN**

- Use any software that supports **sACN (E1.31) multicast or unicast**
- Send data to IP address of Teensy (shown via Serial Monitor or DHCP list)

### 4. **Watch LEDs**

- sACN LED blinks on any received packet
- Per-universe LED blinks each time DMX is output
- Your fixtures should respond normally to each universe

---

## 🧪 Troubleshooting

| Symptom                   | Check                              |
| ------------------------- | ---------------------------------- |
| No DMX output             | Wiring to RS485 chips              |
| sACN LED doesn't blink    | IP config, sACN sender             |
| Universe LEDs don’t blink | DIP setting matches sender         |
| Some LEDs never blink     | You're not sending those universes |

---

## 📦 Notes

- Code written for **unicast**, but can be easily switched to multicast
- Up to 512 channels per universe supported
- DMX output runs at ~44 Hz continuously, independent of sACN rate
