#ifndef _IM920_h_
#define _IM920_h_

#include "IM920_conf.h"

#include "mbed.h"
//#include "FunctionPointer.h"
#include "CBuffer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#if defined(DEBUG)
#define DBG(x, ...) std::printf("[DBG]" x "\r\n", ##__VA_ARGS__);
#define WARN(x, ...) std::printf("[WARN]" x "\r\n", ##__VA_ARGS__);
#define ERR(x, ...) std::printf("[ERR]" x "\r\n", ##__VA_ARGS__);
#define INFO(x, ...) std::printf("[INFO]" x "\r\n", ##__VA_ARGS__);
#else
#define DBG(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#define INFO(x, ...)
#endif

class IM920 {
public:
    enum Response {
        RES_NULL,
        RES_RDID,
        RES_RDNN,
        RES_RDRS,
    };

    enum Mode {
        MODE_COMMAND,
        MODE_DATA_RX,
    };

    enum Status {
        STAT_NONE,
        STAT_SLEEP,
    };

    IM920 (PinName tx, PinName rx, PinName busy, PinName reset, int baud = IM920_BAUD);

    //int init ();
    void poll ();
    int send (char *buf, int len);
    int recv (char *buf, int len);

    /*void attach (void(*fptr)() = NULL) {
        _func.attach(fptr);
    }
    template<typename T>
    void attach (T* tptr, void (T::*mptr)()) {
        if ((mptr != NULL) && (tptr != NULL)) {
            _func.attach(tptr, mptr);
        }
    }*/

    // ----- IM920_util.cpp -----
    int setNode (int node);
    int getNode ();
    int setCh (int ch);
    int setPower (int pwr);
    int setSpeed (int spd);
    int getRssi ();
    int sleep ();
    int wakeup ();
    int test ();

    // ----- IM920_cmd.cpp -----
    int sendCommand(const char * cmd, Response res = RES_NULL, int timeout = DEFAULT_WAIT_RESP_TIMEOUT);
    int sendData(const char * data, int len, int timeout = CFG_TIMEOUT);

private:
    UnbufferedSerial _im;
    DigitalIn *_busy;
    DigitalInOut *_reset;
    int _baud;
    //FunctionPointer _func;

    struct STATE {
        int id, node, rssi;

        time_t time;
        bool initialized;
        volatile Mode mode;
        volatile Status status;
        volatile bool ok, failure;
        volatile Response res;
        int n;
        char buf[CFG_BUF_SIZE];

        CircBuffer<char> *data;
        volatile bool received;
    } _state;

    // ----- IM920_util.cpp -----
    int x2i (char c);
    char i2x (int i);

    // ----- IM920_msg.cpp -----
    void recvData (char c);
    int parseMessage ();
    void msgOk (const char*);
    void msgError (const char*);
    void msgConnect (const char*);
    void resRDID (const char *buf);
    void resRDNN (const char *buf);
    void resRDRS (const char *buf);

    // ----- IM920_cmd.cpp -----
    void clearFlags ();
    int cmdENWR ();
    int cmdDSWR ();
    int cmdRDID ();
    int cmdSTNN (int n);
    int cmdRDNN ();
    int cmdSRID (int n);
    int cmdERID ();
    int cmdSTCH (int n);
    int cmdRDRS ();
    int cmdSTPO (int n);
    int cmdSTRT (int n);
    int cmdSBRT (int n);
    int cmdDSRX ();
    int cmdENRX ();
    int cmdEGRX ();
    int cmdDGRX ();

    // ----- IM920_hal.cpp -----
    void setReset (bool flg);
    void isrUart ();
    int getUart ();
    void putUart (char c);
    int lockUart (int ms);
    void unlockUart ();
    void initUart (PinName busy, PinName reset, int baud);
 };

#endif


/*#ifndef _IM920_H_
#define _IM920_H_

#include "mbed.h"
#define BUFFERSIZE 32

//static UnbufferedSerial *IM920s;

//19,200bps データ長8bit　ストップビット1　パリティなし
//TXDU ノード番号,<date>
class IM920s : public UnbufferedSerial{
    private:
    char com[5],buffer[32],node[5],can,getter[2];
    int i;
    
    public:
    IM920s(PinName TX, PinName RX);
    //16進数のデータを送信
    void senddata(char node, char buffer);

};

#endif*/