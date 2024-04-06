#include <Servo.h>

Servo servo1;
int servoPin = 13;
int currentTemperature = -1; // Track the current temperature
int currentWindDirection = -1; // Initialize with an invalid wind direction

void setup() {
  servo1.attach(servoPin);
  Serial.begin(9600); // Match the baud rate to the Python script
  servo1.write(92); // Explicitly stop the servo upon setup

  // Initialize digital pins 2 through 8 as outputs for LEDs.
  for (int pin = 2; pin <= 11; pin++) {
    pinMode(pin, OUTPUT);
  }
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');

    // Debugging: Print the received data
    Serial.println(data);

    // Parse for temperature
    int tempIndex = data.indexOf('T');
    int dirIndex = data.indexOf('D');
    if (tempIndex != -1 && dirIndex != -1) {
      String tempStr = data.substring(tempIndex + 1, dirIndex - 1);
      int temperature = tempStr.toInt();
      
      // Update LEDs if the temperature has changed
      if (currentTemperature != temperature) {
        updateLEDs(temperature); // Update LEDs for the new temperature
        currentTemperature = temperature; // Update tracking
      }
      
      // Parse for wind direction
      String dirStr = data.substring(dirIndex + 1);
      int windDirection = dirStr.toInt();

      // Move the servo if the wind direction has changed
      if (currentWindDirection != windDirection) {
        pointToWindDirection(windDirection); // Move servo for new wind direction
        currentWindDirection = windDirection; // Update tracking
      }
    }
  }
}
void updateLEDs(int newTemperature) {
    static int lastLedsLit = 0; // Tracks how many LEDs were lit during the last update
    const int baseTemperature = 0; // Base temperature for calibration

    // No LEDs should light up if the temperature is below 0 degrees
    if (newTemperature < 0) {
        for (int pin = 2; pin <= 11; pin++) {
            digitalWrite(pin, LOW); // Turn off all LEDs
        }
        lastLedsLit = 0; // Reset the last LEDs lit count
        return; // Exit the function early
    }

    // Round the temperature to the nearest 2-degree mark
    int roundedTemperature = ((newTemperature + 1) / 2) * 2; // Adding 1 for proper rounding down or up
    int temperatureDifference = roundedTemperature - baseTemperature;

    // Calculate the number of LEDs to light based on the 2-degree difference per LED
    // Starting with at least 1 LED for temperatures >= 0
    int ledsToLight = max(0, temperatureDifference / 2) + (roundedTemperature >= 0 ? 1 : 0);
    ledsToLight = min(ledsToLight, 10); // Assuming 10 LEDs, adjust according to your setup

    // Light up or turn off LEDs based on the rounded temperature
    if (roundedTemperature >= baseTemperature + (lastLedsLit - 1) * 2) {
        for (int pin = 2; pin < 2 + ledsToLight; pin++) {
            digitalWrite(pin, HIGH);
            delay(100); // Visual effect
        }
    } else if (roundedTemperature < baseTemperature + (lastLedsLit - 1) * 2) {
        for (int pin = 2 + lastLedsLit; pin > 2 + ledsToLight; pin--) {
            digitalWrite(pin, LOW);
            delay(100); // Visual effect
        }
    }

    // Turn off any LEDs beyond the current level
    for (int pin = 2 + ledsToLight; pin <= 11; pin++) {
        digitalWrite(pin, LOW);
    }

    // Update the lastLedsLit for the next iteration based on the new, rounded temperature
    lastLedsLit = ledsToLight;
}






void pointToWindDirection(int windDirection) {
    float rotationFraction = windDirection / 360.0;
    int rotationTimeToDirection = 1280 * rotationFraction;

    // Rotate to simulate pointing in the wind direction
    servo1.write(0); // Start rotation
    delay(rotationTimeToDirection); // Delay for calculated duration
    servo1.write(92); // Stop the servo

    delay(1000); // Wait for a brief moment

    // Slightly reduce the time for return rotation to prevent overshooting
    int adjustmentValue = 7; // Start with a small adjustment, increase if necessary
    int rotationTimeToReturn = rotationTimeToDirection - adjustmentValue;

    // Rotate back to the original position ("North")
    servo1.write(180); // Reverse rotation direction
    delay(rotationTimeToReturn); // Adjusted delay for return rotation
    servo1.write(92); // Stop the servo, aiming for a more precise stop at the starting position
}




