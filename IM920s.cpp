#include "IM920s.h"
#include "mbed.h"
//#include <string>


IM920s::IM920s(PinName TX, PinName RX):UnbufferedSerial(TX, RX){
    UnbufferedSerial::baud(19200);
};
void IM920s::senddata(char node, char buffer){
    //for(i=0;buffer[i] !='\0';i++)
    //TXDUとノード番号も
    com[0]='T'; com[1]='X'; com[2]='D'; com[3]='U'; com[4] = ' ';
    can = ' ';
    getter[0]='\r';getter[1]='\n';

    UnbufferedSerial:: write(&com,5);
    UnbufferedSerial:: write(&node,4);
    UnbufferedSerial:: write(&can,1);
    UnbufferedSerial:: write(&buffer,BUFFERSIZE);
    UnbufferedSerial:: write(&getter,2);
};