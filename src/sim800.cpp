#include "sim800.h"


Sim800::Sim800(int baud, Stream &debugPort,Stream &sim800Port, bool hwSerial){
    this->sim800Port = &sim800Port;
    this->debugPort = &debugPort;
    this->baud = baud;
    this->hwSerial = hwSerial;
    this->status.messageAttempts=0;
    this->status.errorCode=0;
    this->status.error=false;
    return;
}

void Sim800::debugResponse(){

    #ifdef DEBUG_GSM
    for(int i =0; i < response.size; i++){
        this->debugPort->print(i);this->debugPort->println(response.lines[i]);
    }
    return;
    #else
    return;
    #endif
}

bool Sim800::activatePort(){

    if(hwSerial){
        static_cast<HardwareSerial*>(sim800Port)->begin(baud);
    }else{
        static_cast<SoftwareSerial*>(sim800Port)->begin(baud);
    }
    delay(SIM800_PORT_ACTIVATION_DELAY);
    status.portActive = true;
}


bool Sim800::deActivatePort(){
    if(hwSerial){
        static_cast<HardwareSerial*>(sim800Port)->end();
    }else{
        static_cast<SoftwareSerial*>(sim800Port)->end();
    }
    delay(SIM800_PORT_ACTIVATION_DELAY);
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

    #ifdef DEBUG_GSM
    this->debugPort->print("Trying command: ");this->debugPort->println(cmd);
    #endif

    delay(SIM800_RESPONSE_DELAY);

    resp = sim800Port->readString();
    
    if(resp.indexOf("OK") != -1){
        sortResponse(resp);
        this->status.messageAttempts = 0;
        return true;
    }else{
        this->status.messageAttempts++;
        return false;
    }


}

bool Sim800::configureSim800(){
    this->debugPort->println("configureSim800()");
    while(!sendCommand(AUTO_BAUD_CMD));
    debugResponse();
    while(!sendCommand(TEXT_MODE_CMD));
    debugResponse();
    while(!sendCommand(WAKE_CMD));
    debugResponse();
    while(!sendCommand(DELETE_MSGS_CMD));
    debugResponse();
    while(!sendCommand(CHECK_BATTERY_CMD));
    debugResponse();
}

/*
    Tasks to do every tick. To be called by main routine every loop
*/
bool Sim800::sim800Task(){

    if(this->status.messageAttempts >= MESSAGE_ATTEMPT_LIMIT){
        this->status.errorCode = 1;
        this->status.error = true;
    }

}
