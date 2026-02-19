#include "mbed.h"
#include "QEI.h"
#include "C12832.h"

#define WHEEL_DIAMETER 0.0796f //meters
#define WHEEL_RADIUS WHEEL_DIAMETER/2.0f //meters
#define GEAR_RATIO 15 //gearbox ratio of 15:1
#define PI 3.14159265359f

class Encoder{
    //test 3
    //translational velocity = (rigthVel + leftVel) /2
    //angular velocity =  (rightvVel - leftVel) / distance between wheels
    private:

    QEI wheel;
    //InterruptIn channelA, channelB;
    int PPR, pulses;
    float totalTicks, time, tickrate, rpm, velocity;
    Timer sampleTime;

    public:

        Encoder(PinName channelA, PinName channelB, PinName index, int pulsesPerRev) : wheel(channelA, channelB, index, pulsesPerRev, QEI::X4_ENCODING), PPR(pulsesPerRev) {
            sampleTime.start();
        }

        float getTickRate(void){
            wheel.reset(); //reset pulse counter 
            pulses = wheel.getPulses(); 

            time = sampleTime.read(); //get sample time 
            sampleTime.reset(); //reset timer for next sample

            tickrate = (totalTicks/time);

            return tickrate;
        }

        float getRPM(){

            return (getTickRate()/(PPR*4)) * 60.0f; //PPR * 4 gives counts per revolution
        }

        float getWheelVelocity(void){
            velocity = (getTickRate())/((PPR*4) * GEAR_RATIO) * PI * WHEEL_DIAMETER;

            return velocity;
        }

};

int main(){
    C12832 LCD(D11, D13, D12, D7, D10);
    Encoder leftWheel(PA_13, PA_14, NC, 256);
    Encoder rightWheel(PA_12, PA_11, NC, 256);

    LCD.cls();

    while (true){
        leftWheel.getTickRate();
        rightWheel.getTickRate();

        wait_ms(100);
        LCD.locate(5,10);
        LCD.printf("RPM of left wheel is %0.1f\n", leftWheel.getRPM());
        LCD.printf("RPM of right wheel is %0.1f\n", rightWheel.getRPM());
    }
}