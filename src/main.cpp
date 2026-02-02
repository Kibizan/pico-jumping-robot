/*
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
#include <SerialBT.h>

struct time {
    bool working = false;
    unsigned long now = 0;
    unsigned long last = 0;
    unsigned long delta = 0;
    uint16_t discretization = 20;
    uint8_t cycler = 0;
    uint8_t maxCycles = 5;
    uint8_t OLEDCycle = 0;
} mainTimeLoopMS;

struct state {
    uint8_t state = 0;
    String command = "";
    unsigned long timeEnterMS = 0;
    unsigned long timeInMS = 0;
} stateSpace;

// --- HARDWARE CONFIGURATION ---
// Using I2C0 on Pico W (GP0/GP1)
#define PIN_SDA 0
#define PIN_SCL 1

// --- GLOBAL OBJECTS ---
// 1. IMU Object (ID = 12345)
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
sensors_event_t IMU_event;

// 2. Display Object
// Driver for SH1106 1.3" OLED. 
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// --- SETUP FUNCTIONS ---
void setupI2C() {
    Wire.setSDA(PIN_SDA);
    Wire.setSCL(PIN_SCL);
    Wire.begin();
}

void setupOLED() {
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Booting Sensors...");
    u8g2.sendBuffer();
}

void setupIMU() {
    if (!accel.begin()) {
        while (1) {
            Serial.println("ERROR: ADXL345 not found. Check wiring!");
            u8g2.drawStr(0, 25, "IMU ERROR!");
            u8g2.sendBuffer();
        }
    }
    // Set range to 4G (Appropriate for jumping movements)
    accel.setRange(ADXL345_RANGE_4_G);
    Serial.println("System Ready.");
}

void setupSerialBT() {
    SerialBT.setName("Jumper-Pico");
    SerialBT.begin();
}

// --- UPDATE FUNCTIONS ---
void updateIMU(sensors_event_t &IMU_event) {
    accel.getEvent(&IMU_event);
}

void updateOLED(sensors_event_t &IMU_event) {
    u8g2.clearBuffer();
    // Draw Header
    u8g2.drawStr(0, 10, "ADXL345 (m/s^2)");
    u8g2.drawHLine(0, 12, 128);
    // Buffer for string formatting
    char buf[32];
    sprintf(buf, "X: % 6.2f", IMU_event.acceleration.x);
    u8g2.drawStr(0, 25, buf);
    sprintf(buf, "Y: % 6.2f", IMU_event.acceleration.y);
    u8g2.drawStr(0, 37, buf);
    sprintf(buf, "Z: % 6.2f", IMU_event.acceleration.z);
    u8g2.drawStr(0, 49, buf);
    // Send buffer to screen
    u8g2.sendBuffer();
}

String updateSerialBT() {
    String command = "";
    if (SerialBT.available()) {
        command = SerialBT.readStringUntil('\n');
        command.trim();
    }
    return command;
}

void setup() {
    // Initialize Bluetooth and Serial
    setupSerialBT();
    Serial.begin(115200);
    delay(500);
    Serial.println("--- SYSTEM BOOT ---");
    Serial.println("Bluetooth Active! Search for 'Pico-Robot'");
    SerialBT.println("--- SYSTEM BOOT ---");

    // setups
    setupI2C();
    setupOLED();
    setupIMU();
}

void loop() {
    // UPDATES & SERIAL
    mainTimeLoopMS.now = millis();
    mainTimeLoopMS.delta = mainTimeLoopMS.now - mainTimeLoopMS.last;
    if (mainTimeLoopMS.delta >= mainTimeLoopMS.discretization) {
        mainTimeLoopMS.last = mainTimeLoopMS.now;
        mainTimeLoopMS.working = true;

        // serial comms
        stateSpace.command = updateSerialBT();
        if (stateSpace.command != "") {
            stateSpace.command.toLowerCase();
            Serial.print("ACK: ");
            Serial.println(stateSpace.command);
        }

        // updates
        updateIMU(IMU_event);
        if (mainTimeLoopMS.cycler == mainTimeLoopMS.OLEDCycle) updateOLED(IMU_event);
        
        // update cycler
        mainTimeLoopMS.cycler = (mainTimeLoopMS.cycler + 1) % mainTimeLoopMS.maxCycles;
    }
    else mainTimeLoopMS.working = false;

    // STATE SPACE
    if (mainTimeLoopMS.working) {
        // time in state
        stateSpace.timeInMS = millis() - stateSpace.timeEnterMS;

        // calculating next state
        switch (stateSpace.state) {
            case 0:
                if (stateSpace.command == "calibrate") {
                    Serial.println("Beginning to calibrate...");
                    stateSpace.timeEnterMS = millis();
                    stateSpace.state = 1;
                }
                break;
            case 1:
                if (stateSpace.timeInMS >= 3000) {
                    Serial.println("Done calibrating.");
                    stateSpace.timeEnterMS = millis();
                    stateSpace.state = 0;
                }
                break;
        }

        // executing state
        switch (stateSpace.state) {
            case 0:
                digitalWrite(LED_BUILTIN, LOW);
                break;
            case 1:
                digitalWrite(LED_BUILTIN, HIGH);
                break;
        }
    }
}