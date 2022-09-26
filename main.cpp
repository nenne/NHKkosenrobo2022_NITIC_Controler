#include "mbed.h"
#include "PS3.h"
#include "IM920s.h"
#include <mbed_wait_api.h>

PS3 ps3(D8,D2); //シリアル通信で使うピンの指定　データシート見ろ
IM920 im920(A0,A1,A2,A3); //tx,rx以降のピンは使用しないの適当かつ重複しないピンを指定する
//PS3 ps3(A0,A1);

void getdata(char *);

int main()
{
    char data[14];
    
    while (1) {
        getdata(data);
        im920.send(data,14);
        ps3.printdata();
        //wait_us(5000);
    }
    
}

void getdata(char d[]){
    d[0]=ps3.getButtonState(PS3::maru);
    d[1]=ps3.getButtonState(PS3::batu);
    d[2]=ps3.getButtonState(PS3::sankaku);
    d[3]=ps3.getButtonState(PS3::sikaku);
    d[4]=ps3.getButtonState(PS3::R1);
    d[5]=ps3.getButtonState(PS3::R2);
    d[6]=ps3.getButtonState(PS3::ue);
    d[7]=ps3.getButtonState(PS3::migi);
    d[8]=ps3.getButtonState(PS3::sita);
    d[9]=ps3.getButtonState(PS3::hidari);
    d[10]=ps3.getButtonState(PS3::L1);
    d[11]=ps3.getButtonState(PS3::L2);
    d[12]=ps3.getSELECTState() ;
    d[13]=ps3.getSTARTState() ;
}