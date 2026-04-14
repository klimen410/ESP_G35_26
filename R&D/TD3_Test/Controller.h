#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "mbed.h"
#include "QEI.h"

class BuggyEncoder {
private:
    QEI wheel;
    int ppr;
public:
    BuggyEncoder(PinName channelA, PinName channelB, int Ppr) 
        : wheel(channelA, channelB, NC, ppr, QEI::X4_ENCODING), ppr(Ppr) {}

    void reset() { wheel.reset(); }
    int getPulses() { return wheel.getPulses(); }
    
    float calculateRPM(int currentPulses, int lastPulses, float dt, int countsPerRev) {
        float deltaPulses = (float)(currentPulses - lastPulses);
        float tickRate = deltaPulses / dt;
        return (tickRate * 60.0f) / (float)countsPerRev;
    }
};

class PISystem {
private:
    float kp;
    float ki;
    float targetRPM;
    float integralError;
    float maxIntegral;

public:
    PISystem(float Kp, float Ki, float maxI = 0.5f) 
        : kp(Kp), ki(Ki), targetRPM(0), integralError(0), maxIntegral(maxI) {}

    void setTargetSpeed(float rpm) {
        targetRPM = rpm;
    }

    float update(float measuredRPM, float dt) {
        float error = targetRPM - measuredRPM;
        integralError += error * dt;

        if (integralError > maxIntegral) integralError = maxIntegral;
        if (integralError < -maxIntegral) integralError = -maxIntegral;

        float output = (kp * error) + (ki * integralError);
        
        float pwmValue = 0.5f + output;

        if (pwmValue > 1.0f) pwmValue = 1.0f;
        if (pwmValue < 0.0f) pwmValue = 0.0f;
        
        return pwmValue;
    }
    
    void reset() {
        integralError = 0;
    }
};
#endif