/* CAN Expanson Module Code
 *  Version 0.2
 * Made by Patrick Murphy 2023
 * Revised in 2024 by Raheel Farouk(raheelfarouk@pitt.edu/raheelfarouk@gmail.com)
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


#include <SPI.h>
#include <mcp2515.h>
struct can_frame frame;

//Initial Variable Declarations
  // constants won't change:
  // Interval Declaration
    long interval = 100;  // interval at which to send HZ

    unsigned long previousMillis = 0; 

  //10 bit values
    unsigned int BrkTmp=0b0000000000;
    unsigned int DPos=0b0000000000;
    unsigned int Aux1=0b0000000000;
    unsigned int Aux2=0b0000000000;
  
  //8 bit values
    uint8_t TEC=0x00; // transmit error counters
    uint8_t firmwareversion= 0xF0; //Firmware version
      // because firmware version is represented as Vxx.xx, the firmware version needs to be 100 times the value
      // due to unsigned int, ie v0.10 ==> firmware version=10 => 0b00001010 ==> 0x0A
  
    uint8_t counter=0x00; // coms loss check


  // board Chip Select Pin for SPI to CAN chip  
    uint8_t CS=10;
    MCP2515 mcp2515(CS);
 


void setup() {
  interval = 1000/interval; //conversion from HZ to MS
  //Setting Pullups on ADC pins in use
  pinMode(A0,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  pinMode(A2,INPUT_PULLUP);
  pinMode(A3,INPUT_PULLUP);


  frame.can_id  = 0x761; //CAN ID
  /*
    FR-x760
    FL-x761
    RR-x762
    RL-x763
  */
  frame.can_dlc = 8;//Data lenght of frame
  //debug control    
  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_5000KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
}

void message1() {
  //ADC Value Aquisition
  BrkTmp=analogRead(A0);
  DPos=analogRead(A1);
  Aux1=analogRead(A2);
  Aux2=analogRead(A3);
  byte TEC=ReadReg(0x1C); // Reading the Transmit Error Counter(TEC) from the can transciever


  frame.data[0] = (BrkTmp>>2);//byte 1
  frame.data[1] = ((BrkTmp<<6 & 0xc0) | (DPos>>4 & 0x3F));//byte 2
  frame.data[2] = ((DPos<<4 & 0xF0) | (Aux1>>6 & 0x0F));//byte 3
  frame.data[3] = ((Aux1<<2 & 0xFc) | (Aux2>>8 & 0x03));//byte 4
  frame.data[4] = Aux2;//byte 5
  frame.data[5] = firmwareversion;
  frame.data[6] = TEC;
  frame.data[7] = counter; 

  mcp2515.sendMessage(&frame);
}


void loop() 
{

   unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) 
  {
    // save the last time you sent
    previousMillis = currentMillis;

    //counter setup
    if (counter<15){
      counter=counter+1;// increment counter everytime we send message so we can observe data loss occurence, rolls over at 2^8-1 to 0
    }
    else
    {
      counter=0;
    }

    //Message function
    message1();

  }
}

byte ReadReg (byte Reg){
  // function that reads a single byte message from a specified register, 
  digitalWrite(CS,LOW); // send SOC
  SPI.transfer(0b00000011); //say read to device
  SPI.transfer(Reg); //saying target register
  byte output=SPI.transfer(0x00); //reading from target register
  digitalWrite(CS,HIGH); // send EOC
  return output;
}
