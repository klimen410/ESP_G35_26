<<<<<<< HEAD
#include "mbed.h"
#include "C12832.h"
=======
#include "mbed.h" //test
>>>>>>> origin/main

#define PWM_FREQUENCY 1000.0f
#define DEFAULT_PWM 0.5f

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
    C12832 lcd(D11, D13, D12, D7, D10); //LCD display
    SamplingPotentiometer potX(A0, 3.3f, 100.0f);
    PWM_LEFT.period_us(50);
    PWM_RIGHT.period_us(50);
    PWM_LEFT.write(0.5f);
    PWM_RIGHT.write(0.5f);
    MDBEnable =  1;
    BPE1 = 1;
    BPE2 = 1;
    while (true) {
        float potValue = 1 - potX.getCurrentSampleNorm();
        PWM_LEFT.write(potValue);
        PWM_RIGHT.write(potValue);
    }
}