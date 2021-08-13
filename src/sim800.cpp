#include "sim800.h"


Sim800::Sim800(int baud, int ssRx, int ssTx):sim800Port(ssRx,ssTx){
    
    return;
}

bool Sim800::activatePort(){
    sim800Port.begin(9600);
    delay(1000);
    status.portActive = true;
}


bool Sim800::deActivatePort(){
    sim800Port.end();
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

    if(sim800Port.available())
        trash = sim800Port.readString();

    sim800Port.print(cmd);
    Serial.println("Trying command");
    delay(SIM800_RESPONSE_DELAY);

    resp = sim800Port.readString();
    
    if(resp.indexOf("OK") != -1){
        sortResponse(resp);
        return true;
    }else{
        return false;
    }


}

bool Sim800::configureSim800(){
    Serial.println("configureSim800()");
}
