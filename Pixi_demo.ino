// Demo Sketch for Pixi Analog Shield
// written by
// Wolfgang Friedrich
// 19. Nov 2014


// inslude the SPI library:
#include <SPI.h>
#include "Pixi.h"

//Initialize the Pixi Shield
Pixi pixi;


void setup() {
  word Pixi_ID = 0;
  float Temp = 0;
  word test = 0;

  Serial.begin(115200);

  Serial.println("");
  Serial.println("PIXI Analog Shield Test");


  Pixi_ID = pixi.config();
  
        // Identify shield by ID register       
	if (Pixi_ID == 0x0424 ) {
		Serial.print("Found PIXI module ID: 0x");
		Serial.println(Pixi_ID,HEX);
  
                //readRawTemperature test
		test = pixi.readRawTemperature ( TEMP_CHANNEL_INT );
		Serial.print("Internal Raw Temperature: 0x");
		Serial.println(test, HEX);

                //read all 3 temperature sensors, cable need to be connected to ext sensor 1
		Temp = pixi.readTemperature ( TEMP_CHANNEL_INT );
		Serial.print("Internal Temperature:  ");
		Serial.println(Temp);
		Temp = pixi.readTemperature ( TEMP_CHANNEL_EXT0 );
		Serial.print("External0 Temperature: ");
		Serial.println(Temp);
		Temp = pixi.readTemperature ( TEMP_CHANNEL_EXT1 );
		Serial.print("External1 Temperature: ");
		Serial.println(Temp);

                // configure channels for functialnal test
                // Channel 0 DAC
                pixi.configChannel ( CHANNEL_0, CH_MODE_DAC, 0, CH_0_TO_10P, 0 );
                pixi.configChannel ( CHANNEL_2, CH_MODE_DAC, 0, CH_0_TO_10P, 0 );

                // register content read back
//		Serial.print("PIXI_DEVICE_CTRL: ");
//		test = pixi.ReadRegister ( PIXI_DEVICE_CTRL, true );
//		Serial.print("PIXI_DAC_DATA+CHANNEL_0: ");
//		test = pixi.ReadRegister ( PIXI_DAC_DATA+CHANNEL_0, true );
//		Serial.print("PIXI_PORT_CONFIG+CHANNEL_0: ");
//		test = pixi.ReadRegister ( PIXI_PORT_CONFIG+CHANNEL_0, true );

                // Channel 1 ADC
                pixi.configChannel ( CHANNEL_1, CH_MODE_ADC_P, 0, CH_0_TO_10P, ADC_MODE_CONT );

                // register content read back
//		Serial.print("PIXI_DEVICE_CTRL: ");
//		test = pixi.ReadRegister ( PIXI_DEVICE_CTRL, true );
//		Serial.print("PIXI_PORT_CONFIG+CHANNEL_1: ");
//		test = pixi.ReadRegister ( PIXI_PORT_CONFIG+CHANNEL_1, true );


	}
	else
	{
		Serial.println("No PIXI module found!");
		Serial.println();

  
  };
}

void loop() {

  word adc_value = 0;
  int incomingByte = 0;   // for incoming serial data
   word test_result = 0;
   
    // DAC and ADC test
    // channel 0 and 1 pins at connector P1 should be connected to see values other than 0 on the ADC  
      pixi.writeAnalog ( CHANNEL_0, 0 );
      pixi.writeAnalog ( CHANNEL_2, 0 );
    Serial.println("DAC out value 0");
		Serial.print("PIXI_PORT_1 ADC value: 0x");
    delay(1);
		adc_value = pixi.ReadRegister ( PIXI_ADC_DATA+CHANNEL_1, false );
		Serial.println(adc_value, HEX);
//    delay(5000);
    while (Serial.available() == 0) {};
incomingByte = Serial.read();
      pixi.writeAnalog ( CHANNEL_0, 1024 );
    Serial.println("DAC out value 1024");
		Serial.print("PIXI_PORT_1 ADC value: 0x");
    delay(1);
		adc_value = pixi.ReadRegister ( PIXI_ADC_DATA+CHANNEL_1, false );
		Serial.println(adc_value, HEX);
//    delay(5000);
    while (Serial.available() == 0) {};
incomingByte = Serial.read();
    pixi.writeAnalog ( CHANNEL_0, 2048 );
    Serial.println("DAC out value 2048");
		Serial.print("PIXI_PORT_1 ADC value: 0x");
//    delay(1);
		adc_value = pixi.ReadRegister ( PIXI_ADC_DATA+CHANNEL_1, false );
		Serial.println(adc_value, HEX);
//    delay(5000);
    while (Serial.available() == 0) {};
incomingByte = Serial.read();
      pixi.writeAnalog ( CHANNEL_0, 3072 );
    Serial.println("DAC out value 3072");
		Serial.print("PIXI_PORT_1 ADC value: 0x");
    delay(1);
		adc_value = pixi.ReadRegister ( PIXI_ADC_DATA+CHANNEL_1, false );
		Serial.println(adc_value, HEX);
//    delay(5000);
    while (Serial.available() == 0) {};
incomingByte = Serial.read();
      pixi.writeAnalog ( CHANNEL_0, 4095 );
    Serial.println("DAC out value 4095");
		Serial.print("PIXI_PORT_1 ADC value: 0x");
    delay(1);
		adc_value = pixi.ReadRegister ( PIXI_ADC_DATA+CHANNEL_1, false );
		Serial.println(adc_value, HEX);
//    delay(5000);
    while (Serial.available() == 0) {};
incomingByte = Serial.read();
  
  
  // ADC loop to output a positive ramp on channel 0
  // Print a dot every 100 levels.
  for (word value = 0; value < 4096; value++) { 
      pixi.writeAnalog ( CHANNEL_0, value );
      if ((value % 100) == 0)
      {
        Serial.print(".");
      }
      
      delay(1);
    }
    // wait a second at the top:
    delay(100);
        Serial.println("!");

  for (word value = 0; value < 100; value++) { 

test_result = pixi.ReadRegister( PIXI_DEVICE_ID, true );

    Serial.print("-");
     while (Serial.available() == 0) {};
incomingByte = Serial.read();
     
      delay(1);
    }





}


