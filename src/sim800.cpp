#include "sim800.h"


Sim800::Sim800(int baud, Stream &debugPort,HardwareSerial &hardwareSerial):hwSerial(true){
    this->sim800Port = &hardwareSerial;
    this->debugPort = &debugPort;
    this->baud = baud;
    return;
}

Sim800::Sim800(int baud,Stream &debugPort,SoftwareSerial &softwareSerial):hwSerial(false){
    this->sim800Port = &softwareSerial;
    this->debugPort = &debugPort;
    this->baud = baud;
    return;
}

bool Sim800::activatePort(){

    if(hwSerial){
        static_cast<HardwareSerial*>(sim800Port)->begin(baud);
    }else{
        static_cast<SoftwareSerial*>(sim800Port)->begin(baud);
    }
    delay(1000);
    status.portActive = true;
}


bool Sim800::deActivatePort(){
    if(hwSerial){
        static_cast<HardwareSerial*>(sim800Port)->end();
    }else{
        static_cast<SoftwareSerial*>(sim800Port)->end();
    }
    delay(1000);
    status.portActive = false;
}

/*
    Populate RESPONSE response
*/
bool Sim800::sortResponse(String resp){

    short lineCounter = 0;
    for(int i =0; i < sizeof(response.lines)/sizeof(String); i++){
        response.lines[i] = "";
    }
    for(int i = 0; i < resp.length(); i++){
        if(resp[i]!='\n' && resp[i]!='\r')
            response.lines[lineCounter] += resp[i];
        if(resp[i]=='\n'){
            lineCounter ++;
        } 
    }
    response.raw = resp;
    response.size = lineCounter;
}

/*
    Send command and return true if "OK" was part of the response
    Populate RESPONSE
*/
bool Sim800::sendCommand(String cmd){
    String trash;
    String resp = "";

    if(sim800Port->available())
        trash = sim800Port->readString();

    sim800Port->print(cmd);
    this->debugPort->println("Trying command");
    delay(SIM800_RESPONSE_DELAY);

    resp = sim800Port->readString();
    
    if(resp.indexOf("OK") != -1){
        sortResponse(resp);
        return true;
    }else{
        return false;
    }


}

bool Sim800::configureSim800(){
    this->debugPort->println("configureSim800()");
}
