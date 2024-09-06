#include <Arduino.h>
#include <LiquidCrystal.h>

// Security system variables:
int parking_door = 11;      // Parking door sensor pin
int entery_door = 12;       // Entry door sensor pin
int securityLED = 13;       // LED for security alerts
unsigned long previousTime; // Variable to store previous time for timing security checks
unsigned long currentTime;  // Variable to store current time
unsigned long Time;         // Time elapsed between security alerts

// Menu system variables:
int menu_btn = 0;           // Menu button pin
int OK_btn = 1;             // OK button pin
bool menuActive = false;    // Flag to track whether the menu is active

// Temperature control variables:
int coldmotor = 2;          // Cooling motor control pin
int heatmotor = 3;          // Heating motor control pin
int heat_btn = A2;          // Button for manual heat control
int cold_btn = A3;          // Button for manual cold control
float currentTemp = 0;      // Variable to store the current temperature
bool autoMode = true;       // Flag to track whether the system is in auto mode
int tempSensor = A1;        // Temperature sensor pin

// Lighting control variables:
int lightMode = 2;          // Lighting mode (2 = normal, 1 = romantic, 0 = rest)
const int lightPin = A4;    // Pin for lighting control (PWM)

// LCD display object
LiquidCrystal lcd(10, 9, 8, 7, 6, 5);  // Define pins for LCD display

// Function to debounce button presses (eliminate false readings)
bool debounce(int pin) {
  static unsigned long lastDebounceTime = 0;  // Track time of last button press
  unsigned long debounceDelay = 50;  // Debounce delay in milliseconds
  bool currentState = digitalRead(pin);  // Read the current state of the button
  
  // If button is pressed and debounce delay has passed, confirm the press
  if (currentState == LOW && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    return true;
  }
  return false;
}

void setup() {
  // Initialize menu buttons as input with pull-up resistors
  pinMode(menu_btn, INPUT_PULLUP);
  pinMode(OK_btn, INPUT_PULLUP);

  // Initialize the LCD display (16 columns, 4 rows)
  lcd.begin(16, 4);

  // Initialize security system pins
  pinMode(parking_door, INPUT_PULLUP);
  pinMode(entery_door, INPUT_PULLUP);
  pinMode(securityLED, OUTPUT);

  // Initialize temperature control pins
  pinMode(coldmotor, OUTPUT);
  pinMode(heatmotor, OUTPUT);
  pinMode(cold_btn, INPUT_PULLUP);
  pinMode(heat_btn, INPUT_PULLUP);
  pinMode(tempSensor, INPUT_PULLUP);

  // Calculate and display the initial temperature
  currentTemp = calculateTemp();

  // Update the LCD display with initial information
  updateDisplay();
}

void loop() {
  // Check if the menu button is pressed to activate the menu
  if (debounce(menu_btn)) {
    menuActive = !menuActive;
  }

  // If the menu is active, enter the menu; otherwise, continue system operations
  if (menuActive) {
    entryMenu();  // Enter the menu system
    menuActive = false;
  } else {
    // Update display and control temperature, lighting, and security
    updateDisplay();
    controlTemperature();
    controlLighting();
    controlSecurity();
  }
}

// Function to update the LCD display with current system info
void updateDisplay() {
  lcd.clear();  // Clear the LCD screen
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");      // Display temperature
  lcd.print(currentTemp);

  lcd.setCursor(0, 1);
  if (autoMode) {
    lcd.print("Auto Mode");  // Display auto mode if active
  } else {
    lcd.print("Manual Mode");  // Display manual mode if active
  }

  lcd.setCursor(0, 2);
  // Display the current lighting mode
  if (lightMode == 0) {
    lcd.print("LED: Rest");
  } else if (lightMode == 1) {
    lcd.print("LED: Romantic");
  } else if (lightMode == 2) {
    lcd.print("LED: Normal");
  }
}

// Function to handle the menu system
void entryMenu() {
  // Turn off all systems during menu navigation
  digitalWrite(heatmotor, LOW);
  digitalWrite(coldmotor, LOW);
  digitalWrite(securityLED, LOW);

  int selectedOption = 0;  // Track the selected menu option
  lcd.clear();
  while (true) {
    // Display menu options
    lcd.setCursor(0, 0);
    lcd.print("1.Temp ");
    lcd.setCursor(0, 1);
    lcd.print("2.Light ");
    lcd.setCursor(0, 2);
    lcd.print("3.Security ");
    lcd.setCursor(0, 3);
    lcd.print("4.Exit");
    lcd.setCursor(0, selectedOption);
    lcd.print(">");  // Highlight the current selection

    // Navigate the menu using the menu button
    if (debounce(menu_btn)) {
      selectedOption = (selectedOption + 1) % 4;
    }

    // Confirm the selection using the OK button
    if (debounce(OK_btn)) {
      if (selectedOption == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("tempSetting");  // Enter temperature setting
        tempSetting();
      } else if (selectedOption == 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("lighting Setting");  // Enter lighting setting
        lightingSetting();
      } else if (selectedOption == 2) {
        lcd.clear();
        checkSecurity();  // Enter security check
      } else if (selectedOption == 3) {
        lcd.print("Exit ");
        return;  // Exit the menu
      }
    }
  }
}

// Function to control the security system
void controlSecurity() {
  // If either door is open, start the security alert process
  if (digitalRead(parking_door) == HIGH || digitalRead(entery_door) == HIGH) {
    blink200();  // Flash the LED
    previousTime = millis();  // Track the time of the security breach
    while (true) {
      currentTime = millis();
      Time = currentTime - previousTime;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(Time);  // Display the time since the breach
      if (Time < 2000 && digitalRead(parking_door) == LOW && digitalRead(entery_door) == LOW) {
        digitalWrite(securityLED, LOW);  // Turn off the LED if doors are closed within 2 seconds
        break;
      }
      if (Time > 2000) {
        digitalWrite(securityLED, HIGH);  // Keep the LED on if time exceeds 2 seconds
        delay(500);
        break;
      }
      blink200();  // Flash the LED continuously

      if (debounce(menu_btn)) {
        return;  // Exit if menu button is pressed
      }
    }
  }
}

