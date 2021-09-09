#include <Arduino.h>
#include <SoftwareSerial.h>
#include "sim800.h"

#define GSM_BAUD 9600
#define DEBUG_BAUD 9600
#define BUZZER_PIN  10


#define DEBUG_LOOP
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

bool GSMPassthrough;

void checkSim800Status(){
  if(gsm.status.error){
    //Do something if error
  }
}


void setup(){

  #ifdef GSM_PORT_HW
  static_cast<SoftwareSerial*>(debugPort)->begin(DEBUG_BAUD);
  #else
  static_cast<HardwareSerial*>(debugPort)->begin(DEBUG_BAUD);
  #endif

  debugPort->println("Serial open, configuring sim800");

  GSMPassthrough = false;
  tone(BUZZER_PIN, 440, 50);
  delay(5000);
  gsm.activatePort();
  tone(BUZZER_PIN, 440, 50);
  delay(1000);
  if(!GSMPassthrough){
    gsm.configureSim800();
    tone(BUZZER_PIN, 880, 50);
  }
}

void loop(){

  while(GSMPassthrough){
    if (debugPort->available()) {      // If anything comes in Serial (USB),
      gsmPort->write(debugPort->read());   // read it and send it out Serial1 (pins 0 & 1)
    }
    if (gsmPort->available()) {     // If anything comes in Serial1 (pins 0 & 1)
      Serial.write(gsmPort->read());   // read it and send it out Serial (USB)
    }
  }

  
  gsm.checkForMessage();
  gsm.sim800Task();
  checkSim800Status();
  delay(1000);
  
  #ifdef DEBUG_LOOP
  debugPort->println("Running loop() ");
  debugPort->flush();
  #endif
}