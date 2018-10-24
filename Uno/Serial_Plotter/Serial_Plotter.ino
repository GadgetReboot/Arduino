/*********************************************************************
  This is an example for displaying data on the 
  Arduino Serial Plotter.
  
  An analog signal on A0 and a DS18B20 digital temperature sensor
  on digital input 2 are continuously read and displayed on the
  Serial interface.  Launch the Serial Plotter from the Tools 
  menu in the Arduino IDE to observe the plot.

  Gadget Reboot

*********************************************************************/

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2                   // DS18B20 data wire is connected to input 2

DeviceAddress thermometerAddress;        // custom array type to hold 64 bit device address
OneWire oneWire(ONE_WIRE_BUS);           // create a oneWire instance to communicate with temperature IC
DallasTemperature tempSensor(&oneWire);  // pass the oneWire reference to Dallas Temperature


void setup()   {
  
  Serial.begin(9600);

  tempSensor.begin();                                    // initialize the temp sensor
  tempSensor.getAddress(thermometerAddress, 0);          // find the address of the DS18B20 on the one wire bus
  tempSensor.setResolution(thermometerAddress, 11);      // set the temperature resolution (9-12)
}


void loop() {

  // show temperature °C
  //tempSensor.requestTemperatures();                           // request temperature sample from sensor on the one wire bus
  //Serial.print(tempSensor.getTempC(thermometerAddress));      // serial debug output of temperature sensor data
  //Serial.print(" ");                                          // multiple plot data points are separated by space, comma, or \t

  // show analog 0 input               
  //Serial.print(analogRead(A0));                               // read the analog input and send the data to the serial plotter
  //Serial.print(" ");

  // show analog 0 input scaled to fit other data             
  Serial.print(analogRead(A0)/32);                            // read the analog input and send the data to the serial plotter
  Serial.print(" ");
  
  Serial.print(32);                                          // plot straight line to set upper scaling boundary
  Serial.print(" ");

  Serial.print(0);                                           // plot straight line to set lower scaling boundary
  Serial.print(" ");

  Serial.print(16);                                          // plot straight line to set a visual reference line
  Serial.print(" ");
  
  Serial.println();                                          // finalize the plot for all current y-axis data points                                        
}





