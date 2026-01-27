# 📄 Project Name: Pico Jumping Robot

**Overview:**
This project aims to build a monopedal jumping robot inspired by the **SALTO** robot from UC Berkeley (as seen in this [video](https://www.youtube.com/shorts/f5nILPkynrE)). The goal is to replicate the capability for controlled jumping and dynamic balance using accessible components.

**Key Objectives:**
* Create a spring-loaded leg mechanism for propulsion.
* Implement a balance system (possibly using an inertial tail or reaction wheels) controlled by sensors.
* Achieve stable vertical jumps.

**Hardware (Tech Stack):**
* **Microcontroller:** Raspberry Pi Pico W (Dual-core ARM Cortex-M0+).
* **Sensors (IMU):** ADXL345 (Accelerometer) - *Note: Consider upgrading to MPU6050 if a gyroscope is needed for active stabilization.*
* **Interface:** 1.3" OLED Display (SH1106 Driver) via I2C for debugging and real-time feedback.
* **Communication:** I2C Protocol (Pins GP26/GP27 on I2C1).

**Software & Environment:**
* **IDE:** VS Code + PlatformIO.
* **Framework:** Arduino Core (Earle Philhower version).
* **Language:** C++.
* **Key Libraries:**
    * `Adafruit ADXL345` (Sensor reading).
    * `U8g2` (OLED Graphics).

**🚧 Current Status (Jan 2026):**
- [x] Development environment (PlatformIO) configured for Pico W.
- [x] Initial wiring of OLED and ADXL345 via I2C.
- [x] GitHub repository created for version control.
- [ ] Development of stability control algorithm.
- [ ] Integration of motors/actuators for jumping.