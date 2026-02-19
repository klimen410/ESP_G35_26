#include "VelocityDetection.h"

VelocityDetection::VelocityDetection(PinName leftPin, PinName rightPin)

// construct encoder using specified pin
    : leftEncoder(leftPin),
      rightEncoder(rightPin),
      leftTicks(0),
      rightTicks(0)

{
    // count rising edges of encoder signals
    leftEncoder.rise(callback(this, &VelocityDetection::leftRise));
    rightEncoder.rise(callback(this, &VelocityDetection::rightRise));
}

void VelocityDetection::leftRise() {
    leftTicks++;        // counts up with every rising edge of the left encoderr
}

void VelocityDetection::rightRise() {
    rightTicks++;       // counts up with every rising edge of the right encoder
}

int VelocityDetection::getLeftTicks() {
    return leftTicks;
}

int VelocityDetection::getRightTicks() {
    return rightTicks;
}

void VelocityDetection::reset() {
    leftTicks = 0;
    rightTicks = 0;
}