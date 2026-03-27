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
Serial hm10(PA_11, PA_12); //UART6 TX,RX

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

void turnAround180(Encoder &encLeft, Encoder &encRight) {
    encLeft.reset();
    encRight.reset();
    
    float offset = TURN_SPEED - STOP_PWM;
    PWM_LEFT.write(1-(STOP_PWM-offset)); // Backwards
    PWM_RIGHT.write(STOP_PWM - offset); // Forwards (mirrored)

    while (true) {
        int counts = -1*encRight.getPulses(); 
        LCD.locate(0,0);
        LCD.printf("Turning Around");
        LCD.locate(0,10);
        LCD.printf("Counts: %d / %d", counts, TURN_180_COUNTS);
        
        if (counts >= TURN_180_COUNTS) break;
    }
    stopMotors();
}

int main() {
    PWM_LEFT.period_us(50);
    PWM_RIGHT.period_us(50);
    MDBEnable = 0; BPE1 = 1; BPE2 = 1;
    stopMotors();

    Encoder ENCODER_LEFT(PC_10, PC_12);
    Encoder ENCODER_RIGHT(PB_13, PB_14);

    LCD.cls();
    LCD.printf("Ready! Starting in 3s");
    PWM_LEFT.write(0.5f);
    PWM_RIGHT.write(0.5f);
    wait(3.0);
    LCD.cls();
    LCD.locate(0,0);
    LCD.printf("Waiting for Bluetooth, Input `1` to start");
    int BTin = 0;
    hm10.baud(9600);
    while(1){
        if(hm10.readable()){ //does square function
            BTin = hm10.getc();
            if (BTin == 1){
                LCD.cls();
                LCD.locate(0,0);
                LCD.printf("Turning 180");
                wait(1.0);
                MDBEnable=1;
                turnAround180(ENCODER_LEFT, ENCODER_RIGHT);
                MDBEnable=0;
                BTin = 0;
            }
        }
    }
}
