#include <M5Core2.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <FS.h>
#include <SD.h>

TinyGPSPlus gps;
HardwareSerial GPSModule(1);

const int numLines = 7;
char displayBuffer[numLines][100];
char nextBuffer[numLines][100];
File file;
const char *filename = "/wifi_data.csv";
bool scanning = false;

void setup() {
    M5.begin();
    Serial.begin(115200);
    GPSModule.begin(9600, SERIAL_8N1, 13, 14);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 50);
    M5.Lcd.println("WELCOME TO");
    M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("RINGMAST4R'S EMPORIUM");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (!SD.begin()) {
        M5.Lcd.println("SD Card Fail");
        return;
    } else {
        file = SD.open(filename, FILE_WRITE);
        if (file.size() == 0) {
            file.println("SSID,BSSID,SignalStrength,Latitude,Longitude,Time,Date");
        }
        file.close();
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    memset(nextBuffer, 0, sizeof(nextBuffer));
}

void loop() {
    M5.update();
    if (M5.BtnC.wasPressed()) {
        scanning = true;
        clearBuffers(); 
        M5.Lcd.fillScreen(BLACK);
    }

    if (scanning) {
        while (GPSModule.available() > 0) {
            if (gps.encode(GPSModule.read())) {
                displayGPSInfo();
            }
        }

        scanForWiFiNetworks();
        updateDisplay();
        displayCreatorInfo();
    }
}

void displayGPSInfo() {
    sprintf(nextBuffer[0], "Latitude: %.6f", gps.location.lat());
    sprintf(nextBuffer[1], "Longitude: %.6f", gps.location.lng());
    sprintf(nextBuffer[2], "Altitude: %.2f ft", gps.altitude.meters() * 3.281);
    sprintf(nextBuffer[3], "Satellites: %d", gps.satellites.value());
    sprintf(nextBuffer[4], "Speed: %.2f mph", gps.speed.kmph() * 0.621371);

    if (gps.date.isValid() && gps.time.isValid()) {
        int hour = gps.time.hour() - 5; // Adjust for EST
        if (hour < 0) hour += 24;
        bool isPM = hour >= 12;
        if (hour > 12) hour -= 12;
        if (hour == 0) hour = 12;
        sprintf(nextBuffer[5], "%02d/%02d/%04d %02d:%02d:%02d %s",
                gps.date.month(), gps.date.day(), gps.date.year(),
                hour, gps.time.minute(), gps.time.second(),
                isPM ? "PM" : "AM");
    }
}

void scanForWiFiNetworks() {
    int n = WiFi.scanNetworks();
    sprintf(nextBuffer[6], "WiFi Networks: %d", n);

    file = SD.open(filename, FILE_APPEND);
    if (file) {
        for (int i = 0; i < n; ++i) {
            file.printf("%s,%s,%d,%.6f,%.6f,%02d:%02d:%02d,%02d/%02d/%04d\n",
                        WiFi.SSID(i).c_str(),
                        WiFi.BSSIDstr(i).c_str(),
                        WiFi.RSSI(i),
                        gps.location.lat(),
                        gps.location.lng(),
                        gps.time.hour(), gps.time.minute(), gps.time.second(),
                        gps.date.month(), gps.date.day(), gps.date.year());
        }
        file.close();
    }
    WiFi.scanDelete();
}

void updateDisplay() {
    for (int i = 0; i < numLines; i++) {
        if (strcmp(displayBuffer[i], nextBuffer[i]) != 0) {
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setCursor(0, i * 20);
            M5.Lcd.fillRect(0, i * 20, 320, 20, BLACK);
            M5.Lcd.print(nextBuffer[i]);
            strcpy(displayBuffer[i], nextBuffer[i]);
        }
    }
}

void displayCreatorInfo() {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(0, M5.Lcd.height() - 20);
    M5.Lcd.fillRect(0, M5.Lcd.height() - 20, 320, 20, BLACK);
M5.Lcd.print("Hacked by @Ringmast4r");
}

void clearBuffers() {
memset(displayBuffer, 0, sizeof(displayBuffer));
memset(nextBuffer, 0, sizeof(nextBuffer));
for (int i = 0; i < numLines; i++) {
M5.Lcd.fillRect(0, i * 20, 320, 20, BLACK);
}
}
