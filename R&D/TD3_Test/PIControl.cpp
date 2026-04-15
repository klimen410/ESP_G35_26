#include "mbed.h"
#include "C12832.h"
#include "Variables.h"
#include "Controller.h"

DigitalOut MDBEnable(PC_7, 0); 
DigitalOut BPE1(PC_0, 1); DigitalOut BPE2(PC_1, 1);
PwmOut PWM_L(PC_9); PwmOut PWM_R(PC_8);
C12832 lcd(D11, D13, D12, D7, D10);
DigitalIn Down(A3, PullDown);

BuggyEncoder encL(PC_10, PC_12, PPR);
BuggyEncoder encR(PB_13, PB_14, PPR);

PISystem piL(0.0058f, 0.03f); 
PISystem piR(0.0065f, 0.03f);

int main() {
    PWM_L.period_us(50); PWM_R.period_us(50);
    PWM_L.write(0.5f); PWM_R.write(0.5f);
    MDBEnable = 0;
    
    Timer loopTimer;
    int lastL = 0, lastR = 0;
    bool isRunning = false;

    while(1) {
        if (!isRunning) {
            lcd.locate(0,0);
            lcd.printf("Task 1, Constant speed over slopes");
            if (Down == 1) {
                isRunning = true;
                MDBEnable = 1;
                lastL = encL.getPulses();
                lastR = -encR.getPulses(); 
                piL.reset(); piR.reset();
                piL.setTargetSpeed(TARGET_RPM);
                piR.setTargetSpeed(TARGET_RPM);
                lcd.cls();
            }
            wait_ms(100);
            continue;
        }

        loopTimer.reset(); loopTimer.start();

        int cL = encL.getPulses(); //current pulses
        int cR = -encR.getPulses(); 
        
        float rpmL = encL.calculateRPM(cL, lastL, SAMPLE_TIME, COUNTS_PER_REV);
        float rpmR = encR.calculateRPM(cR, lastR, SAMPLE_TIME, COUNTS_PER_REV);
        lastL = cL; lastR = cR;

        float updateL = piL.update(rpmL, SAMPLE_TIME);
        float updateR = piR.update(rpmR, SAMPLE_TIME);

        float pwmL = 0.5f - updateL;
        float pwmR = 0.5f - updateR;
        
        float pwmLDis= 1.0f - pwmL;
        float pwmRDis= 1.0f - pwmR;
    
        PWM_L.write(pwmL);
        PWM_R.write(pwmR);

        lcd.locate(0,0);
        lcd.printf("L_RPM:%.1f R_RPM:%.1f", rpmL, rpmR);
        lcd.locate(0,12);
        lcd.printf("L_PWM:%.3f R_PWM:%.3f", pwmLDis, pwmRDis);

        while (loopTimer.read() < SAMPLE_TIME);
    }
}