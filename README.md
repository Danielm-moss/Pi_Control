# Stepper Motor Rail Exposure Testing System

A synchronized system for automated exposure time testing using a stepper motor rail, Raspberry Pi, and flash-triggered camera capture.

## Overview

This project automates the process of testing multiple exposure times by:
1. Moving a target along a stepper motor rail at consistent speed
2. Triggering a flash when a photoelectric sensor detects the target
3. Capturing images with incrementing exposure times

## Hardware Requirements

- Stepper motor with driver (400 pulse/rev, 2.8A RMS / 3.9A peak)
- Arduino-compatible board (for rail control)
- Raspberry Pi 5 with IMX296 Global Shutter camera
- Photoelectric sensor (mounted on rail)
- Flash unit (GPIO-triggered)

## System Components

### Arduino Rail Controller (`rail.ino`)

Controls the stepper motor rail using the FastAccelStepper library.

| Parameter | Value | Notes |
|-----------|-------|-------|
| `EXP_TIMES` | 10 | Total exposure times to test |
| `distance` | -4880 steps | ~2 meters (negative = away from motor) |
| `MAX_SPEED` | 4500 steps/s | ~4.5 mph |
| `ACCEL` | 4500 steps/s² | Acceleration rate |

The rail completes `LAPS` back-and-forth movements, where `LAPS = ceil(EXP_TIMES / 2)`. Each direction change includes a 1.5s delay.

**Motor Driver Settings:**
- Pulse/rev: 400 (maintains steps-to-distance ratio)
- Current: 2.8A RMS, 3.9A peak

### Pi Trigger Script (`trig.cpp`)

C++ program using libgpiod to control flash timing based on sensor input.

| Pin | Function |
|-----|----------|
| GPIO 21 | Photoelectric sensor input (pull-up) |
| GPIO 22 | Flash trigger output |

The script iterates through the exposure time array, triggering the flash for the specified duration when the sensor is activated. A 2-second debounce delay prevents rapid retriggering.

**Build and run:**
```bash
g++ -o trig trig.cpp -lgpiodcxx -std=c++17
sudo ./trig
```

### Pi Capture Script (`cap.sh`)

Bash script for capturing images with the IMX296 camera.

- Disables hardware trigger mode
- Captures one frame per exposure time
- Names files as `T_{exposure}_G_{gain}.jpg`
- Saves to `~/Projects/moss/frames/`

**Run:**
```bash
chmod +x cap.sh
./cap.sh
```

**Transfer files to Windows:**
```bash
scp -r "Pi name":~/Projects/moss/frames "local path"
```

### Combined Script (`both.sh`)

Orchestrates both capture and trigger scripts:
```bash
./both.sh
```

## Configuration

### Setting Exposure Times

Update the exposure array in both Pi scripts to match:

**trig.cpp:**
```cpp
const unsigned time[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
```

**cap.sh:**
```bash
EXPOSURES=(100 200 300 400 500 600 700 800 900 1000)
```

### Setting Number of Exposures

Update `EXP_TIMES` in the Arduino sketch to match the array length:
```cpp
const int EXP_TIMES = 10;
```

## Workflow

1. Position the target at the home position (motor end of rail)
2. Start the combined script on the Pi: `./both.sh`
3. Power on or reset the Arduino to begin rail movement
4. The system will automatically:
   - Move the target along the rail
   - Trigger flash/capture when sensor is tripped
   - Return to home and repeat for remaining exposures
5. Retrieve captured images from the output directory

## File Structure

```
├── rail.ino          # Arduino stepper control
├── trig.cpp          # Pi GPIO trigger script
├── cap.sh            # Pi camera capture script
├── both.sh           # Combined orchestration script
└── frames/           # Captured images output
```

## Dependencies

**Arduino:**
- FastAccelStepper library

**Raspberry Pi:**
- libgpiod (C++ bindings)
- rpicam-apps
- IMX296 camera driver with trigger mode support
