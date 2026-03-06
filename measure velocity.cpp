#include "mbed.h"
#include "QEI.h"
#include "C12832.h"

//Physical constant
#define WHEEL_DIAMETER 0.0796f
#define GEAR_RATIO     15.0f
#define PI             3.1415926f
#define CPR            1024.0f      //256*4

//Lcd
C12832 LCD(D11, D13, D12, D7, D10);
QEI encL(PC_10, PC_12, NC, 256, QEI::X4_ENCODING);
QEI encR(PB_13, PB_14, NC, 256, QEI::X4_ENCODING);

Timer t;

int main() {
    int lastL = 0, lastR = 0;
    float lastTime = 0;
    
    t.start();
    LCD.cls();

    while(1) {
        float currTime = t.read();
        float dt = currTime - lastTime;

        //refresh every 0.1s
        if (dt >= 0.1f) {
            //Get the current cumulative pulse
            int currL = encL.getPulses();
            int currR = encR.getPulses();

            //calculate tick Rate
            float tickRateL = (float)(currL - lastL) / dt;
            float tickRateR = (float)(currR - lastR) / dt;

            //calculate Wheel RPM
            float rpmL = (tickRateL * 60.0f) / (CPR * GEAR_RATIO);
            float rpmR = (tickRateR * 60.0f) / (CPR * GEAR_RATIO);

            //calculate Wheel Velocity
            float velL = (rpmL / 60.0f) * (PI * WHEEL_DIAMETER);
            float velR = (rpmR / 60.0f) * (PI * WHEEL_DIAMETER);

            //LCD
            LCD.locate(0,0);
            LCD.printf("L:%.2f R:%.2f m/s", velL, velR); //left speed right speed  on screen
            LCD.locate(0,20);
            LCD.printf("RPM L:%.1f R:%.1f", rpmL, rpmR); //real time RPM

            //refresh
            lastL = currL;
            lastR = currR;
            lastTime = currTime;
        }
    }
}
