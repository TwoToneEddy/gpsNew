#include <Arduino.h>
#include <Sim800L.h>
#include <SoftwareSerial.h>   

#define RX  6
#define TX  7

Sim800L GSM(RX, TX,10);

/*
 * In alternative:
 * Sim800L GSM;                       // Use default pinout
 * Sim800L GSM(RX, TX, RESET);        
 * Sim800L GSM(RX, TX, RESET, LED);
 */

char* text;
char* number;
bool error; 					//to catch the response of sendSms
String message;
//SoftwareSerial test(RX,TX);

void setup(){
  Serial.begin(9600);
  //test.begin(9600);
	GSM.begin(9600); 			
	
  GSM.delAllSms(); // this is optional
  while(!GSM.prepareForSmsReceive())
  {
    delay(1000);
  }
  Serial.println("Setup complete, Ready..");

}

void loop(){ 

  byte index = GSM.checkForSMS();
  if(index != 0)
  {
    message=GSM.readSms(index,false);
  	Serial.println("Got:");
    Serial.println(message);
    Serial.println("From:");
    Serial.println(GSM.getNumberSms(index));
    delay(1000);
    GSM.delAllSms();
    if(message[0] == 'P'){
  	  Serial.println("Position Request");
    }
    if(message[0] == 'R'){
  	  Serial.println("Ring Request");
    }

  }

  

}