#ifndef ACTIONS_H
#define ACTIONS_H

#include "mbed.h"
#include "QEI.h"
#include "C12832.h"
#include "Controller.h"
#include "Variables.h"
class BuggyActions {
private:
    PwmOut &pwmLeft;
    PwmOut &pwmRight;
    BuggyEncoder &encLeft;
    BuggyEncoder &encRight;
    PISystem &piLeft, &piRight;
    C12832 &lcd;
    public:

    BuggyActions(PwmOut &l, PwmOut &r, BuggyEncoder &el, BuggyEncoder &er, C12832 &Lcd, PISystem &pl, PISystem &pr)
        : pwmLeft(l), pwmRight(r), encLeft(el), encRight(er), lcd(Lcd), piLeft(pl), piRight(pr) {}


    void stopMotors() {
        pwmLeft.write(STOP_PWM);
        pwmRight.write(STOP_PWM);
        piLeft.reset();
        piRight.reset();
        wait_ms(50);
    }

    void turnAround180(int targetCounts) {
        encLeft.reset();
        encRight.reset();
        
        pwmLeft.write(TURN_SPEED); 
        pwmRight.write(1-TURN_SPEED); 

        while (true) {
            int counts = abs(encRight.getPulses()); 
            
            lcd.locate(0,0);
            lcd.printf("Action: Turning 180");
            lcd.locate(0,10);
            lcd.printf("Progress: %d / %d", counts, targetCounts);
            
            if (counts >= targetCounts) break;
        }
        stopMotors();
        lcd.cls();
        lcd.printf("Turn Complete");
    }
    void normalDrive(float targetL, float targetR, float rpmL, float rpmR, float dt) {
        float outL = piLeft.update(targetL - rpmL, dt);
        float outR = piRight.update(targetR - rpmR, dt);
        
        float finalL = 0.5f + outL;
        float finalR = 0.5f + outR;

        if (finalL > 1.0f) finalL = 1.0f; if (finalL < 0.0f) finalL = 0.0f;
        if (finalR > 1.0f) finalR = 1.0f; if (finalR < 0.0f) finalR = 0.0f;

        pwmLeft.write(finalL);
        pwmRight.write(finalR);
    }
};

#endif