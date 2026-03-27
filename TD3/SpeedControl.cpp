#include "C12832.h"
#include "mbed.h" 
#include "QEI.h"
#include "Variables.h"

PwmOut PWM_LEFT(PC_9);
PwmOut PWM_RIGHT(PC_8);

DigitalOut MDBEnable(PA_8);
DigitalOut BPE1(PC_0);
DigitalOut BPE2(PC_1);

C12832 LCD(D11, D13, D12, D7, D10);
class Encoder {
private:
    QEI wheel;
public:
    Encoder(PinName channelA, PinName channelB) : wheel(channelA, channelB, NC, PPR, QEI::X4_ENCODING) {}
    void reset() { wheel.reset(); }
    int getPulses() { return wheel.getPulses(); }
};
void stopMotors() {
    PWM_LEFT.write(STOP_PWM);
    PWM_RIGHT.write(STOP_PWM);
    wait_ms(50);
}
//This code use for in slope or resistance increase, the speed don't change
class SpeedController {
private:
    float Kp;               // Proportional gain
    float Ki;               // Integral gain
    float targetSpeed;      // Desired setpoint (RPM)
    float integralError;    // Accumulated error over time (Integral term)
    float currentPWM;       // Current calculated PWM duty cycle (0.0 to 1.0)
    Encoder &ENCODER_LEFT;
    Timer t;
public:
    SpeedController(float p, float i, Encoder &enc) : Kp(p), Ki(i), ENCODER_LEFT(enc),targetSpeed(0), integralError(0), currentPWM(0.5f) {}
    //Set the desired wheel speed
    void setTargetSpeed(float rpm) {
        targetSpeed = rpm;
    }

    //dt is the time time interval
    float update(float measuredRPM, float dt) {
        // Calculate the error between setpoint and feedback
        float error = targetSpeed - measuredRPM;
        
        // Update the integral term (error accumulation)
        integralError += error * dt;

        // PI Calculation
        float output = (Kp * error) + (Ki * integralError);
        
        // Offset the PI output from the neutral/stop point (0.5 for bipolar drive)
        currentPWM = 0.5f + output;
        return currentPWM;
    }
};

int main() {
    PWM_LEFT.period_us(50);
    PWM_RIGHT.period_us(50);
    PWM_LEFT.write(0.5f); 
    PWM_RIGHT.write(0.5f); 
    MDBEnable = 0; BPE1 = 1; BPE2 = 1;
    stopMotors();

    Encoder ENCODER_LEFT(PC_10, PC_12);
    Encoder ENCODER_RIGHT(PB_13, PB_14);

    SpeedController SpeedControlLeft(1,1,ENCODER_LEFT);
    SpeedController SpeedControlRight(1,1,ENCODER_RIGHT);
    ENCODER_LEFT.reset();
    ENCODER_RIGHT.reset();
    SpeedControl.setTargetSpeed(float rpm);
    while(1){
        t.reset();
        t.start();
        int currLeft = ENCODER_LEFT.getPulses(); 
        int currRight = -1*ENCODER_RIGHT.getPulses(); 
        int avgCounts = (currLeft + currRight) / 2;
        float left_tickrate = (float)(currLeft - prevLeft) / sampleTime;
        float right_tickrate = (float)(currRight - prevRight) / sampleTime;
        float left_RPM = (left_tickrate * 60.0f) / (float)COUNTS_PER_REV;
        float right_RPM = (right_tickrate * 60.0f) / (float)COUNTS_PER_REV;
        float left_PWM = SpeedControlLeft.update(left_RPM, 0.05);
        float right_PWM = SpeedControlRight.update(right_RPM, 0.05);
        MDBEnable = 1;
        PWM_LEFT.write(left_PWM); 
        PWM_RIGHT.write(right_PWM); 
    }

}
