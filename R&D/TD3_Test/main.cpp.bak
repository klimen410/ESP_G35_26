#include "mbed.h"
#include "C12832.h"
#include "Variables.h"
#include "Controller.h"
#include "Actions.h"
#include "Sensors.h"

DigitalOut MDBEnable(PC_7, 0); 
DigitalOut BPE1(PC_0, 1); DigitalOut BPE2(PC_1, 1);
PwmOut PWM_L(PC_9); PwmOut PWM_R(PC_8);
C12832 LCD(D11, D13, D12, D7, D10);
Serial HM10(PA_11, PA_12);
DigitalIn Down(A3, PullDown);

BuggyEncoder encL(PC_10, PC_12, PPR);
BuggyEncoder encR(PB_13, PB_14, PPR);

PISystem piL(0.003f, 0.035f); 
PISystem piR(0.003f, 0.035f);

PISystem linePI(2.0f, 0.1f, 0.5f); 

Sensors sensors(PC_2, PC_3, PC_4, PB_1);
BuggyActions actions(PWM_L, PWM_R, encL, encR, LCD, piL, piR);

int main() {
    PWM_L.period_us(50); PWM_R.period_us(50);
    PWM_L.write(0.5f); PWM_R.write(0.5f);
    MDBEnable = 0; HM10.baud(9600);
    
    linePI.setTargetSpeed(0.0f);

    Timer loopTimer;
    int lastL = 0, lastR = 0;
    bool isRunning = false;

    LCD.cls();

    while(1) {
        if (!isRunning) {
            LCD.locate(0,0);
            LCD.printf("Press down to start");
            if (Down == 1) {
                isRunning = true;
                MDBEnable = 1;
                lastL = encL.getPulses();
                lastR = -encR.getPulses(); 
                piL.reset(); piR.reset(); linePI.reset();
                piL.setTargetSpeed(TARGET_RPM);
                piR.setTargetSpeed(TARGET_RPM);
                LCD.cls();
            }
            wait_ms(100);
            continue;
        }

        loopTimer.reset(); loopTimer.start();

        if(HM10.readable() && HM10.getc() == 1) {
            actions.turnAround180(TURN_180_COUNTS);
            lastL = encL.getPulses(); lastR = -encR.getPulses();
            piL.reset(); piR.reset(); linePI.reset();
        }


        float error = sensors.getLineError();
        float steerAdjustment = linePI.update(error, SAMPLE_TIME) * 17.5f;

        piL.setTargetSpeed(TARGET_RPM + steerAdjustment);
        piR.setTargetSpeed(TARGET_RPM - steerAdjustment);

        int cL = encL.getPulses();
        int cR = -encR.getPulses();
        float rpmL = encL.calculateRPM(cL, lastL, SAMPLE_TIME, COUNTS_PER_REV);
        float rpmR = encR.calculateRPM(cR, lastR, SAMPLE_TIME, COUNTS_PER_REV);
        lastL = cL; lastR = cR;

        float updateL = piL.update(rpmL, SAMPLE_TIME);
        float updateR = piR.update(rpmR, SAMPLE_TIME);

        float pwmL = 0.5f - updateL;
        float pwmR = 0.5f - updateR;

        if (pwmL > 0.8f) pwmL = 0.8f; if (pwmL < 0.2f) pwmL = 0.2f;
        if (pwmR > 0.8f) pwmR = 0.8f; if (pwmR < 0.2f) pwmR = 0.2f;

        PWM_L.write(pwmL);
        PWM_R.write(pwmR);

        LCD.locate(0,0);
        LCD.printf("E:%.1f Adjustment:%.1f", error, steerAdjustment);
        LCD.locate(0,12);
        LCD.printf("L:%.1f R:%.1f", rpmL, rpmR);
        LCD.locate(0,20);
        LCD.printf("V:%.1f|%.1f|%.1f|%.1f", 
            sensors.getVoltage(0), sensors.getVoltage(1), 
            sensors.getVoltage(2), sensors.getVoltage(3));

        while (loopTimer.read() < SAMPLE_TIME);
    }
}