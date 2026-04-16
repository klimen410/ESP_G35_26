#ifndef SENSORS_H
#define SENSORS_H

#include "mbed.h"
#include "C12832.h"
#include "Variables.h"

class Sensors {
private:
    AnalogIn s1, s2, s3,s4;
    float voltages[4];
    float weights[4];

public:
    Sensors(PinName p1, PinName p2, PinName p3, PinName p4)
        : s1(p1), s2(p2), s3(p3), s4(p4) {
            weights[0] = -2.0f; 
            weights[1] = -1.0f; 
            weights[2] = 1.0f; 
            weights[3] = 2.0f;
        }

    void update() {
        voltages[0] = s1.read() * 3.3f;
        voltages[1] = s2.read() * 3.3f;
        voltages[2] = s3.read() * 3.3f;
        voltages[3] = s4.read() * 3.3f;
        for (int i = 0; i < 4; i++) {
        if (voltages[i] > 3.3f) {
            voltages[i] = 3.3f;
            }
        }
    }

    float getVoltage(int index){
        if (index >= 0 && index < 4) return voltages[index];
        return 0.0f;
    }
    float getLineError() {
        update(); 
        
        float totalWeight = 0;
        float totalIntensity = 0;
        float weights[4] = {-2.0f, -1.0f, 1.0f, 2.0f};

        for (int i = 0; i < 4; i++) {

            float intensity = 3.3f - voltages[i]; 
            
            if (intensity < 1.0f) intensity = 0; 

            totalWeight += (weights[i] * intensity);
            totalIntensity += intensity;
        }

        if (totalIntensity < 0.1f) return 0.0f;

        return totalWeight / totalIntensity;
    }
    int getBlackCount() {
        int blackCount = 0;
        for (int i = 0; i < 4; i++) {
            if (voltages[i] < BLACK_THRESHOLD) blackCount++;
        }
        return blackCount;
    }
    bool isBlack(int index, float threshold = BLACK_THRESHOLD) {
        return (voltages[index] < threshold);
    }
};
#endif