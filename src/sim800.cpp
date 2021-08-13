#include "sim800.h"


Sim800::Sim800(int baud, int ssRx, int ssTx):sim800Port(ssRx,ssTx);baud(baud){
    
    return;
}

bool Sim800::activatePort(){
    sim800Port.begin(9600);
    delay(1000);
}

bool Sim800::sortResponse(String resp){

    short lineCounter = 0;
    response.raw="";
    response.line0="";
    response.line1="";
    response.line2="";
    response.line3="";

    for(int i = 0; i < resp.length(); i++){

        if(lineCounter == 0)
            response.line0 += resp[i];
        if(lineCounter == 1)
            response.line1 += resp[i];
        if(lineCounter == 2)
            response.line2 += resp[i];
        if(lineCounter == 3)
            response.line3 += resp[i];

        if(resp[i]=='\n'){
            lineCounter ++;
        } 

    }
    response.raw = resp;


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
