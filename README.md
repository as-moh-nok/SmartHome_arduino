# SmartHome_arduino
# Smart Home Control System - Arduino Project

This project simulates a **smart home control system** using Arduino, offering features like **temperature control**, **lighting control**, and **security monitoring**. The system includes an LCD menu interface, allowing users to monitor and adjust the home environment through an intuitive interface. The project is meant to be run in the **Arduino IDE** and simulated using **Proteus**.

## Features

### 1. **Menu System**
   - **LCD Menu Display** with the following options:
     1. **Temperature Control**: Switch between automatic and manual modes.
     2. **Lighting Control**: Adjust lighting intensity in three modes.
     3. **Security Check**: Monitor door status (garage and main door).
     4. **Exit**: Return to normal operation.
     
### 2. **Temperature Control**
   - **Automatic Mode**:
     - Cooling activates if the temperature > 25°C.
     - Heating activates if the temperature < 20°C.
   - **Manual Mode**:
     - Manually control the heating or cooling system.

### 3. **Lighting Control**
   - Three lighting modes:
     1. **Rest Mode**: Lights off.
     2. **Romantic Mode**: Half-brightness lighting.
     3. **Normal Mode**: Full-brightness lighting.
   - Uses a **12V lamp** for lighting control.

### 4. **Security System**
   - Monitors two doors (garage and main entrance).
   - An **LED** indicates door status:
     - Blinks if the door is open.
     - Remains on if the door is open for more than 5 seconds.
     - Turns off if both doors are closed.
   - Status is displayed in the **Security Check** section of the menu.

## Installation

### Arduino IDE
To run this project, you'll need to use the **Arduino IDE**.You can use platfomrIO too if you can create
hex file from it.(you need some action for it)

**Install the LiquidCrystal Library**:
   - Go to **Sketch > Include Library > Manage Libraries**.
   - Search for "**LiquidCrystal**" and install it.

### Proteus Simulation
You can simulate the system in **Proteus** by following these steps:
note: I attach shematic image in this project.

1. Add the required components:
   - Arduino Uno
   - LCD display
   - Temperature sensor
   - Heating/cooling motors
   - Push buttons for menu and door sensors
   - 12V lamp for lighting
   - Security system with LED

2. Wire the components according to the schematic.

3. Run the simulation to verify temperature control, lighting control, and security monitoring functionality.

## Notes for Running the System

- The system defaults to **automatic control** when not in the menu.
- Access the menu using the **Menu Button** and navigate using the **OK Button**.
- Ensure all components are correctly connected in **Proteus** for accurate simulation.
- Temperature and security control are paused while in the menu, resuming once you exit.

## Conclusion

This project demonstrates a smart home system with real-time monitoring and control of temperature, lighting, and security. You can easily simulate this in **Proteus** and upload the code to an **Arduino Uno** using the Arduino IDE for a working prototype.