// Function to check security status in the menu
void checkSecurity() {
  int selectedOption = 0;
  lcd.clear();
  while (true) {
    // Display door statuses
    lcd.setCursor(0, 0);
    if (digitalRead(parking_door) == HIGH) {
      lcd.print("*Parking open");
    } else {
      lcd.print("*Parking close");
    }

    lcd.setCursor(0, 1);
    if (digitalRead(entery_door) == HIGH) {
      lcd.print("*Entry open");
    } else {
      lcd.print("*Entry close");
    }

    lcd.setCursor(0, 2);
    lcd.print("*Exit ");
    lcd.setCursor(0, selectedOption);
    lcd.print(">");

    // Navigate options
    if (debounce(menu_btn)) {
      selectedOption = (selectedOption + 1) % 3;
    }

    // Exit security check when "Exit" is selected
    if (debounce(OK_btn)) {
      if (selectedOption == 2) {
        return;
      }
    }
  }
}

// Function to blink the security LED with 200ms delay
void blink200() {
  digitalWrite(securityLED, HIGH);
  delay(200);
  digitalWrite(securityLED, LOW);
  delay(200);
}

// Function to control temperature based on auto or manual mode
void controlTemperature() {
  currentTemp = calculateTemp();  // Get the current temperature

  if (autoMode) {
    automotor();  // Control temperature automatically
  } else {
    manual();  // Control temperature manually
  }
}

// Function for manual temperature control
void manual() {
  // Turn on cooling or heating based on button presses
  if (debounce(cold_btn)) {
    digitalWrite(coldmotor, HIGH);
    digitalWrite(heatmotor, LOW);
  } else if (debounce(heat_btn)) {
    digitalWrite(heatmotor, HIGH);
    digitalWrite(coldmotor, LOW);
  } else {
    // Turn off both cooling and heating if


    // no button is pressed
    digitalWrite(coldmotor, LOW);
    digitalWrite(heatmotor, LOW);
  }
}

// Function for automatic temperature control
void automotor() {
  currentTemp = calculateTemp();  // Recalculate the current temperature

  // Control cooling or heating based on temperature thresholds
  if (currentTemp > 25) {
    digitalWrite(coldmotor, HIGH);  // Turn on cooling if temp > 25°C
    digitalWrite(heatmotor, LOW);   // Turn off heating
  } else if (currentTemp < 20) {
    digitalWrite(heatmotor, HIGH);  // Turn on heating if temp < 20°C
    digitalWrite(coldmotor, LOW);   // Turn off cooling
  } else {
    // Turn off both cooling and heating if temp is between 20°C and 25°C
    digitalWrite(coldmotor, LOW);
    digitalWrite(heatmotor, LOW);
  }
}

// Function to configure temperature settings in the menu
void tempSetting() {
  int selectedOption = 0;
  lcd.clear();
  while (true) {
    lcd.setCursor(0, 0);
    if (autoMode) {
      lcd.print("*Automode [on]");  // Show auto mode status
    } else {
      lcd.print("*Automode [off]"); // Show manual mode status
    }
    lcd.setCursor(0, 1);
    lcd.print("*Exit");
    lcd.setCursor(0, selectedOption);
    lcd.print(">");  // Highlight the current selection

    // Navigate options
    if (debounce(menu_btn)) {
      selectedOption = (selectedOption + 1) % 2;
    }

    // Toggle auto mode or exit
    if (debounce(OK_btn)) {
      if (selectedOption == 0) {
        autoMode = !autoMode;  // Toggle auto mode
      } else if (selectedOption == 1) {
        return;  // Exit the temperature setting menu
      }
    }
  }
}

// Function to configure lighting settings in the menu
void lightingSetting() {
  int selectedLightingMode = 0;
  lcd.clear();
  while (true) {
    // Display lighting modes
    lcd.setCursor(0, 0);
    lcd.print("1. Rest");
    lcd.setCursor(0, 1);
    lcd.print("2. Romantic");
    lcd.setCursor(0, 2);
    lcd.print("3. Normal");
    lcd.setCursor(0, 3);
    lcd.print("4. Exit");
    lcd.setCursor(0, selectedLightingMode);
    lcd.print(">");  // Highlight the current selection

    // Navigate options
    if (debounce(menu_btn)) {
      selectedLightingMode = (selectedLightingMode + 1) % 4;
    }

    // Set lighting mode or exit
    if (debounce(OK_btn)) {
      if (selectedLightingMode < 4) {
        lightMode = selectedLightingMode;  // Set the selected lighting mode
        break;
      } else {
        return;  // Exit the lighting setting menu
      }
    }
  }
}

// Function to control lighting based on the selected mode
void controlLighting() {
  if (lightMode == 0) {
    analogWrite(lightPin, 0);      // Rest mode: light off
  } else if (lightMode == 1) {
    analogWrite(lightPin, 128);    // Romantic mode: half brightness
  } else if (lightMode == 2) {
    analogWrite(lightPin, 255);    // Normal mode: full brightness
  }
}

// Function to calculate the current temperature
int calculateTemp() {
  int t = analogRead(tempSensor);  // Read the temperature sensor value
  float temp = t * (5.0 / 1024.0); // Convert to voltage (5V reference, 10-bit ADC)
  temp = temp * 100;               // Convert voltage to temperature in °C
  return temp;                     // Return the calculated temperature
}
