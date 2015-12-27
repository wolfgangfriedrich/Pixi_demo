/*
  Pixi.cpp - Library for Analog shield with Maxim PIXI A/D chip.
  Created by Wolfgang Friedrich, July 29. 2014.
  Will be released into the public domain.
*/

#include "Arduino.h"
#include "SPI.h"
#include "Pixi.h"

// set pin 10 as the slave select for the SPI interface:
const int slaveSelectPin = 10;

// Config SPI for communication witht the PIXI
Pixi::Pixi()
{

// Config SPI interface
pinMode (slaveSelectPin, OUTPUT);
 digitalWrite(slaveSelectPin,HIGH);
SPI.begin();
SPI.setClockDivider(SPI_CLOCK_DIV2);
SPI.setDataMode(SPI_MODE0);
SPI.setBitOrder(MSBFIRST) ; 

}

/*
Read register and return value
if the debug parameter is set to TRUE, the register value is printed in format
SPI read adress: 0x0 : h 0x4 : l 0x24
SPI read result 0x424
*/
word Pixi::ReadRegister(byte address, boolean debug = false)
{
    word result = 0;
    byte read1 = 0;
    byte read2 = 0;
    
if (debug) {
//		Serial.print("SPI read adress: 0x");
//		Serial.print(address,HEX);
//		Serial.print(" : h 0x");
};          

  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address and return value via SPI:
  SPI.transfer( (address << 0x01) | PIXI_READ );
  read1 = SPI.transfer(0x00);
if (debug) {
//		Serial.print(read1,HEX);
};
  read2 = SPI.transfer(0x00);
if (debug) {
//		Serial.print(" : l 0x");
//		Serial.println(read2,HEX);
};

  result = (read1 << 8) + read2;
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH); 

if (debug) {
//		Serial.print("SPI read result 0x");
//		Serial.println(result,HEX);
}
  return (result);
}


/*
write value into register
Address needs to be shifted up 1 bit, LSB is read/write flag
hi data byte is sent first, lo data byte last.
*/
void Pixi::WriteRegister(byte address, word value)
{
//    word result = 0;
    byte value_hi =0;
    
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address and value via SPI:
  SPI.transfer( (address << 0x01) | PIXI_WRITE );
  SPI.transfer( byte( value >> 8));
  SPI.transfer( byte( value & 0xFF));
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH); 

  return; // (result);
}


/*
General Config for the PixiShield
Read ID register to make sure the shield is connected.
*/
word Pixi::config()
{
  word result = 0;
  word info = 0;
  
  result = ReadRegister( PIXI_DEVICE_ID, true );

if (result == 0x0424) {
// enable default burst, thermal shutdown, leave conversion rate at 200k
	WriteRegister (PIXI_DEVICE_CTRL, !BRST | THSHDN ); // ADCCONV = 00 default.
// enable internal temp sensor
	// disable series resistor cancelation
        info = ReadRegister ( PIXI_DEVICE_CTRL, false );
	WriteRegister (PIXI_DEVICE_CTRL, info | !RS_CANCEL );
	// keep TMPINTMONCFG at default 4 samples
	
	// Set int temp hi threshold
	WriteRegister (PIXI_TEMP_INT_HIGH_THRESHOLD, 0x0230 );		// 70 deg C in .125 steps
	// Keep int temp lo threshold at 0 deg C, negative values need function to write a two's complement number.
	// enable internal and both external temp sensors
        info = ReadRegister ( PIXI_DEVICE_CTRL, false );
	WriteRegister (PIXI_DEVICE_CTRL, info | TMPCTLINT | TMPCTLEXT1 | TMPCTLEXT2 );
	
}


  return (result);
  
}



/*
Channel Config
Parameters that are not used for the selected channel are ignored.
*/

