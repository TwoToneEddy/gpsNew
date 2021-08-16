#include <Arduino.h>
#include <SoftwareSerial.h>
#include "sim800.h"

#define DEBUG_PORT_HW
//#define DEBUG_PORT_SW

SoftwareSerial gsmPort = SoftwareSerial(6,7);

#ifdef DEBUG_PORT_HW
Stream *debugPort = &Serial;
#endif

#ifdef DEBUG_PORT_SW
SoftwareSerial ssdb = SoftwareSerial(8,9);
Stream *debugPort = &ssdb;
#endif



Sim800 gsm = Sim800(9600,*debugPort,gsmPort);

void setup(){

  #ifdef DEBUG_PORT_HW
  static_cast<HardwareSerial*>(debugPort)->begin(9600);
  #else
  static_cast<SoftwareSerial*>(debugPort)->begin(9600);
  #endif

  debugPort->println("Serial open, configuring sim800");
  gsm.configureSim800();
  gsm.activatePort();

  while(!gsm.sendCommand("AT\r\n")){

  }

  for(int i =0; i < gsm.response.size; i++){
    debugPort->print(i);debugPort->println(gsm.response.lines[i]);
  }


}

void loop(){

}