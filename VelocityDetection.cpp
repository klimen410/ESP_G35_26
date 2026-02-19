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
    leftTicks++;
}

void VelocityDetection::rightRise() {
    rightTicks++;
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