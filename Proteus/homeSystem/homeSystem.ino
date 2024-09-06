#include <Arduino.h>
#include <LiquidCrystal.h>

// Security:
int parking_door = 11;
int entery_door = 12;
int securityLED = 13;
unsigned long previousTime;
unsigned long currentTime;
unsigned long Time;

// Menu:
int menu_btn = 0;
int OK_btn = 1;
bool menuActive = false;

// Temp:
int coldmotor = 2;
int heatmotor = 3;
int heat_btn = A2;
int cold_btn = A3;
float currentTemp = 0;
bool autoMode = true;
int tempSensor = A1;

// Lighting control variables
int lightMode = 2; //  2 = normal, 1 = romantic, 0 = rest
const int lightPin = A4;

// LCD display
LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

// Debounce function
bool debounce(int pin) {
  static unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 50;  // Debounce delay in milliseconds
  bool currentState = digitalRead(pin);
  
  if (currentState == LOW && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    return true;
  }
  return false;
}

void setup() {
  // Menu:
  pinMode(menu_btn, INPUT_PULLUP);
  pinMode(OK_btn, INPUT_PULLUP);

  lcd.begin(16, 4);

  // Security:
  pinMode(parking_door, INPUT_PULLUP);
  pinMode(entery_door, INPUT_PULLUP);
  pinMode(securityLED, OUTPUT);

  // Temp:
  pinMode(coldmotor, OUTPUT);
  pinMode(heatmotor, OUTPUT);
  pinMode(cold_btn, INPUT_PULLUP);
  pinMode(heat_btn, INPUT_PULLUP);
  pinMode(tempSensor, INPUT_PULLUP);

  // Calculate temp:
  currentTemp = calculateTemp();

  updateDisplay();
}

void loop() {
  if (debounce(menu_btn)) {
    menuActive = !menuActive;
  }

  if (menuActive) {
    entryMenu();
    menuActive = false;
  } else {
    updateDisplay();
    controlTemperature();
    controlLighting();
    controlSecurity();
  }
}

void updateDisplay() {
  lcd.clear();  // Clean
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(currentTemp);

  lcd.setCursor(0, 1);
  if (autoMode) {
    lcd.print("Auto Mode");
  } else {
    lcd.print("Manual Mode");
  }

  lcd.setCursor(0, 2);
  if (lightMode == 0) {
    lcd.print("LED: Rest");
  } else if (lightMode == 1) {
    lcd.print("LED: Romantic");
  } else if (lightMode == 2) {
    lcd.print("LED: Normal");
  }
}

void entryMenu() {
  // Turn off all
  digitalWrite(heatmotor, LOW);
  digitalWrite(coldmotor, LOW);
  digitalWrite(securityLED, LOW);

  int selectedOption = 0;
  lcd.clear();
  while (true) {
    lcd.setCursor(0, 0);
    lcd.print("1.Temp ");
    lcd.setCursor(0, 1);
    lcd.print("2.Light ");
    lcd.setCursor(0, 2);
    lcd.print("3.Security ");
    lcd.setCursor(0, 3);
    lcd.print("4.Exit");
    lcd.setCursor(0, selectedOption);
    lcd.print(">");

    if (debounce(menu_btn)) {
      selectedOption = (selectedOption + 1) % 4;
    }

    if (debounce(OK_btn)) {
      if (selectedOption == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("tempSetting");
        tempSetting();
      } else if (selectedOption == 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("lighting Setting");
        lightingSetting();
      } else if (selectedOption == 2) {
        lcd.clear();
        checkSecurity();
      } else if (selectedOption == 3) {
        lcd.print("Exit ");
        return;
      }
    }
  }
}

