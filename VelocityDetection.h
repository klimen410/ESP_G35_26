#ifndef VELOCITY_DETECTION_H
#define VELOCITY_DETECTION_H

#include "mbed.h"

class VelocityDetection {
public:
    VelocityDetection(PinName leftA, PinName rightA);

    int getLeftTicks();                             // pulse counter for the left encoder
    int getRightTicks();                            // pulse counter for the right encoder
    void reset();

private:
    InterruptIn leftEncoder;
    InterruptIn rightEncoder;

    volatile int leftTicks;
    volatile int rightTicks;

    void leftRise();
    void rightRise();
};

#endif