word Pixi::configChannel( int channel, int channel_mode, word dac_dat, word range, byte adc_ctl )
{
  word result = 0;
  word info = 0;

if ( ( channel <= 19 ) && ( channel_mode <= 12 ) )
{

if (channel_mode == CH_MODE_1  ||
    channel_mode == CH_MODE_3  ||
    channel_mode == CH_MODE_4  ||
    channel_mode == CH_MODE_5  ||
    channel_mode == CH_MODE_6  ||
    channel_mode == CH_MODE_10 ) 
    {
      // config DACREF (internal reference),DACCTL (sequential update)
      info = ReadRegister ( PIXI_DEVICE_CTRL, true );
      WriteRegister (PIXI_DEVICE_CTRL, info | DACREF | !DACCTL );
      delay (1);
      info = ReadRegister ( PIXI_DEVICE_CTRL, true );
      // Enter DACDAT
      WriteRegister ( PIXI_DAC_DATA + channel, dac_dat);
      // Mode1: config FUNCID, FUNCPRM (non-inverted default)
      if (channel_mode == CH_MODE_1)
      {
        WriteRegister ( PIXI_PORT_CONFIG + channel, ( ( (CH_MODE_1 << 12 ) & FUNCID ) |
                                                      ( (range << 8 ) & FUNCPRM_RANGE ) ) );
        
      };
      delay (1);
      // Mode3: config GPO_DAT, leave channel at logic level 0
      if (channel_mode == CH_MODE_3)
      {
        if ( channel <= 15 )
        {
          WriteRegister ( PIXI_GPO_DATA_0_15, 0x00);
        }
        else if (channel >= 16 )
        {
          WriteRegister ( PIXI_GPO_DATA_16_19, ( 0x00 )  );
        };
      }
      // Mode3,4,5,6,10: config FUNCID, FUNCPRM (non-inverted default)
      if (channel_mode == CH_MODE_3  ||
          channel_mode == CH_MODE_5  ||
          channel_mode == CH_MODE_6  ||
          channel_mode == CH_MODE_10)
      {
        WriteRegister ( PIXI_PORT_CONFIG + channel, ( ( (channel_mode << 12 ) & FUNCID ) |
                                                      ( (range << 8 ) & FUNCPRM_RANGE ) ) );
        
      }
      else if (channel_mode == CH_MODE_4  )
      {
        WriteRegister ( PIXI_PORT_CONFIG + channel, ( ( (channel_mode << 12 ) & FUNCID ) |
                                                      ( (range << 8 ) & FUNCPRM_RANGE )
                                                       // assoc port & FUNCPRM_ASSOCIATED_PORT                                                          
                                                      ) );
        
      }
      delay (1);
       // Mode1: config GPIMD (leave at default INT never asserted
      if (channel_mode == CH_MODE_1)
      {
//        WriteRegister ( PIXI_GPI_IRQ_MODE_0_7, 0 );
        
      }
      delay (1);
       
    }

else if (channel_mode == CH_MODE_7  ||
         channel_mode == CH_MODE_8  ||
         channel_mode == CH_MODE_9  ){

      // Mode9: config FUNCID, FUNCPRM
      if (channel_mode == CH_MODE_9)
      {
        WriteRegister ( PIXI_PORT_CONFIG + channel, ( ( (channel_mode << 12 ) & FUNCID ) |
                                                      ( (range << 8 ) & FUNCPRM_RANGE )
                                                      ) );
      }
      delay (1);
      if (channel_mode == CH_MODE_7  ||
          channel_mode == CH_MODE_8)
      {
        WriteRegister ( PIXI_PORT_CONFIG + channel, ( ( (channel_mode << 12 ) & FUNCID ) |
                                                      ( (range << 8 ) & FUNCPRM_RANGE ) 
                                                      ) );
      }
      delay (1);

      // config ADCCTL 
      info = ReadRegister ( PIXI_DEVICE_CTRL, false );
      WriteRegister (PIXI_DEVICE_CTRL, info | ( adc_ctl & ADCCTL ) );
      delay (1);
         
}
else if (channel_mode == CH_MODE_2  ||
         channel_mode == CH_MODE_11 ||
         channel_mode == CH_MODE_12 ){

        WriteRegister ( PIXI_PORT_CONFIG + channel, ( ( (channel_mode << 12 ) & FUNCID ) |
                                                      ( (range << 8 ) & FUNCPRM_RANGE ) 
                                                      ) );

         
         
};

}
  return (result);
};


/*
void Pixi::configTempChannel()
{

}
*/

/*
void Pixi::configInterrupt()
{

}
*/

/*
Readout of raw register value for given temperature channel
*/
word Pixi::readRawTemperature(int temp_channel)
{
  word result = 0;
  
  result = ReadRegister( PIXI_INT_TEMP_DATA + temp_channel, false ); // INT_TEMP_DATA is the lowest temp data adress, channel runs from 0 to 2.

  return (result);

}  

/*
Readout of given temperature channel and conversion into degC float return value
*/
float Pixi::readTemperature(int temp_channel)
{
  float result = 0;
  word rawresult = 0;
  boolean sign = 0;
  
  rawresult =  ReadRegister( PIXI_INT_TEMP_DATA + temp_channel, false ); // INT_TEMP_DATA is the lowest temp data adress, channel runs from 0 to 2.

  sign = ( rawresult & 0x0800 ) >> 11; 
  
  if (sign == 1){
	rawresult = ( ( rawresult & 0x07FF ) xor 0x07FF ) + 1;	// calc absolut value from 2's comnplement
  }
  
  result = 0.125 * ( rawresult & 0x0007 ) ;	// pick only lowest 3 bit for value left of decimal point  
											// One LSB is 0.125 deg C
  result = result + ( ( rawresult >> 3) & 0x01FF ) ;
  
  if (sign == 1){
	result = result * -1;	// fix sign
  }
    
  return (result);

}  

/*
output analog value when channel is configured in mode 5
*/
word Pixi::writeAnalog(int channel, word value)
{
word result = 0;
word channel_func = 0;

  channel_func = ReadRegister (  PIXI_PORT_CONFIG + channel, false ); 
  channel_func = ( channel_func & FUNCID ) >> 12 ;

  if (channel_func == 5)
  {
    WriteRegister ( PIXI_DAC_DATA + channel, value );
    result = ReadRegister ( PIXI_DAC_DATA + channel, false );
  }


  return (result);
};

