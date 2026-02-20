#include "C12832.h"
#include "mbed.h" 
#include "QEI.h"

#define PWM_FREQUENCY 1000.0f
#define DEFAULT_PWM 0.5f

#define WHEEL_DIAMETER 0.0796f //meters
#define WHEEL_RADIUS (WHEEL_DIAMETER/2.0f) //meters
#define GEAR_RATIO 15 //gearbox ratio of 15:1
#define PI 3.14159265359f


PwmOut PWM_LEFT(PC_9);
PwmOut PWM_RIGHT(PC_8);

DigitalOut MDBEnable(PA_8);
DigitalOut BPE1(PC_0);
DigitalOut BPE2(PC_1);
class Potentiometer                                                     //Begin updated potentiometer class definition
{
private:                                                                //Private data member declaration
    AnalogIn inputSignal;                                               //Declaration of AnalogIn object
    float VDD, currentSampleNorm, currentSampleVolts;                   //Float variable to speficy the value of VDD (3.3 V for the Nucleo-64)

public:                                                                 // Public declarations
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}   //Constructor - user provided pin name assigned to AnalogIn...
                                                                        //VDD is also provided to determine maximum measurable voltage
    float amplitudeVolts(void)                                          //Public member function to measure the amplitude in volts
    {
        return (inputSignal.read()*VDD);                                //Scales the 0.0-1.0 value by VDD to read the input in volts
    }
    float amplitudeNorm(void)                                           //Public member function to measure the normalised amplitude
    {
        return inputSignal.read();                                      //Returns the ADC value normalised to range 0.0 - 1.0
    }
    void sample(void)                                                   //Public member function to read a sample and store the value as data members
    {
        currentSampleNorm = inputSignal.read();                         //Read a sample from the ADC and store normalised representation [0..1]
        currentSampleVolts = currentSampleNorm*VDD;                     //Convert this to a voltage and store that as a data member too.
    }
     float getCurrentSampleNorm(void)                              //Public member function to return the most recent normalised sample [0..1]
    {
        return currentSampleNorm;                                       //Return the most recent normalised sample
    }
     float getCurrentSampleVolts(void)                             //Public member function to return the most recent sampled voltage [0.. 3.3 V]
    {
        return currentSampleVolts;                                      //Return the most recent sampled voltage
    }
};
class SamplingPotentiometer : public Potentiometer{
    private:
    float samplingFrequency, samplingPeriod;
    Ticker sampler;
    void periodicSample() {
        Potentiometer::sample();   // Call base class sample()
    }
    public:
    SamplingPotentiometer(PinName p, float v, float fs): Potentiometer(p,v), samplingFrequency(fs){
        samplingPeriod = 1.0f/samplingFrequency;
        sampler.attach(callback(this, &SamplingPotentiometer::periodicSample), samplingPeriod);
        }
};
int main() {

    C12832 LCD(D11, D13, D12, D7, D10);
    Serial pc(USBTX, USBRX);

    SamplingPotentiometer potLeft(A0, 3.3f, 100.0f);
    SamplingPotentiometer potRight(A1, 3.3f, 100.0f);
    PWM_LEFT.period_us(50);
    PWM_RIGHT.period_us(50);
    PWM_LEFT.write(0.5f);
    PWM_RIGHT.write(0.5f);
    MDBEnable =  1;
    BPE1 = 1;
    BPE2 = 1;

    QEI encoderLeft(PC_10, PC_12, NC, 256);
    QEI encoderRight(PB_13, PB_14, NC, 256);

    while (true) {
        float potValueLeft = 1 - potLeft.getCurrentSampleNorm();
        float potValueRight = 1 - potRight.getCurrentSampleNorm();

        float encoderLeft_current, encoderLeft_prev, encoderRight_current, encoderRight_prev, encoderLeft_tickrate, encoderRight_tickrate, time_curr, time_prev, sampletime, rightwheel_vel, leftwheel_vel, left_RPM, right_RPM;
        encoderRight_current = 0;
        encoderLeft_current = 0;

        PWM_LEFT.write(potValueLeft);
        PWM_RIGHT.write(potValueRight);

        //resetting variables for sample
        Timer time;
        time_curr = 0;
        time_prev = time_curr;

        time.reset();
        time.start();
        encoderLeft.reset();
        encoderRight.reset();

        encoderLeft_prev = encoderLeft_current;
        encoderRight_prev = encoderRight_current;

        wait_ms(30); //sample tim

        time_curr = time.read();
        sampletime = time_curr - time_prev;

        encoderRight_current =  -1 * encoderRight.getPulses(); // multiply by -1 for clockwise to be +ve
        encoderLeft_current = encoderLeft.getPulses();

        encoderLeft_tickrate = (encoderLeft_current - encoderLeft_prev)/sampletime;
        encoderRight_tickrate = (encoderRight_current - encoderRight_prev)/sampletime;

        left_RPM = (encoderLeft_tickrate * 60)/(256 * 15);
        right_RPM = (encoderRight_tickrate * 60)/(256 * 15);

        LCD.locate(0,0);
        LCD.printf("RPM of left wheel is %0.1f", left_RPM);

        LCD.locate(0,8);
        LCD.printf("RPM of right wheel is %0.1f", right_RPM);

        LCD.locate(0,16);
        LCD.printf("Pulses left: %0.1f  right: %0.1f", encoderLeft_current, encoderRight_current);


    }
}