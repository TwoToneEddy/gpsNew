#include "sim800.h"

#define BUZZER_PIN  10

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

/*
    Tasks to do every tick. To be called by main routine every loop
*/
bool Sim800::sim800Task(){


    #ifdef DEBUG_GSM
    this->debugPort->println("sim800Task()");
    #endif

    sim800Port->flush();

}

void Sim800::flush(){
    String trash = "";
    if(sim800Port->available()){
        trash = sim800Port->readString();
        #ifdef DEBUG_GSM
        this->debugPort->print("Clearing from buffer: ");this->debugPort->println(trash);
        #endif
    }
}

void Sim800::setError(short errorCode){
    #ifdef DEBUG_GSM
    this->debugPort->print("ERROR:");this->debugPort->println(errorCode);
    #endif
    status.error = true;
    status.errorCode |= errorCode;
}

void Sim800::debugResponse(){

    #ifdef DEBUG_GSM
    for(int i =0; i < response.size; i++){
        this->debugPort->print(i);this->debugPort->println(response.lines[i]);
    }
    this->debugPort->println();
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



short Sim800::readSMS(uint8_t index){
    String resp = "";
    String _buffer="";
    
    if(( _readSerial().indexOf("ER")) != -1)
    {
    	return SEND_COMMAND_FAIL;
    }

    sim800Port->print (F("AT+CMGR="));
    sim800Port->print (index);
    sim800Port->print ("\r");
    _buffer=_readSerial();
    
    if (_buffer.indexOf("CMGR") == -1)
    {
    	return SEND_COMMAND_FAIL;
    }


    resp = _readSerial(10000);


    #ifdef DEBUG_GSM
    this->debugPort->print("Trying command: ");this->debugPort->println("AT+CMGR=");
    #endif


    #ifdef DEBUG_GSM
    this->debugPort->print("Response: ");this->debugPort->print(resp);
    #endif

    if(resp.indexOf("OK") != -1){
        sortResponse(resp);
        this->status.messageAttempts = 0;
        return SEND_COMMAND_SUCCESS;
    }else{

        if(this->status.messageAttempts >= MESSAGE_ATTEMPT_LIMIT){
            setError(INVALID_RESPONSE_ERROR);
            return SEND_COMMAND_FAIL_CANCEL;
        }
        this->status.messageAttempts++;
        delay(500);
        return SEND_COMMAND_FAIL;
    }


}
/*
    Send command and return true if "OK" was part of the response
    Populate RESPONSE
*/
short Sim800::sendCommand(char* cmd){
    String resp = "";
    //flush();

    #ifdef DEBUG_GSM
    this->debugPort->print("Trying command: ");this->debugPort->println(cmd);
    #endif

    sim800Port->print(cmd);
    resp = _readSerial();




    #ifdef DEBUG_GSM
    this->debugPort->print("Response: ");this->debugPort->print(resp);
    #endif

    if(resp.indexOf("OK") != -1){
        sortResponse(resp);
        this->status.messageAttempts = 0;
        return SEND_COMMAND_SUCCESS;
    }else{

        if(this->status.messageAttempts >= MESSAGE_ATTEMPT_LIMIT){
            setError(INVALID_RESPONSE_ERROR);
            return SEND_COMMAND_FAIL_CANCEL;
        }
        this->status.messageAttempts++;
        delay(500);
        return SEND_COMMAND_FAIL;
    }


}

bool Sim800::configureSim800(){
    this->debugPort->println("configureSim800()");
    tone(BUZZER_PIN, 440, 50);
    while(sendCommand(AUTO_BAUD_CMD) == SEND_COMMAND_FAIL);
    debugResponse();
    tone(BUZZER_PIN, 440, 50);
    while(sendCommand(TEXT_MODE_CMD)== SEND_COMMAND_FAIL);
    debugResponse();
    while(sendCommand(WAKE_CMD)== SEND_COMMAND_FAIL);
    debugResponse();
    while(sendCommand(DELETE_MSGS_CMD)== SEND_COMMAND_FAIL);
    debugResponse();
    while(sendCommand(CHECK_BATTERY_CMD)== SEND_COMMAND_FAIL);
    debugResponse();

    while(sendCommand(SLEEP_CMD)== SEND_COMMAND_FAIL);
    debugResponse();
}


int Sim800::getMostRecentMSGIndex(String rxString){
    char buf[4];
    char catInt[4];
    rxString.toCharArray(buf,rxString.length());

    if(isdigit(buf[rxString.length()-4])){
        sprintf(catInt,"%c%c",buf[rxString.length()-4],buf[rxString.length()-3]);
    }else{
        sprintf(catInt,"%c",buf[rxString.length()-3]);
    }

    return atoi(catInt);

}

/*
    Reads message at index, populates MSG_CONTENTS message
*/
bool Sim800::processMessage(int index){

    char cmd[24]="";
    int i = 0;
    int lineCounter = 0;
    int numberStart = 0;
    int numberEnd = 0;
    int bodyStart = 0;
    int bodyEnd = 0;
    String buf;

    message.message = "";
    message.senderNumber = "";

    sprintf(cmd,"AT+CMGR=%d\r\n",index);
    while(readSMS(index) == SEND_COMMAND_FAIL);
    debugResponse();

    while(response.lines[1][i] != '+' || response.lines[1][i+1] != '4')
        i++;

    numberStart = i;

    // Find end of number by looking for ,
    while(response.lines[1][i] != ',')
        i++;
    numberEnd = i-1;

    for(i = numberStart; i < numberEnd; i++)
        message.senderNumber+=response.lines[1][i];

    if(message.senderNumber.length()!=13){
        #ifdef DEBUG_GSM
        this->debugPort->print("Invalid recipient, default used");
        #endif
        message.senderNumber = DEFAULT_RECIPIENT;
    }

    message.message = response.lines[2];

    #ifdef DEBUG_GSM
    this->debugPort->print("SMS sender :");this->debugPort->println(message.senderNumber);
    this->debugPort->print("Number Length :");this->debugPort->println(message.senderNumber.length());
    this->debugPort->print("Message :");this->debugPort->println(message.message);
    #endif

}

/*
    Checks for indication of message in serial buffer. Calls processMessage which populates
    MSG_CONTENTS message. Returns true if a message has been found. Deletes message after processing
*/
bool Sim800::checkForMessage(){
    String buffer = "";
    if(this->sim800Port->available()){
        buffer = _readSerial();

        #ifdef DEBUG_GSM
        this->debugPort->print("Got sms interrupt: ");this->debugPort->println(buffer);
        #endif

        if(buffer.startsWith("+CMTI:",2)){
            newestMsgIndex=getMostRecentMSGIndex(buffer);

            #ifdef DEBUG_GSM
            this->debugPort->print("Message index:");this->debugPort->println(newestMsgIndex);
            #endif

            processMessage(newestMsgIndex);

            /*
            while(sendCommand(DELETE_MSGS_CMD) == SEND_COMMAND_FAIL);
            debugResponse();
            */
            #ifdef DEBUG_GSM
            this->debugPort->println("processMessage() complete!");
            #endif
           
            return true;
        }else{
            this->sim800Port->flush();
            return false;
        }
    }
    return false;
}

String Sim800::_readSerial()
{

    uint64_t timeOld = millis();

    #ifdef DEBUG_GSM
    this->debugPort->println("_readSerial()");
    #endif

    while (!sim800Port->available() && !(millis() > timeOld + TIME_OUT_READ_SERIAL))
    {
        delay(13);
    }

    String str;

    while(sim800Port->available())
    {
        if (sim800Port->available()>0)
        {
            str += (char) sim800Port->read();
        }
    }

    return str;

}

String Sim800::_readSerial(uint32_t timeout)
{

    uint64_t timeOld = millis();

    #ifdef DEBUG_GSM
    this->debugPort->println("_readSerial(timeout)");
    #endif

    while (!sim800Port->available() && !(millis() > timeOld + timeout))
    {
        delay(13);
    }

    String str;

    while(sim800Port->available())
    {
        if (sim800Port->available()>0)
        {
            str += (char) sim800Port->read();
        }
    }

    return str;

}

