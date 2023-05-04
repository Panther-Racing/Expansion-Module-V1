// CAN Send Example
//

#include <SPI.h>

const int CSpin=8;

void setup()
{
  pinMode(CSpin,OUTPUT);
  //Serial Decs
  Serial.begin(115200);
  Serial.println("Setup Start");

  SPI.begin();
  // SPI decs
  SPI.beginTransaction(SPISettings(8000000,MSBFIRST,SPI_MODE0));

  
  delay(10);
  // First Send reset Command
  digitalWrite(CSpin,LOW);
  SPI.transfer(0b11000000); //sending reset command
  Serial.println("Reset");
  digitalWrite(CSpin,HIGH);
  // after reset command need to hold for min 128 OSC1 clock cycles
  delay(50);  
}

byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void loop()
{
  delay(300);   // send data per 100ms
  byte target=0x0e;// reading the canstat register
  byte data=ReadReg(target);
  Serial.println("Canstat Value");
  Serial.println(data,BIN);
  if (data==0b10000000)// the value that should corespond to configuration mode
  {

    Serial.println("Changing Mode to Normal");
    NormalMode();

  }
  delay(5000);// just a timer to stop annoying repetition for absurd time
}


//Read register function
byte ReadReg (byte Reg){
  // function that reads a single byte message from a specified register, 
  digitalWrite(CSpin,LOW); // send SOC
  SPI.transfer(0b00000011); //say read to device
  SPI.transfer(Reg); //saying target register
  byte output=SPI.transfer(0x00); //reading from target register
  digitalWrite(CSpin,HIGH); // send EOC
  return output;
}


//Write to register function
void WriteReg(byte Reg,byte data){
  digitalWrite(CSpin,LOW); // send SOC
  SPI.transfer(0b00000010); //say write to device
  SPI.transfer(Reg); //saying target register
  SPI.transfer(data); //writing data to target register
  digitalWrite(CSpin,HIGH); // send EOC
}

//Set operational mode to normal mode, clkout on, osm disabled, clkout/1
void NormalMode()
{
  //need to set CANCTRL register REQOP[2:0] bits
  //Read CANSTAT register OPMOD[2:0] bits to verify
  WriteReg(0x0f,0b00000100);// Writing to the normal mode with prescribed conditions
  byte data=ReadReg(0x0e);// Reading the status of the CANSTAT reg to confirm set
  // if 0x00 then should be good and in normal mode
  Serial.println("Normal Mode state Confirmation");
  Serial.println(data,BIN);
}
// Need a function to allow setup of the CNF1,2,3 registers maybe TXRTSCTRL aswell and acceptance filter registers?
void MCPSetup(int NBR,int Fosc)// gonna default this to a 500kbps 40m bus length like the default config in the datasheet.
//reference the excel sheet for why
{
  // setting CNF1
WriteReg(0x2A,0b11000000);// tq=4, brp=0
  //setting CNF2
WriteReg(0x29,0b11011110); // BTLMODE 1, SAM 1, PHSEG1[2:0]=3==> PS1=4TQ, PRSEG=6==> PSLB=7TQ 
  //setting CNF3
WriteReg(0x28,0b00000011);// SOF=0, WAKFIL=0,UNIMPL=000,PHSEG=3==>PS2=4TQ
}
/*Process Flow
 *  PWR on
 *  Reset
 *  Check mode by reading CANSTAT register
 *  Config mode - change params
 *  Change mode to normal
 *  tx data
 * 
 */

/*SPI coms
 * commands and data sent via SI pin data clocked on rising edge of SCK
 * data driven out by MCP on SO line on falling edge of SCK
 * CS pin must be low while anything performed
 */

/* Details on the MCP25625
 *  INT pin signals interrupt to microcontroller, must be cleared through SPI
 *  Must set controller operational mode using SPI in CANCTRL Register (5)
 *    REQOP[2:0} bits
 *    mode change only when all pending message tx complete, mode must be verified by reading OPMOD[2:0} bits in CANSTAT register
 *  Can transciever has modes (2)
 *    normal selected by low level to STBY pin
 *    standby from high level to STBY pin
 *  Config mode.
   *  must be initialized before activiation, only possible in configuration mode
   *    auto selected after powerup, reset, or by changing REQOPx bits in CANCTRL register
   *    verify using CANSTAT register
   *    when entered all error counters cleared
   *    only mode where modify 
   *      CNF1,CNF2,CNF3, TXRTSCTRL, Acceptance filter registers
 *   Normal mode
   *     standard op mode
   *     only one where send ACK, error frame, TX on CAN bus
   *     BOTH controller and transceiver must be in normal mode
 *  
 *  
 */
// CANSTAT regster First half(0000-0111) second half 1110
 
 /*Reset
 *    single byte instruction, pull CS low, send instruction byte, raise CS
 *    11000000
  */
  
  /* Read
   *  lower CS pin
   *  00000011
   *  8 bit address(A7-A0)
   *  data shifts out on SO
   *  terminate by raising CS can have more data come out of next register by holding low
   */
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
