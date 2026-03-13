#include "mbed.h"
#include "C12832.h"

C12832 lcd(D11, D13, D12, D7, D10);
//Analog input pins
AnalogIn pot1(PC_2);
AnalogIn pot2(PC_3);
AnalogIn pot3(PC_4);
AnalogIn pot4(PB_1);
AnalogIn pot5(PC_5);
AnalogIn pot6(PA_3);

int main(){
    float sensor1;
    float sensor2;
    float sensor3;
    float sensor4;
    float sensor5;
    float sensor6;
    while (1){
        lcd.cls();
        //real value(0-3.3)
        sensor1 = pot1.read()*3.3;
        sensor2 = pot2.read()*3.3;
        sensor3 = pot3.read()*3.3;
        sensor4 = pot4.read()*3.3;
        sensor5 = pot5.read()*3.3;
        sensor6 = pot6.read()*3.3;
        //Display values on LCD
        lcd.locate(0,0);
        lcd.printf("S1: %.3f", sensor1);

        lcd.locate(0,10);
        lcd.printf("S2: %.3f", sensor2);

        lcd.locate(0,20);
        lcd.printf("S3: %.3f", sensor3);

        lcd.locate(60,0);
        lcd.printf("S4: %.3f", sensor4);

        lcd.locate(60,10);
        lcd.printf("S5: %.3f", sensor5);

        lcd.locate(60,20);
        lcd.printf("S6: %.3f", sensor6);
        wait_ms(20);
    }
}