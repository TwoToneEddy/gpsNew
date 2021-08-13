#include <Arduino.h>
#include "sim800.h"

Sim800 gsm = Sim800(9600,6,7);

void setup(){
  Serial.begin(9600);
  Serial.println("Serial open, configuring sim800");
  gsm.configureSim800();
  gsm.activatePort();

  while(!gsm.sendCommand("AT\r\n")){

  }
  Serial.print("Line0 = ");Serial.println(gsm.response.line0);
  Serial.print("Line1 = ");Serial.println(gsm.response.line1);
  Serial.print("Line2 = ");Serial.println(gsm.response.line2);
  Serial.print("Line3 = ");Serial.println(gsm.response.line3);

}

void loop(){

}