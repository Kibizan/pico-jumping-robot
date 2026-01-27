/**
 * PICO JUMPING ROBOT - SENSOR TEST
 * Reads acceleration data from ADXL345 and displays it on SH1106 OLED.
 * * HARDWARE CONNECTIONS:
 * - SDA -> GP0 (Physical Pin 1)
 * - SCL -> GP1 (Physical Pin 2)
 * - VCC -> 3.3V
 * - GND -> GND
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <U8g2lib.h>

// --- HARDWARE CONFIGURATION ---
// Using I2C0 on Pico W (GP0/GP1)
#define PIN_SDA 0
#define PIN_SCL 1

// --- GLOBAL OBJECTS ---
// 1. IMU Object (ID = 12345)
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// 2. Display Object
// Driver for SH1106 1.3" OLED. 
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
    Serial.begin(115200);
    // Safety delay to allow USB to initialize
    delay(2000);
    Serial.println("--- SYSTEM BOOT ---");

    // 1. Initialize I2C Bus on specific pins
    Wire.setSDA(PIN_SDA);
    Wire.setSCL(PIN_SCL);
    Wire.begin();

    // 2. Initialize Display
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr); // Readable bold font
    u8g2.drawStr(0, 10, "Booting Sensors...");
    u8g2.sendBuffer();

    // 3. Initialize Accelerometer
    if (!accel.begin()) {
        Serial.println("ERROR: ADXL345 not found. Check wiring!");
        u8g2.drawStr(0, 25, "IMU ERROR!");
        u8g2.sendBuffer();
        while (1); // Halt execution
    }

    // Set range to 4G (Appropriate for jumping movements)
    accel.setRange(ADXL345_RANGE_4_G);
    Serial.println("System Ready.");
}

void loop() {
    // --- 1. Get Sensor Event ---
    sensors_event_t event;
    accel.getEvent(&event);

    // --- 2. Update Display ---
    u8g2.clearBuffer();
    
    // Draw Header
    u8g2.setFont(u8g2_font_profont12_mf); // Monospaced font for numbers
    u8g2.drawStr(0, 10, "ADXL345 (m/s^2)");
    u8g2.drawHLine(0, 12, 128); // Horizontal line

    // Buffer for string formatting
    char buf[32];

    // Format and draw X axis
    sprintf(buf, "X: % 6.2f", event.acceleration.x);
    u8g2.drawStr(0, 25, buf);

    // Format and draw Y axis
    sprintf(buf, "Y: % 6.2f", event.acceleration.y);
    u8g2.drawStr(0, 37, buf);

    // Format and draw Z axis
    sprintf(buf, "Z: % 6.2f", event.acceleration.z);
    u8g2.drawStr(0, 49, buf);

    // Send buffer to screen
    u8g2.sendBuffer();

    // --- 3. Serial Debug (for Plotter) ---
    // Print in a format compatible with "Serial Plotter"
    Serial.print("X:"); Serial.print(event.acceleration.x);
    Serial.print(" Y:"); Serial.print(event.acceleration.y);
    Serial.print(" Z:"); Serial.println(event.acceleration.z);

    // Refresh rate (approx 10 FPS)
    delay(20);
}