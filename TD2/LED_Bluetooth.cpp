#include "mbed.h"

Serial hm10(PA_11, PA_12); //UART6 TX,RX
DigitalOut led(LED2);

int c; //the character we want to receive

int main() {
    hm10.baud(9600);
        while(1) {
        if(hm10.readable()){
            c = hm10.getc(); //read a single character
            if(c == 1){
            led = 1;
            }
            else if(c == 0){
            led = 0;
            }
        }
    }
}