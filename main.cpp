#include "mbed.h"
#include "PS3.h"
//#include "IM920s.h"
//#include <string.h>

// main() runs in its own thread in the OS
int main()
{
    PS3 ps3(D1,D0);
    //IM920s im920s(A0,A1);

    while (true) {

        //im920s.senddata(0003,(char)ps3.getButtonState(PS3::sikaku));   
        //ps3.getSELECTState();
        ps3.getButtonState(PS3::sikaku);

        wait_us(6000);
    }
}

