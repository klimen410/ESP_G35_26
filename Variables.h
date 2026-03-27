#ifndef VARIABLES_H
#define VARIABLES_H

#define PWM_FREQUENCY 1000.0f
#define DEFAULT_PWM 0.5f
#define WHEEL_DISTANCE 0.1775
#define PPR             256              // Pulses Per Revolution

//PWM Settings
#define STOP_PWM       0.5f
#define DRIVE_SPEED    0.677f            
#define TURN_SPEED     0.655f

#define WHEEL_DIAMETER 0.0796f //meters
#define WHEEL_RADIUS (WHEEL_DIAMETER/2.0f) //meters
#define GEAR_RATIO 15 //gearbox ratio of 15:1
#define PI 3.14159265359f   
#define TURN_180_COUNTS   1200

//X4 encoding = 4 edges per pulse. Total counts per one wheel revolution:
#define COUNTS_PER_REV   (PPR * 4 * GEAR_RATIO)
#define STRAIGHT_COUNTS  1225
#define TURN_90_COUNTS   590
#define TURN_70_COUNTS   400
#endif