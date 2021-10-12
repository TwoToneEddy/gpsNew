#ifndef SIM800_H
#define SIM800_H
#include <Arduino.h>
#include <SoftwareSerial.h>

#define SIM800_PORT_ACTIVATION_DELAY 500
#define SIM800_RESPONSE_DELAY 1000
#define TIME_OUT_READ_SERIAL 5000
#define DEBUG_GSM


#define AUTO_BAUD_CMD       "AT\r\n"
#define TEXT_MODE_CMD       "AT+CMGF=1\r\n"
#define WAKE_CMD            "AT+CSCLK=0\r\n"
#define SLEEP_CMD            "AT+CSCLK=2\r\n"
#define DELETE_MSGS_CMD     "AT+CMGD=2,4\r\n"
#define CHECK_BATTERY_CMD   "AT+CBC\r\n"

#define DEFAULT_RECIPIENT      "+447747465192"

#define MESSAGE_ATTEMPT_LIMIT   10
#define COMMUNICATION_TIMEOUT   50 // In units of 100ms

#define INVALID_RESPONSE_ERROR   0x01
#define NO_RESPONSE_ERROR        0x02

#define SEND_COMMAND_SUCCESS        0
#define SEND_COMMAND_FAIL           1 
#define SEND_COMMAND_FAIL_CANCEL    2    


class Sim800
{
    public:
        Sim800(int baud, Stream &debugPort, Stream &sim800Port, bool hwSerial);
        Stream *sim800Port;
        Stream *debugPort;
        bool activatePort();
        bool deActivatePort();
        bool configureSim800();
        bool sortResponse(String resp);
        void setError(short errorCode);
        short sendCommand(char* cmd);
        void debugResponse();
        bool sim800Task();
        bool checkForMessage();
        int getMostRecentMSGIndex(String rxString);
        bool processMessage(int index);
        void flush();
        short readSMS(uint8_t index);
        String _readSerial();
        String _readSerial(uint32_t timeout);


        struct RESPONSE{
            String raw;
            String lines[10];
            int size;
        };

        struct STATUS{
            bool portActive;
            bool error;
            short errorCode;
            short messageAttempts;
        };

        struct MSG_CONTENTS{
            String senderNumber;
            String message;
        };


        RESPONSE response;
        STATUS status;
        MSG_CONTENTS message;
        int baud,newestMsgIndex;
        bool hwSerial;
};

#endif