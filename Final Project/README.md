# JAK-HD: Autonomous Ball-Handling Robot  

**ECE118 - Introduction to Mechatronics**  
**University of California, Santa Cruz**  
**Spring 2024**  

## 📌 Project Overview  

JAK-HD is an autonomous robot designed and built for the ECE118 final project. The robot competed in a game where it had to navigate a 4’x8’ playing field, collect chrome balls, and dispense them into the opponent’s field. The project required integrating **electronics, software, and mechanical design** to create a reliable and competitive mechatronic system.  

### 🔹 Core Features:  
- **Autonomous navigation** using IR sensors and bump detectors  
- **Ball collection mechanism** with a motorized roller  
- **Storage and dispensing system** with a ramp and actuator-controlled door  
- **State machine-based software architecture** for efficient task execution  
- **Collision handling** with limit switches and adaptive movement strategies  

## ⚙️ Technologies & Components  

- **Microcontroller:** Uno32  
- **Motors:** DC brushless motors, linear actuator  
- **Sensors:** IR tape sensors, track wire detector, bump switches  
- **Software:** Embedded C, state machines (HSM & SSM)  
- **Mechanical Design:** MDF chassis, laser-cut assembly, 3D-printed roller mounts  

## 📂 Project Structure  

```plaintext
📦 JAK-HD Final Project 
 ┣ 📜 README.md   # Project documentation  
 ┣ 📂 src         # Source code (motor control, sensors, state machines)  
 ┣ 📂 hardware    # Schematics and wiring diagrams  
 ┣ 📂 cad         # Mechanical design files (SolidWorks, laser-cut templates)  
 ┗ 📂 docs        # Reports, diagrams, and reference materials  
```

## 🚀 How It Works  

1. **Search & Collect** – The robot follows a predefined search pattern to collect balls.  
2. **Navigate & Align** – It uses IR sensors to follow field markings and avoid obstacles.  
3. **Deposit Balls** – Upon reaching the trap door, it activates the actuator and roller to dispense balls.  
4. **Wall Follow Strategy** – The robot optimizes collection efficiency by tracking balls along the field’s walls.  

## 🔧 Challenges & Solutions  

- **Ghost Events:** Addressed by improving wiring connections and filtering noisy sensor signals.  
- **Actuator Speed:** Optimized placement and timing to minimize delay.  
- **Wheel Traction Issues:** Switched to harder wheels with collar locks for better torque.  
- **Power Distribution Problems:** Redesigned wiring and added fuses for stability.  

## 🎯 Performance  

- Achieved **37/40 ball collection** in successful trials.  
- Competed in the **UCSC Mechatronics Tournament**, reaching the **semifinals**.  

## 📸 Images

<img width="400" alt="Competition during first round" src="https://github.com/user-attachments/assets/f69a20f5-ae45-4882-8d0f-2d9c8a6c8e0a" />

<img width="400" alt="Competition, passing to semi-finals" src="https://github.com/user-attachments/assets/40e22f5f-b905-4c0c-b5df-0c3bd698eb97" />

<img width="300" alt="Robot and Aleida" src="https://github.com/user-attachments/assets/3e2e7f4d-d3f6-4ec0-9d0a-ecb4c06a5751" />

<img width="500" alt="Robot and Team" src="https://github.com/user-attachments/assets/80264bbf-a35a-468b-9ed4-0584ad987ea8" />


## 🛠 Team  

- **Aleida Diaz-Roque** 
- **Jovanni Shen**   
- **Karan Humpal**  

## 📜 Conclusion  

JAK-HD successfully demonstrated the integration of electronics, mechanics, and software in an autonomous system. The project provided hands-on experience in **state machine programming, embedded systems, and real-world problem-solving** in mechatronics.  
