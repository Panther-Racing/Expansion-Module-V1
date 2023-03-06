/* CAN Expanson Module Code
 *  Version 0.00
 * Made by Patrick Murphy
 *
 * Repository https://github.com/Panther-Racing/Expansion-Module-V1
 *
 * Goals: 
 *  Use MCP25625(SPI to CAN) to take analog sensor readings and put data onto the CAN bus for logging purposes.
 * 
 * To Do: 
 *  Parametric BIT shifting function for formatting data lengths for adaptability to other applications
 *  AUX input specification
 *  Compound input specification
 * 
 * Information Specifics:
 * CAN info 64bit data max, 11bit ID 
 * Expected CAN data output Table:
 *  Data Name   |  Length  |  Position
 *  ------------|----------|------------
 *   brake temp |  10 bit  |  0-9
 *   dpos       |  10 bit  |  10-19
 *   slip angle |  10 bit  |  20-29
 *   aux        |  10 bit  |  30-39   
 *   firm vers  |  8  bit  |  40-47
 *   compound   |  8  bit  |  48-55S
 *   counter    |  8  bit  |  56-63
 *  ------------|----------|------------
 * 
 *  Analog Sensor Table:
 *  Sensor Name |  PIN      
 *  ------------|----------
 *   brake temp | ADC0
 *   dpos       | ADC1
 *   slip angle | ADC2
 *   aux        | 
 *   Compound   | 
 *  ------------|----------
 * 
 *
 * Program Notes:
 *  CS pin is technically variable to setup
 *  CAN bus rate is variable to Setup
 *  MCP Clock is variable to setup
 *  
 *  Sensor Wiring is variable to setup
 *
 */


#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN0(10);     // Set CS to pin 10

//Initial Variable Declarations
  //10 bit values
  unsigned int BrkTmp=0b0000000000;
  unsigned int DPos=0b0000000000;
  unsigned int SlpAgl=0b0000000000;
  unsigned int Aux=0b0000000000;

  //8 bit values
  uint8_t compound=0x00; 
  uint8_t firmwareversion= 0x00; //Firmware version
  uint8_t counter=0x00; // coms loss check


  uint8_t CAN_ID  = 0xE0;// Device CAN ID
  
void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

byte data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void loop()
{ 
  //Data Value Aquisition
  BrkTmp=analogRead(A0);
  DPos=analogRead(A1);
  SlpAgl=analogRead(A2);
  
  Aux=0b1100000011;
  compound=0xFF; 

  //Debuging lines
  //Serial.println(BrkTmp,BIN);
  //Serial.println(DPos,BIN);
  //Serial.println(SlpAgl,BIN);
      
  //Formatting data so 10 bit and 8 bits are stored in segments of 8 bits for SPI transfer
  data[0] = BrkTmp & 0xFF;//byte 1
  data[1] = (BrkTmp>>8 & 0xFF) or (DPos<<2 & 0xFF);//byte 2
  data[2] = (DPos>>6 & 0xFF) or (SlpAgl<<4 & 0xFF);//byte 3
  data[3] = (SlpAgl>>4 & 0xFF) or (Aux<<6 & 0xFF);//byte 4
  data[4] = (Aux>>2 & 0xFF);//byte 5
  data[5] = firmwareversion;
  data[6] = compound;
  data[7] = counter; 
  
  counter=counter+1;// increment counter everytime we send message so we can observe data loss occurence, rolls over at 2^8-1 to 0

  //Sending data to MCP25625 chip
  // Standard can frame with 8bytes of data
  byte sndStat = CAN0.sendMsgBuf(CAN_ID, 0, 8, data);
  if(sndStat == CAN_OK){
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  delay(100);   // send data per 100ms
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