void controlSecurity() {
  if (digitalRead(parking_door) == HIGH || digitalRead(entery_door) == HIGH) {
    blink200();
    previousTime = millis();
    while (true) {
      currentTime = millis();
      Time = currentTime - previousTime;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(Time);
      if (Time < 2000 && digitalRead(parking_door) == LOW && digitalRead(entery_door) == LOW) {
        digitalWrite(securityLED, LOW);
        break;
      }
      if (Time > 2000) {
        digitalWrite(securityLED, HIGH);
        delay(500);
        break;
      }
      blink200();

      if (debounce(menu_btn)) {
        return;
      }
    }
  }
}

void checkSecurity() {
  int selectedOption = 0;
  lcd.clear();
  while (true) {
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

    if (debounce(menu_btn)) {
      selectedOption = (selectedOption + 1) % 3;
    }

    if (debounce(OK_btn)) {
      if (selectedOption == 2) {
        return;
      }
    }
  }
}

void blink200() {
  digitalWrite(securityLED, HIGH);
  delay(200);
  digitalWrite(securityLED, LOW);
  delay(200);
}

void controlTemperature() {
  currentTemp = calculateTemp();

  if (autoMode) {
    automotor();
  } else {
    manual();
  }
}

void manual() {
  if (debounce(cold_btn)) {
    digitalWrite(coldmotor, HIGH);
    digitalWrite(heatmotor, LOW);
  } else if (debounce(heat_btn)) {
    digitalWrite(heatmotor, HIGH);
    digitalWrite(coldmotor, LOW);
  } else {
    digitalWrite(coldmotor, LOW);
    digitalWrite(heatmotor, LOW);
  }
}

void automotor() {
  currentTemp = calculateTemp();

  if (currentTemp > 25) {
    digitalWrite(coldmotor, HIGH);
    digitalWrite(heatmotor, LOW);
  } else if (currentTemp < 20) {
    digitalWrite(heatmotor, HIGH);
    digitalWrite(coldmotor, LOW);
  } else {
    digitalWrite(coldmotor, LOW);
    digitalWrite(heatmotor, LOW);
  }
}

void tempSetting() {
  int selectedOption = 0;
  lcd.clear();
  while (true) {
    lcd.setCursor(0, 0);
    if (autoMode) {
      lcd.print("*Automode [on]");
    } else {
      lcd.print("*Automode [off]");
    }
    lcd.setCursor(0, 1);
    lcd.print("*Exit");
    lcd.setCursor(0, selectedOption);
    lcd.print(">");

    if (debounce(menu_btn)) {
      selectedOption = (selectedOption + 1) % 2;
    }

    if (debounce(OK_btn)) {
      if (selectedOption == 0) {
        autoMode = !autoMode;
      } else if (selectedOption == 1) {
        return;
      }
    }
  }
}

void lightingSetting() {
  int selectedLightingMode = 0;
  lcd.clear();
  while (true) {
    lcd.setCursor(0, 0);
    lcd.print("1. Rest");
    lcd.setCursor(0, 1);
    lcd.print("2. Romantic");
    lcd.setCursor(0, 2);
    lcd.print("3. Normal");
    lcd.setCursor(0, 3);
    lcd.print("4. Exit");
    lcd.setCursor(0, selectedLightingMode);
    lcd.print(">");

    if (debounce(menu_btn)) {
      selectedLightingMode = (selectedLightingMode + 1) % 4;
    }

    if (debounce(OK_btn)) {
      if (selectedLightingMode < 4) {
        lightMode = selectedLightingMode;
        break;
      } else {
        return;
      }
    }
  }
}

void controlLighting() {
  if (lightMode == 0) {
    analogWrite(lightPin, 0);
  } else if (lightMode == 1) {
    analogWrite(lightPin, 128);  // Romantic mode: half brightness
  } else if (lightMode == 2) {
    analogWrite(lightPin, 255);  // Normal mode: full brightness
  }
}

int calculateTemp() {
  int t = analogRead(tempSensor);
  float temp = t * (5.0 / 1024.0);
  temp = temp * 100;
  return temp;
}
