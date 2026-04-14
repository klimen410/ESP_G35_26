#include "mbed.h"
#include "C12832.h"
#include "Variables.h"
#include "Controller.h"
#include "Actions.h"
#include "Sensors.h"


PwmOut PWM_L(PC_9);
PwmOut PWM_R(PC_8);
DigitalOut MDBEnable(PA_8);
DigitalOut BPE1(PC_0);
DigitalOut BPE2(PC_1);
C12832 LCD(D11, D13, D12, D7, D10);
Serial hm10(PA_11, PA_12);

BuggyEncoder encL(PC_10, PC_12, PPR);
BuggyEncoder encR(PB_13, PB_14, PPR);

PISystem piL(0.006f, 0.0015f);
PISystem piR(0.006f, 0.0015f);
PISystem linePI(15.0f, 0.5f, 30.0f);

Sensors lineSensors(PC_2, PC_3, PC_4, PB_1, PC_5, PA_3);
BuggyActions actions(PWM_L, PWM_R, encL, encR, LCD, piL, piR);

float calculateLineError() {
    lineSensors.update();
    float weights[6] = {-3.0f, -2.0f, -1.0f, 1.0f, 2.0f, 3.0f};
    float sum = 0;
    int count = 0;
    for (int i = 0; i < 6; i++) {
        if (lineSensors.getVoltage(i) < BLACK_THRESHOLD) {
            sum += weights[i];
            count++;
        }
    }
    return (count == 0) ? 0.0f : (sum / count);
}

int main() {
    // 1. Setup Motors
    PWM_L.period_us(50);
    PWM_R.period_us(50);
    PWM_L.write(STOP_PWM);
    PWM_R.write(STOP_PWM);
    hm10.baud(9600);
    MDBEnable = 1; BPE1 = 1; BPE2 = 1;

    Timer loopTimer;
    int lastL = 0, lastR = 0;

    while(1) {
        loopTimer.reset();
        loopTimer.start();
        if(hm10.readable()) {
            char val = hm10.getc();
            if(val == '1') {
                actions.turnAround180(TURN_180_COUNTS);
                lastL = encL.getPulses();
                lastR = -encR.getPulses(); 
                linePI.reset();
            }
        } else {
            lineSensors.update();
            int blackCount = 0;
            for (int i = 0; i < 6; i++) {
                if (lineSensors.getVoltage(i) < BLACK_THRESHOLD) {
                    blackCount++;
                }
            }

            // 3. STOP CONDITION: If all 6 sensors are black, stop and restart the loop
            if (blackCount == 6) {
                actions.stopMotors();
                LCD.locate(0,0);
                LCD.printf("STATUS: FINISH LINE ");
                LCD.locate(0,10);
                LCD.printf("All Sensors Black   ");
                
                // Stay in this "stopped" state until the loop finishes
                while (loopTimer.read() < SAMPLE_TIME);
                continue; // Skip the driving logic below and go back to the top of while(1)
            }
            float lineError = calculateLineError(); 
            float steerAdjustment = (linePI.update(lineError, SAMPLE_TIME) - 0.5f) * 100.0f; 

            float targetRPM_L = TARGET_RPM + steerAdjustment;
            float targetRPM_R = TARGET_RPM - steerAdjustment;
            piL.setTargetSpeed(targetRPM_L);
            piR.setTargetSpeed(targetRPM_R);

            // 4. Measure Actual Speed
            int currL = encL.getPulses();
            int currR = -encR.getPulses(); // Invert right side if motors face opposite

            float rpmL = encL.calculateRPM(currL, lastL, SAMPLE_TIME, COUNTS_PER_REV);
            float rpmR = encR.calculateRPM(currR, lastR, SAMPLE_TIME, COUNTS_PER_REV);
            
            lastL = currL;
            lastR = currR;

            // 5. Update Wheel Speed PI and Drive
            // Use the PISystem update which returns the 0.5-centered PWM
            float pwmL = piL.update(rpmL, SAMPLE_TIME);
            float pwmR = piR.update(rpmR, SAMPLE_TIME);

            PWM_L.write(pwmL);
            PWM_R.write(pwmR);

            // 6. Debugging to LCD
            LCD.locate(0,0);
            LCD.printf("Err:%.1f Adj:%.1f  ", lineError, steerAdjustment);
            LCD.locate(0,10);
            LCD.printf("L:%.1f R:%.1f   ", rpmL, rpmR);

            // 7. Wait for end of 50ms cycle
            while (loopTimer.read() < SAMPLE_TIME);
        }
    }
}