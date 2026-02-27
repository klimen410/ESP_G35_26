#include "C12832.h"
#include "mbed.h" 
#include "QEI.h"

//Physical Constants 
#define WHEEL_DIAMETER  0.0796f          // meters
#define GEAR_RATIO      15               // 15:1 gearbox
#define PI              3.14159265359f
#define PPR             256              // Pulses Per Revolution

//PWM Settings
#define STOP_PWM       0.5f
#define DRIVE_SPEED    0.677f            // 0.2 offset from 0.5
#define TURN_SPEED     0.655f

//X4 encoding = 4 edges per pulse. Total counts per one wheel revolution:
#define COUNTS_PER_REV   (PPR * 4 * GEAR_RATIO)
#define STRAIGHT_COUNTS  1225
#define TURN_90_COUNTS   590
#define TURN_70_COUNTS   400

//Hardware Setup
PwmOut PWM_LEFT(PC_9);
PwmOut PWM_RIGHT(PC_8);
DigitalOut MDBEnable(PA_8);
DigitalOut BPE1(PC_0);
DigitalOut BPE2(PC_1);
DigitalIn Fire(D4);
C12832 LCD(D11, D13, D12, D7, D10);
//Encoder Class
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

void drive90cm(Encoder &encLeft, Encoder &encRight) {
    encLeft.reset();
    encRight.reset();

    Timer t;
    int prevLeft = 0, prevRight = 0;
    float sampleTime = 0.05; // 50ms sample rate
    PWM_LEFT.write(1.0f - DRIVE_SPEED); 
    PWM_RIGHT.write(1.0f - DRIVE_SPEED+0.0075); 

    while (true) {
        t.reset();
        t.start();
        
        int currLeft = encLeft.getPulses(); 
        int currRight = -1*encRight.getPulses(); 
        
        int avgCounts = (currLeft + currRight) / 2;

        // RPM Calculation logic
        float left_tickrate = (float)(currLeft - prevLeft) / sampleTime;
        float right_tickrate = (float)(currRight - prevRight) / sampleTime;
        
        float left_RPM = (left_tickrate * 60.0f) / (float)COUNTS_PER_REV;
        float right_RPM = (right_tickrate * 60.0f) / (float)COUNTS_PER_REV;

        // Display Status
        LCD.cls();
        LCD.locate(0,0);
        LCD.printf("Dist: %d / %d", avgCounts, STRAIGHT_COUNTS);
        LCD.locate(0,10);
        LCD.printf("L RPM: %0.1f", left_RPM);
        LCD.locate(0,20);
        LCD.printf("R RPM: %0.1f", right_RPM);

        if (avgCounts >= STRAIGHT_COUNTS) break;

        prevLeft = currLeft;
        prevRight = currRight;

        while(t.read() < sampleTime); 
    }
    stopMotors();
}
void turnLeft90(Encoder &encLeft, Encoder &encRight) {
    encLeft.reset();
    encRight.reset();
    
    float offset = TURN_SPEED - STOP_PWM;
    PWM_LEFT.write(1-(STOP_PWM-offset)); // Backwards
    PWM_RIGHT.write(STOP_PWM - offset); // Forwards (mirrored)

    while (true) {
        int counts = -1*encRight.getPulses(); 
        LCD.locate(0,0);
        LCD.printf("Turning Left");
        LCD.locate(0,10);
        LCD.printf("Counts: %d / %d", counts, TURN_90_COUNTS);
        
        if (counts >= TURN_90_COUNTS) break;
    }
    stopMotors();
}
void turnLeft70(Encoder &encLeft, Encoder &encRight) {
    encLeft.reset();
    encRight.reset();
    
    float offset = TURN_SPEED - STOP_PWM;
    PWM_LEFT.write(1-(STOP_PWM - offset)); 
    PWM_RIGHT.write(STOP_PWM - offset); 

    while (true) {
        int counts = -1 * encRight.getPulses(); 
        LCD.locate(0,0);
        LCD.printf("Turning Left 70");
        LCD.locate(0,10);
        LCD.printf("Counts: %d / %d", counts, TURN_70_COUNTS);
        
        if (counts >= TURN_70_COUNTS) break;
    }
    stopMotors();
}
void turnRight90(Encoder &encLeft, Encoder &encRight) {
    encLeft.reset();
    encRight.reset();
    
    float offset = TURN_SPEED - STOP_PWM;
    PWM_LEFT.write(STOP_PWM-offset); // Backwards
    PWM_RIGHT.write(1-(STOP_PWM - offset)); // Forwards (mirrored)

    while (true) {
        int counts = encLeft.getPulses(); 
        LCD.locate(0,0);
        LCD.printf("Turning Right");
        LCD.locate(0,10);
        LCD.printf("Counts: %d / %d", counts, TURN_90_COUNTS);
        
        if (counts >= TURN_90_COUNTS) break;
    }
    stopMotors();
}
void turnRight70(Encoder &encLeft, Encoder &encRight) {
    encLeft.reset();
    encRight.reset();
    
    float offset = TURN_SPEED - STOP_PWM;
    PWM_LEFT.write(STOP_PWM-offset); // Backwards
    PWM_RIGHT.write(1-(STOP_PWM - offset)); // Forwards (mirrored)

    while (true) {
        int counts = encLeft.getPulses(); 
        LCD.locate(0,0);
        LCD.printf("Turning Right 70");
        LCD.locate(0,10);
        LCD.printf("Counts: %d / %d", counts, TURN_70_COUNTS);
        
        if (counts >= TURN_70_COUNTS) break;
    }
    stopMotors();
}
int main() {
    PWM_LEFT.period_us(50);
    PWM_RIGHT.period_us(50);
    
    MDBEnable = 0; BPE1 = 1; BPE2 = 1;
    stopMotors();

    Encoder encoderLeft(PC_10, PC_12);
    Encoder encoderRight(PB_13, PB_14);

    LCD.cls();
    LCD.printf("Ready! Starting in 3s");
    PWM_LEFT.write(0.5f);
    PWM_RIGHT.write(0.5f);
    wait(3.0);
    LCD.cls();
    LCD.locate(0,0);
    LCD.printf("Waiting for Fire Input");

    while(1){
        if(Fire == 1){ //does square function
            LCD.cls();
            LCD.locate(0,0);
            LCD.printf("Starting");
            MDBEnable=1;
            wait(1.0);
             for (int i=0; i<4; i++){
                drive90cm(encoderLeft, encoderRight);
                turnLeft90(encoderLeft, encoderRight);
            }
            turnLeft70(encoderLeft, encoderRight);
            for (int i=0; i<4; i++){
                drive90cm(encoderLeft, encoderRight);
                turnRight90(encoderLeft, encoderRight);
            }
            turnRight70(encoderLeft, encoderRight);
            MDBEnable=0;
        }
    }
}