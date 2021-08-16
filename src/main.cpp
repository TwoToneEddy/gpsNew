#include <Arduino.h>
#include <SoftwareSerial.h>
#include "sim800.h"

#define GSM_BAUD 9600
#define DEBUG_BAUD 9600


// GSM Serial, debug SoftwareSerial
//#define GSM_PORT_HW


#ifdef GSM_PORT_HW
SoftwareSerial ss = SoftwareSerial(6,7);
Stream *debugPort = &ss;
Stream *gsmPort = &Serial;
Sim800 gsm = Sim800(GSM_BAUD,*debugPort,*gsmPort,true);
#else
SoftwareSerial ss = SoftwareSerial(6,7);
Stream *gsmPort = &ss;
Stream *debugPort = &Serial;
Sim800 gsm = Sim800(GSM_BAUD,*debugPort,*gsmPort,false);
#endif


void setup(){

  #ifdef GSM_PORT_HW
  static_cast<SoftwareSerial*>(debugPort)->begin(DEBUG_BAUD);
  #else
  static_cast<HardwareSerial*>(debugPort)->begin(DEBUG_BAUD);
  #endif

  debugPort->println("Serial open, configuring sim800");

  gsm.activatePort();
  gsm.configureSim800();

}

void loop(){

}