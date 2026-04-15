#ifndef SENSORS_H
#define SENSORS_H

#include "mbed.h"
#include "C12832.h"
#include "Variables.h"

class Sensors {
private:
    AnalogIn s1, s2, s3,s4;
    float voltages[4];

public:
    Sensors(PinName p1, PinName p2, PinName p3, PinName p4)
        : s1(p1), s2(p2), s3(p3), s4(p4) {}
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

    bool isBlack(int index, float threshold = BLACK_THRESHOLD) {
        return (voltages[index] < threshold);
    }
};
#endif