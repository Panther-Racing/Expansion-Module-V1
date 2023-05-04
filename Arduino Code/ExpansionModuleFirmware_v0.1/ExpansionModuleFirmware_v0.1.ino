/* CAN Expanson Module Code
 *  Version 0.1
 * Made by Patrick Murphy
 *
 * Repository https://github.com/Panther-Racing/Expansion-Module-V1
 *
 * Goals: 
 *  Use MCP25625(SPI to CAN) to take analog sensor readings and put data onto the CAN bus for logging purposes.
 * 
 * To Do: 
 *  Parametric BIT shifting function for formatting data lengths for adaptability to other applications
 *  Compound value
 * 
 *
 * Program Notes:
 *  CS pin is technically variable to setup
 *  CAN bus rate is variable to Setup
 *  MCP Clock is variable to setup
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
  uint8_t firmwareversion= 0x0A; //Firmware version
  // because firmware version is represented as Vxx.xx, the firmware version needs to be 100 times the value
  // due to unsigned int, ie v0.10 ==> firmware version=10 => 0b00001010 ==> 0x0A
  uint8_t counter=0x00; // coms loss check


  uint16_t CAN_ID  = 0x702;// Device CAN ID
  /*
FR-x700
FL-x701
RR-x702
RL-x703
  */


  unsigned long previousMillis = 0;  // will store last time LED was updated

  // constants won't change:
  const long interval = 20;  // interval at which to blink (milliseconds)

  
void setup()
{
  pinMode(A0,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  pinMode(A2,INPUT_PULLUP);
  pinMode(A3,INPUT_PULLUP);
  
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
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
  Aux=analogRead(A3);
  // debugging lines
  /*
  Serial.println("BrkTmp");  
  Serial.println(BrkTmp);

  Serial.println("DPOS");
  Serial.println(DPos);

  Serial.println("SLPAGL");
  Serial.println(SlpAgl);

  Serial.println("AUX");
  Serial.println(Aux);
  delay(500);
  */

      
  //Formatting data so 10 bit and 8 bits are stored in segments of 8 bits for SPI transfer
  data[0] = (BrkTmp>>2);//byte 1
  data[1] = ((BrkTmp<<6 & 0xc0) | (DPos>>4 & 0x3F));//byte 2
  data[2] = ((DPos<<4 & 0xF0) | (SlpAgl>>6 & 0x0F));//byte 3
  data[3] = ((SlpAgl<<2 & 0xFc) | (Aux>>8 & 0x03));//byte 4
  data[4] = Aux;//byte 5
  data[5] = firmwareversion;
  data[6] = compound;
  data[7] = counter; 

  //Sending data to MCP25625 chip
  // Standard can frame with 8bytes of data
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    //Serial.println((DPos>>4 & 0x3F),BIN);
     for (int i=0; i<8; i++)
      {
        //Serial.println(data[i],HEX);
      }
    if (counter<15){
    counter=counter+1;// increment counter everytime we send message so we can observe data loss occurence, rolls over at 2^8-1 to 0
    }else{
    counter=0;
    }
    byte sndStat = CAN0.sendMsgBuf(CAN_ID, 0, 8, data);
    /*
    if(sndStat == CAN_OK)
    {
      Serial.println("Message Sent Successfully!");
    } 
    else
    {
      Serial.println("Error Sending Message...");
    }
    */
  //delay(50);   // send data per 100ms
  }
  
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
