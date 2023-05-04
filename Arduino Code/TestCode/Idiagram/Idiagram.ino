/* CAN Expanson Module Code
 *  Version 0.2
 * Made by Patrick Murphy
 *
 * Repository https://github.com/Panther-Racing/Expansion-Module-V1
 *
 *
 * FOR: I diagram testing of the CAN bus
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
    //unsigned int BrkTmp=0b0000000000;
    //unsigned int DPos=0b0000000000;
    //unsigned int Aux1=0b0000000000;
    //unsigned int Aux2=0b0000000000;
  
  //8 bit values
    uint8_t TEC=0x00; // transmit error counters
    uint8_t firmwareversion= 0x14; //Firmware version
      // because firmware version is represented as Vxx.xx, the firmware version needs to be 100 times the value
      // due to unsigned int, ie v0.10 ==> firmware version=10 => 0b00001010 ==> 0x0A
  
    uint8_t counter=0x00; // coms loss check


  // board Chip Select Pin for SPI to CAN chip 
    MCP2515 mcp2515(10);
 


void setup() {
  interval = 1000/interval; //conversion from HZ to MS
  //Setting Pullups on ADC pins in use
  //pinMode(A0,INPUT_PULLUP);
  //pinMode(A1,INPUT_PULLUP);
  //pinMode(A2,INPUT_PULLUP);
  //pinMode(A3,INPUT_PULLUP);


  frame.can_id  = 0x760; //CAN ID
  /*
    FR-x760
    FL-x761
    RR-x762
    RL-x763
  */
  frame.can_dlc = 2;//Data lenght of frame
  //debug control    
  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
}

void message1() {
  //ADC Value Aquisition
 // BrkTmp=analogRead(A0);
  //DPos=analogRead(A1);
  //Aux1=analogRead(A2);
  //Aux2=analogRead(A3);
  //byte TEC=ReadReg(0x1C); // Reading the Transmit Error Counter(TEC) from the can transciever


  frame.data[0] = 0b01100110;
  frame.data[1] = 0b00110000;

  mcp2515.sendMessage(&frame);
  Serial.println("test");
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
  digitalWrite(10,LOW); // send SOC
  SPI.transfer(0b00000011); //say read to device
  SPI.transfer(Reg); //saying target register
  byte output=SPI.transfer(0x00); //reading from target register
  digitalWrite(10,HIGH); // send EOC
  return output;
}
