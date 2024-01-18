#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <set>

std::set<String> scannedNetworks; // Set to store SSIDs of scanned networks
int totalNetworkCount = 0;
int backgroundColor = TFT_BLACK;
int initialColor = TFT_BLUE; // Initial color to indicate power on
bool isNewNetworkFound = false;
const int ledPin = 19; // LED pin on GPIO 19
const int buzzerPin = 2; // Buzzer pin on GPIO 2

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(4); // Set larger text size
  M5.Lcd.fillScreen(initialColor); // Fill screen with initial color
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextDatum(MC_DATUM); // Center-align text
  M5.Lcd.setRotation(3); // Rotate screen for vertical orientation
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Set the LED pin as an output
  pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as an output
  delay(1000); // Delay to ensure initialization is complete
}

void loop() {
  scanWiFiNetworks(); // Perform a Wi-Fi scan

  if (isNewNetworkFound) {
    backgroundColor = getRandomColor();
    M5.Lcd.fillScreen(backgroundColor);
    flashLED(3); // Flash LED three times
    buzz(); // Buzz the buzzer
    isNewNetworkFound = false;
  }

  displayTotalNetworkCount();
  delay(5000); // Delay between scans
}

void scanWiFiNetworks() {
  int networkCount = WiFi.scanNetworks(); // Count of networks found in the current scan
  for (int i = 0; i < networkCount; ++i) {
    String ssid = WiFi.SSID(i);
    if (scannedNetworks.find(ssid) == scannedNetworks.end()) { // Check if the SSID is new
      scannedNetworks.insert(ssid);
      totalNetworkCount++; // Increment as the network is new
      isNewNetworkFound = true;
    }
  }
}

void displayTotalNetworkCount() {
  M5.Lcd.setTextSize(4); // Set a larger text size for better readability
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setCursor(10, 20);
  M5.Lcd.print("WIFI SEEN");
  int centerX = (M5.Lcd.width() - (6 * 12)) / 2; // Adjust '6 * 12' based on estimated character width
  int centerY = M5.Lcd.height() / 2;
  M5.Lcd.setCursor(centerX, centerY);
  M5.Lcd.printf("%d", totalNetworkCount);
}

int getRandomColor() {
  return random(0xFFFFFF); // Generate a random color (excluding black)
}

void flashLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH); // Turn the LED on
    delay(100); // LED on for 100 milliseconds
    digitalWrite(ledPin, LOW); // Turn the LED off
    delay(100); // LED off for 100 milliseconds
  }
}

void buzz() {
  // Buzz the buzzer
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH); // Turn the buzzer on
    delay(100); // Buzzer on for 100 milliseconds
    digitalWrite(buzzerPin, LOW); // Turn the buzzer off
    delay(100); // Buzzer off for 100 milliseconds
  }
}
