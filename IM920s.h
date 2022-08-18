#define _IM920_H_
#include "mbed.h"
//#include <string>
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

