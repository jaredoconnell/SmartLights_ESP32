# LED Strip Controller Bluetooth LE Protocol Specification

### Version 1.1

---
## Data types:


#### Array
A single byte specifying number of items in the array (0-255)
Followed by all of the data values.

A string is a simple char array and follows the same format.

#### IDs
A single byte if the practical limit is 255. Else two bytes.

#### Colors
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | One byte | Red |
| 1 | One byte | Green |
| 2 | One byte | Blue |

#### Color Sequences
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | 37 Bytes | The color sequence UUID as String |
| 2 | One byte | The number of items in the sequence. Max 255 |
| 3 | One byte | The sequence type (in this case, 0) |
| 4 | Two bytes | The sustain time (in 60ths of a second) |
| 6 | Two bytes | The transition time (in 60ths of a second) |
| 8 | One byte | The transition type ID |
| n * 3 + 9 | Three bytes. | The color |
| N * 5 | Up to 30 | Name of the color sequence as a char array up to 29 characters. |
Note: n starts at 0.

The number of bytes in the sequence is 44 + n * 3. If there are about 10 other bytes in the packet for other things, the practical limit for a single packet is 54 items in the sequence.

#### LED Strip
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | 37 Bytes | The ID of the color strip |
| 2 | One byte. | The number of colors in the LED strip (typically 1 to 5). <br>Should not exceed 100 (though I cannot see why anyone would do that). |
| 3 | 37 Bytes. | The current color sequence ID, or 0 if none. |
| 5 | One byte. | Whether the LED strip is toggled on (1) of off (0) |
| 6 | Two Bytes. | The current brightness of the LED strip (0-4095) |
| 8 | One Byte. | Temporary color is active (1 if active, else 0) |
| 9 | Two Bytes. | Seconds left for temp color (0 if persistent) |
| 11 | Three Bytes. | The temporary color |
| n * 5 + 14 | One byte. | The driver ID |
| n * 5 + 15 | One byte. | The index of the driver pin |
| n * 5 + 16 | Three bytes. | The color given by the diodes on this pin of the LED Strip |
| N * 5 + 14 | Up to 30 | Name of the LED strip as a char array up to 29 characters. |

For context, in an RGBCCT color strip, which is made up of red, green, blue, soft white, and  white, the data for the single LED strip would be 139 bytes long. That means, with overhead, it is realistic to pack about 3 LED strips into a single packet.

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

Packet name: **Get PWM Drivers**\
Packet ID: 1\
Data: None

Packet name: **Get LED Strips**\
Packet ID: 2\
Data: None

Packet name: **Add PWM Driver**\
Packet ID: 3\
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | One byte | The i2c address of the PWM driver. |

Packet name: **Remove PWM Driver**\
Packet ID: 4\
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | One byte | The i2c address of the PWM driver. |

Packet name: **Add LED Strip**\
Packet ID: 5 \
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Many bytes | The LED Strip object (as defined in the data types section)

Packet name: **Remove LED Strip** \
Packet ID: 6 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Two bytes | LED Strip ID |

Packet name: **Add Color Sequence** \
Packet ID: 8 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | One byte | Overwrite boolean.<br>1 if it should overwrite the old ID at this value<br>0 if it should send an error response. |
| 1 | 7 or more | The color sequence (as defined in the data types section) |

Packet name: **Set LED Strip Color Sequence** \
Packet ID: 9 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | 37 Bytes | The Color Strip ID |
| 2 | 37 Bytes | The Color Sequence ID |

Packet name: **Schedule LED Strip Sequence Change** \
Packet ID: 10 \
Data: TODO

Packet name: **Get Color Sequence ID for LED Strip** \
Packet ID: 11 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Two bytes | The Color Sequence ID |

Packet name: **Get Color Sequences** \
Packet ID: 12 \
Data: None

Packet name: **Get Scheduled Sequences** \
Packet ID: 13 \
Data: None

Packet name: **Get Version Number** \
Packet ID: 14 \
Data: None

Packet name: **Get Protocol ID** \
Packet ID: 15 \
Data: None

Packet name: **Set LED Strip brightness** \
Packet ID: 16 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | 37 Bytes | The LED Strip ID |
| 2 | One Byte | 0 if off, 1 if on |
| 3 | Two bytes | The LED Strip brightness between 0 and 4095 |

Packet name: **Get Settings** \
Packet ID: 17 \
Data: None

Packet name: **Set Setting** \
Packet ID: 18 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Many Bytes | The Setting ID as a String |
| ? | One Byte | 0 if short, 1 if string |
| ? | 2 or more bytes | The data (as string or int) |

Packet name: **Set Color Without Sequence** \
Packet ID: 19 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | 37 Bytes | The LED Strip ID |
| 2 | Three bytes | The Color |
| 5 | Two bytes | The number of seconds |

Sets the color for the LED strip.
The color will last the specified number of seconds, indefinitely if 0, except will change if the color sequence is set for the LED strip.


---
### From ESP32 to phone:


Packet name: **PWM Drivers List Response** \
Packet ID: 255 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | One byte | Number of PWM drivers |
| n | One byte per | The PWM driver address(es) |

Packet name: **LED Strips List Response** \
Packet ID: 254 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Two bytes | The number of LED strips |
| 2 | Two bytes | The offset for this packet |
| 4 | One byte | The number of LED strips being sent in this packet |
| * + 5 | Many bytes | The LED strip data sequentially (as defined in the data types section) |

This is one that can easily exceed the packet size, so it is designed to be sent in multiple packets.

Packet name: **Color Sequence for LED Strip Response** \
Packet ID: 252 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Two bytes | The color sequence for that LED strip |


Packet name: **Color Sequence List Response** \
Packet ID: 251 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Two bytes | The number of Color Sequences |
| 2 | Two bytes | The offset for this packet |
| 4 | One byte | The number of Color Sequences being sent in this packet |
| * + 4 | Many bytes | The Color Sequences data sequentially (as defined in the data types section) |

Packet name: **Scheduled Sequences List Response** \
Packet ID: 250 \
Data: TODO


Packet name: **Contents Version Number Response** \
Packet ID: 249 \
Data: TODO

The version number gets updated every time something changes.
If the version number is greater than it was last time, the phone should query for changes.
If the version number is less than it was last time, it means the old data was lost. The user should be prompted asking if the phone should re-sync the data.

Packet name: **Protocol Version Response** \
Packet ID: 248 \
Data: TODO

Packet name: **Settings Response** \
Packet ID: 247 \
Data:
| Index | Size | Data Details |
| --- | --- | --- |
| 0 | Two bytes | The number of settings |
| 2 | Two bytes | The offset for this packet |
| 4 | One byte | The number of settings being sent in this packet |
| ? + 4 | Many Bytes | The setting name |
| ? | One Byte | 0 if this setting is a short, 1 if string. |
| ? | Two or many | The setting value |
