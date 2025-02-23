# Lab 0: Signal Conditioning and Filtering

**ECE118 - Introduction to Mechatronics**  
**University of California, Santa Cruz**  
**Spring 2024**  

## 📌 Overview

Lab 0 serves as an introduction to the ES_Framework, debugging tools, and hardware components used in ECE118. The lab is divided into multiple sections, covering PCB assembly, basic programming, hardware exploration, and the implementation of finite and hierarchical state machines for the Roach robot.

### 🔹 Objectives:
- Learn **soldering techniques** by assembling a mini-beacon PCB
- Program a **basic "Hello World!"** application on the Roach robot
- Explore the **hardware components** of the Roach robot
- Implement **event detection and handling** using sensors
- Develop **finite state machines (FSM)** and **hierarchical state machines (HSM)** to control robot behavior

## ⚙️ Technologies & Components

- **Microcontroller:** PIC-based Roach robot
- **Software:** MPLAB X IDE, Embedded C, ES_Framework
- **Hardware:** IR sensors, bumpers, motors, LED bar
- **Tools:** Oscilloscope, soldering iron, power management tools

## 📂 Project Structure

```plaintext
📦 ECE118_Lab0
 ┣ 📜 README.md   # Lab documentation
 ┣ 📂 src         # Source code (FSM, event checkers, motor control)
 ┣ 📂 hardware    # Schematics and PCB soldering details
 ┗ 📂 docs        # Reports, diagrams, and references
```

## 🚀 Lab Sections

### 1️⃣ PCB Assembly & Soldering
- Assembled a **mini-beacon PCB**
- Identified and resolved issues such as **cold solder joints**
- Used **leaded vs. unleaded solder** and analyzed differences

### 2️⃣ "Hello World!" on the Roach
- Set up **MPLAB X** for programming the Roach
- Debugged **heap allocation** and serial printing issues
- Verified proper firmware execution

### 3️⃣ Running the Roach Test Harness
- Tested **bumpers, motors, and LED bar**
- Validated sensor readings using a **serial monitor**
- Ensured correct **power and hardware connections**

### 4️⃣ Roach Hardware Exploration
- Developed a **test harness** to control sensors and motors
- Used **bumper inputs** to control motor speeds
- Displayed light sensor values on the **LED bar**

### 5️⃣ Event Detection
- Implemented **event checkers** for **bumpers and light sensors**
- Debugged **sensor noise and event spamming** issues

### 6️⃣ Better Event Detection
- **Debounced** bumper inputs using **bit shifting and circular buffers**
- Applied **hysteresis** to reduce false light sensor triggers
- Implemented a **simple service** to handle sensor polling

### 7️⃣ Finite State Machine (FSM)
- Designed and implemented an **FSM-based robot controller**
- States included **Hiding, Running, Reversing, and Turning**
- Debugged **state transitions and bumper responses**

### 8️⃣ Hierarchical State Machine (HSM)
- Enhanced the FSM by incorporating **nested sub-states**
- Implemented an **infinity-loop search pattern** for darkness
- Used **HSM debugging techniques** to optimize transitions

## 🎯 Key Challenges & Solutions

- **Ghost Events:** Improved wiring and applied signal filtering
- **Motor Control Issues:** Refined **timing and speed adjustments**
- **FSM Debugging:** Used **serial prints and keyboard inputs** for verification
- **HSM Complexity:** Incrementally tested each state before full integration


## 🛠 Team
- **Aleida Diaz-Roque**
- **Andrew Ton That**


## 🔗 References
- [Project Git Repository](https://git.ucsc.edu/adiazroq/ece118_lab0)
- [FSM Demonstration Video](https://youtu.be/qtOb4daB8VE)
- [HSM Demonstration Video](https://youtu.be/aQDWnOwneKg)



