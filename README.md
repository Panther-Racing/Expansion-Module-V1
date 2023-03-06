# Expansion-Module-V1
## Messaging Structure

| Message Structure||
|:---          |:---                          |
|Base CAN ID:  |                              |
|Endianness    | Big Endian                   |
|Message Length| 8 Bytes                      |
|Byte          | Description of Contents      |
|0             | Brake Temp [0-7]             |
|1             | Brake Temp [8-9] / DPos[0-5] | 
|2             | DPos [6-9] / SlipAngle [0-3] |
|3             | SlipAngle [4-9] / Aux [0-1]  |
|4             | Aux [2-9]                    |
|5             | Firmware Version             |
|6             | Compound ID                  |
|7             | Counter                      |



## Data Composition
|Data Name  |  Length(bits)  |  Position
|:---:      |:---:     |:---:
|Brake Temp. |  10   |  0-9
|DPos       |  10   |  10-19
|Slip Angle |  10   |  20-29
|Aux        |  10   |  30-39   
|Firm. Vers  |  8    |  40-47
|Compound   |  8    |  48-55
|Counter    |  8    |  56-63


## Analog Sensor Wiring Table:
|Sensor Name |  PIN      
|:---:       |:---:
| Brake Temp. | ADC0
| DPos       | ADC1
| Slip Angle | ADC2
| Aux        | 
| Compound   | 
