# Project 3: Motor Control

**ECE118 - Introduction to Mechatronics**  

**University of California, Santa Cruz**  

**Spring 2024**  

## 📌 Overview

Project 3 explores the control and operation of various types of motors, including DC motors, servos, and stepper motors. The Project covers topics such as unidirectional and bidirectional DC motor control, dealing with inductive kickback, and different drive modes for stepper motors.

### 🔹 Objectives:

- Control an **RC servo** using a potentiometer.
- Implement **unidirectional and bidirectional control** of a DC motor.
- Understand and mitigate **inductive kickback** in DC motors.
- Control a **stepper motor** using different drive modes (full-step, half-step, wave drive).
- Interface with a **dedicated stepper motor driver board**.

## ⚙️ Technologies & Components

- **Microcontroller:** Uno32
- **Motor Drivers:** DS3658, H-bridge
- **Motors:** RC servo, DC motor, stepper motor
- **Components:** Diodes (normal and Zener), potentiometer, LEDs, switch
- **Tools:** Oscilloscope, breadboard, multimeter

## 📂 Project Structure
```plaintext
📦 ECE118_Lab3
 ┣ 📜 README.md      # Lab documentation
 ┣ 📂 src           # Source code for motor control
 ┣ 📂 schematics   # Circuit diagrams in PDF format
 ┣ 📂 reports       # Lab report in PDF format
 ┗ 📂 datasheets   # Datasheets for key components
```

## 🚀 Project Sections

### 1️⃣ RC Servo Control

- Controlled an RC servo using a potentiometer to determine the pulse time.
- Implemented an **exponential moving average (EMA) filter** to reduce noise from the potentiometer readings.
- Scaled the potentiometer input to control the servo position and LEDs.

### 2️⃣ Unidirectional DC Motor Control

- Controlled the speed of a DC motor in one direction using a potentiometer.
- Used **PWM (Pulse Width Modulation)** to vary the duty cycle and control the motor speed.
- Implemented EMA filtering for the potentiometer and used LEDs to indicate motor speed.

### 3️⃣ Inductive Kickback Mitigation

- Investigated the effects of **inductive kickback** when switching DC motors.
- Implemented **snubbing circuits** using diodes (both normal and Zener) to protect the motor driver from voltage spikes.
- Observed the impact of the snubbing circuits on the motor's waveform under resistive torque.

### 4️⃣ Bidirectional DC Motor Control

- Controlled the direction of rotation of a DC motor using a switch.
- Used an **H-bridge** to control the direction of current flow through the motor.
- Implemented LED indicators for the direction of rotation and motor speed.

### 5️⃣ Stepper Motor Control with H-Bridge

- Controlled a stepper motor using an H-bridge.
- Implemented different drive modes: **full-step, half-step, and wave drive**.
- Developed **state machines** to control the sequencing of coil activation for each drive mode.
- Determined the **maximum stepping rate** for each drive mode before losing steps.

### 6️⃣ Stepper Motor Control with Dedicated Driver

- Used a **dedicated stepper motor driver board (DRV8811)** for more precise control.
- Generated step signals using a **two-state state machine**.
- Compared the **maximum stepping rates** achieved with the dedicated driver board to those using the H-bridge.

## 🎯 Key Challenges & Solutions

- **Noisy Potentiometer Readings:** Implemented an EMA filter to smooth the input signal.
- **Inductive Kickback:** Added snubbing circuits with diodes to protect the motor driver.
- **Stepper Motor Control:** Developed state machines to manage the coil activation sequences for different drive modes.
- **Determining Max Stepping Rate:** Iteratively tested and observed the motor's behavior to find the maximum rate without losing steps.

## 🛠 Team

- **Aleida Diaz-Roque**
- **Zachary Bang**

