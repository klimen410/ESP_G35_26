//This code use for in slope or resistance increase, the speed don't change
class SpeedController {
private:
    float Kp;               // Proportional gain
    float Ki;               // Integral gain
    float targetSpeed;      // Desired setpoint (RPM)
    float integralError;    // Accumulated error over time (Integral term)
    float currentPWM;       // Current calculated PWM duty cycle (0.0 to 1.0)

public:
    SpeedController(float p, float i) : Kp(p), Ki(i), targetSpeed(0), integralError(0), currentPWM(0.5f) {}

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