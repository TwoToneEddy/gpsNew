#ifndef SIM800_H
#define SIM800_H
#include <Arduino.h>
#include <SoftwareSerial.h>

#define SIM800_RESPONSE_DELAY 800

class Sim800
{
    public:
        Sim800(int baud, Stream &debugPort, SoftwareSerial &softwareSerial);
        Sim800(int baud, Stream &debugPort, HardwareSerial &hardwareSerial);
        Stream *sim800Port;
        Stream *debugPort;
        bool activatePort();
        bool deActivatePort();
        bool configureSim800();
        bool sortResponse(String resp);
        bool sendCommand(String cmd);


        struct RESPONSE{
            String raw;
            String lines[10];
            int size;
        };

        struct STATUS{
            bool portActive;
            bool error;
            short errorCode;
        };


        RESPONSE response;
        STATUS status;
        int baud;
        bool hwSerial;
};

#endif