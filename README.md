# University of Manchester ESP Group 35 2025/2026

This Embedded Systems Project (ESP) API help developers to interact with a STM32F401RE Nucleo Platform using Mbed hardware abstraction to provide real-time data to a line-following buggy.
This documentation so far explains how to implement these functions:
- Control motor speed using potentiometers
- Use X4 encoders to track wheel velocity
- Execute basic functions using Bluetooth

To ensure that the API functions as intended, the hardware must be wired to the following pins on the Nucleo-64 pins:
  ### Pin Configuration Summary
| Component | Pin(s) | Function |
| :--- | :--- | :--- |
| PWM_LEFT / RIGHT | PC_9, PC_8 | Controls speed of motor |
| MDBEnable | PA_8 | Enables motor driver |
| BPE1 / BPE2 | PC_0, PC_1 | Motor driver control signal |
| ENCODER_LEFT / RIGHT | PC_10, PC_12, PB_13, PB_14 | Measures left & right wheel rotation |
| potLeft / potRight | A0, A1 | Reads input voltage |
| HM10 (TX / RX) | PA_11, PA_12 | Transmits and receives data from Bluetooth module |
| LCD | D11, D13, D12, D7, D10 | Displays system data |

This repository will have multiple parts to it: 
1. TD1
2. TD2
3. TD3
4. TD4
5. R&D
6. Variables
7. Final Code

# Buggy Component Details
Components present:
1. 6x TCRT5000 sensors
2. STM32F401RE microcontroller
3. 2x encoders

# TD1
Located in TD1 folder, buggy is expected to have:
1. Working PWM
2. Working encoders
3. 90cm length square function

# TD2
Located in TD2 folder, buggy is expected to have:
1. All of TD1 functionalities
2. Working TCRT5000 sensor voltage variation on a stripboard/PCB
3. Interfacing with STM32 with line sensor variation
4. Display of velocity from encoders
5. Bluetooth communication

# TD3
Located in TD3 folder, buggy is expected to have:
1. All of TD2 functionalities
2. 

# TD4
Located in TD4 folder, buggy is expected to have:
1. All of TD3 functionalities
2. 

# R&D
This folder only contains prototypes and acts as a cache if past code is needed to be accessed. This folder should not be used as working code.

# Variables
This file contains all the defines and variables used for the buggy

# Final Code
This folder should be used as the final working code used for the line race competition.
