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

  for(int i =0; i < gsm.response.size; i++){
    Serial.print(i);Serial.println(gsm.response.lines[i]);
  }


}

void loop(){

}