# LED Strip Controller Bluetooth LE Protocol Specification

### Version 1.0

---
## Data types:

#### All data types:
A single byte specifying length (0-255)\
Followed by up to 255 bytes of payload.

#### Array
A single byte specifying number of items in the array (0-255)
Followed by all of the data values.

---

## Packet Format
The packet ID, followed by the data.

---
## Packets
### Both:

Packet name: Reserved\
Packet ID: 0

---
### From phone to ESP32:

Packet name: Get PWM Drivers\
Packet ID: 1\
Data: None

Packet name: Get LED Strips\
Packet ID: 2\
Data: None

Packet name: Add PWM Driver\
Packet ID: 3\
Data: TODO

Packet name: Remove PWM Driver\
Packet ID: 4\
Data: TODO

Packet name: Add LED Strip\
Packet ID: 5 \
Data: TODO

Packet name: Remove LED Strip \
Packet ID: 6 \
Data: TODO

Packet name: Add Colors Sequence to pallet \
Packet ID: 7 \
Data: TODO

Packet name: Set LED Strip Color Sequence \
Packet ID: 8 \
Data: TODO

Packet name: Schedule LED Strip Sequence Change \
Packet ID: 9 \
Data: TODO

Packet name: Get Color Pallet \
Packet ID: 10 \
Data: TODO

Packet name: Get Color Sequences \
Packet ID: 11 \
Data: TODO

Packet name: Get Scheduled Sequences \
Packet ID: 12 \
Data: TODO

---
### From ESP32 to phone:


Packet name: PWM Drivers List Response \
Packet ID: 255 \
Data: TODO

Packet name: LED Strips List Response \
Packet ID: 254 \
Data: TODO

Packet name: Color Pallet List Response \
Packet ID: 253 \
Data: TODO

Packet name: Color Sequences List Response \
Packet ID: 252 \
Data: TODO

Packet name: Scheduled Sequences List Response \
Packet ID: 251 \
Data: TODO
