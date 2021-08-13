#ifndef SIM800_H
#define SIM800_H
#include <Arduino.h>
#include <SoftwareSerial.h>

#define SIM800_RESPONSE_DELAY 800

class Sim800
{
    public:
        Sim800(int baud, int ssRx, int ssTx);
        SoftwareSerial sim800Port;
        bool activatePort();
        bool deActivatePort();
        bool configureSim800();
        bool sortResponse(String resp);
        bool sendCommand(String cmd);

        struct RESPONSE{
            String raw;
            String line0;
            String line1;
            String line2;
            String line3;
        };

        struct STATUS{
            bool portActive;
            bool error;
            short errorCode;
        };


        RESPONSE response;
        STATUS status;
};

#endif