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
    float Kp, Ki;               // Proportional gain
    float targetSpeed;      // Desired setpoint (RPM)
    float integralError;    // Accumulated error over time (Integral term)
    float currentPWM;       // Current calculated PWM duty cycle (0.0 to 1.0)
public:
    SpeedController(float p, float i) : Kp(p), Ki(i), targetSpeed(0), integralError(0) {}
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
        if (integralError > 10.0f) integralError = 10.0f;
        if (integralError < -10.0f) integralError = -10.0f;
        // PI Calculation
        float output = (Kp * error) + (Ki * integralError);
        
        // Offset the PI output from the neutral/stop point (0.5 for bipolar drive)
        currentPWM = 0.5f + output;
        if (currentPWM > 1.0f) currentPWM = 1.0f;
        if (currentPWM < 0.0f) currentPWM = 0.0f;

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

    SpeedController SpeedControlLeft(0.006f, 0.0015f);
    SpeedController SpeedControlRight(0.006f, 0.0015f);
    Timer t;
    int prevLeft = 0;
    int prevRight = 0;

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
        prevLeft = currLeft;
        prevRight = currRight;
        float left_PWM = SpeedControlLeft.update(left_RPM, SAMPLE_TIME);
        float right_PWM = SpeedControlRight.update(right_RPM, SAMPLE_TIME);

        PWM_LEFT.write(left_PWM); 
        PWM_RIGHT.write(right_PWM); 
        LCD.cls();
        LCD.locate(0,0);
        LCD.printf("Target: %.1f RPM", TARGET_RPM);
        LCD.locate(0,10);
        LCD.printf("L:%.1f R:%.1f", left_RPM, right_RPM);
        LCD.locate(0,20);
        LCD.printf("LPWM:%.2f RPWM:%.2f", left_PWM, right_PWM)
        while (t.read() < SAMPLE_TIME);
    }

}
