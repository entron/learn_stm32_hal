# STM32F103C8 Learning Project

A comprehensive embedded systems learning project for the STM32F103C8 microcontroller (Blue Pill board), featuring modular component drivers and a practical demonstration application.

## Overview

This project demonstrates various embedded programming concepts including:
- GPIO control and interrupts
- PWM generation for servo and motor control
- ADC for analog sensor readings
- I2C communication with OLED display
- UART communication for data logging
- Modular driver architecture

## Hardware Setup

### Microcontroller
- **STM32F103C8T6** (Blue Pill board)
- 72MHz ARM Cortex-M3
- 64KB Flash, 20KB RAM
- Development framework: STM32Cube HAL

### Components and Pin Assignments

| Component | Pin(s) | Function | Notes |
|-----------|--------|----------|-------|
| **Buttons** | PB1, PB11 | Input with interrupts | Active low with pull-up |
| **Servo Motor** | PA1 | PWM output (TIM2_CH2) | 50Hz PWM, 0.5-2.5ms pulse width |
| **DC Motor** | PA2, PA4, PA5 | PWM + Direction control | TB6612FNG motor driver |
| **Light Sensor** | PA7 | ADC input (ADC1_CH7) | Analog photoresistor |
| **OLED Display** | PB8, PB9 | I2C (SCL, SDA) | 128x64 SSD1306 |
| **UART** | PA9, PA10 | TX, RX | 115200 baud for data logging |
| **LED** | PA0 | Digital output | Built-in LED |
| **Buzzer** | PB12 | Digital output | Active low |
| **IR Sensor** | PB14 | Digital input with interrupt | Beam break detection |

## Project Structure

```
├── platformio.ini          # PlatformIO configuration
├── include/                # Header files
│   ├── board.h            # Core system functions
│   ├── keys.h             # Button input handling
│   ├── servo.h            # Servo motor control
│   ├── motor.h            # DC motor control
│   ├── light_sensor.h     # Light sensor (analog/digital)
│   ├── oled.h             # OLED display wrapper
│   ├── usart.h            # UART communication
│   ├── led.h              # LED control
│   ├── buzzer.h           # Buzzer control
│   └── ir_sensor.h        # IR sensor input
├── src/                   # Source files
│   ├── main.c             # Main application
│   ├── board.c            # System initialization
│   ├── keys.c             # Button implementation
│   ├── servo.c            # Servo control implementation
│   ├── motor.c            # Motor control implementation
│   ├── light_sensor.c     # Light sensor implementation
│   ├── oled.c             # OLED display implementation
│   ├── usart.c            # UART implementation
│   ├── led.c              # LED implementation
│   ├── buzzer.c           # Buzzer implementation
│   ├── ir_sensor.c        # IR sensor implementation
│   └── stm32f1xx_it.c     # Interrupt handlers
└── lib/                   # External libraries
    └── ssd1306/           # SSD1306 OLED driver library
```

## Features

### Main Application (`main.c`)
A demonstration application that combines multiple peripherals:

- **Servo Control**: Button B1 cycles servo angle (0° → 30° → 60° → ... → 180° → 0°)
- **Motor Control**: Button B11 cycles motor speed (0% → 20% → 50% → -20% → -50% → 0%)
- **Light Monitoring**: Continuous ADC reading of light sensor
- **OLED Display**: Real-time status display showing servo angle, motor speed, and light level
- **Data Logging**: UART transmission of light sensor data in CSV format every second
- **Interrupt-driven Input**: Debounced button handling using GPIO EXTI

### Component Modules

#### Core System (`board.h/c`)
- HAL library initialization
- System clock configuration (72MHz)
- Error handling
- SysTick timer setup

#### Button Input (`keys.h/c`)
- GPIO configuration with pull-up resistors
- External interrupt (EXTI) setup for buttons
- Debouncing implementation
- ISR callback for button events

#### Servo Control (`servo.h/c`)
- TIM2 Channel 2 PWM generation (50Hz)
- Microsecond-precise pulse width control
- Degree-based position interface (0-180°)
- Safe pulse width limits (0.5-2.5ms)

#### DC Motor Control (`motor.h/c`)
- PWM speed control via TIM2 Channel 3
- Direction control using GPIO pins
- Bidirectional operation (-100% to +100%)
- TB6612FNG motor driver interface

#### Light Sensor (`light_sensor.h/c`)
- ADC1 Channel 7 for analog readings
- 12-bit resolution (0-4095)
- Percentage conversion (0-100%)
- Both raw and inverted readings

#### OLED Display (`oled.h/c`)
- I2C communication wrapper for SSD1306
- Text rendering with multiple font sizes
- Graphics primitives (pixels, lines, rectangles, circles)
- Printf-style formatted text output
- Buffer management and display updates

#### UART Communication (`usart.h/c`)
- USART1 configuration (115200 baud)
- String transmission functions
- Printf-style formatted output
- Data logging capabilities

### Additional Components
- **LED Control** (`led.h/c`): Simple GPIO control for status indication
- **Buzzer Control** (`buzzer.h/c`): Audio feedback via GPIO
- **IR Sensor** (`ir_sensor.h/c`): Beam break detection with interrupts

## Building and Flashing

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- STM32F103C8 board
- ST-Link programmer or USB-to-Serial adapter

### Build Commands
```bash
# Build the project
pio run

# Upload to microcontroller
pio run --target upload

# Open serial monitor
pio device monitor --baud 115200
```

### Programming Methods
1. **ST-Link**: Direct SWD programming
2. **USB-to-Serial**: Bootloader mode (BOOT0 = HIGH)

## Usage

### Basic Operation
1. Power on the STM32 board
2. The OLED display shows current status
3. Use buttons to control servo and motor:
   - **B1 (PB1)**: Cycle servo position
   - **B11 (PB11)**: Cycle motor speed
4. Light sensor data is continuously displayed and logged via UART

### Serial Data Logging
Connect to UART (115200 baud) to receive CSV data:
```
STM32F103C8 Light Sensor Monitor
Sending light sensor readings every 1 second
Format: timestamp_ms,light_percent,light_raw
---
1000,45,1854
2000,47,1920
3000,43,1790
...
```

### OLED Display Format
```
Servo: 90°
Motor: 50%
Light: 45% (1854)
B1:Servo B11:Motor
```

## Technical Details

### Timer Configuration
- **TIM2**: 1MHz tick rate (1μs resolution)
- **Channel 2**: Servo PWM (20ms period)
- **Channel 3**: Motor PWM (shared timer)

### ADC Configuration
- **ADC1**: 12-bit resolution
- **Sample time**: Configured for stable readings
- **Single conversion mode**

### Interrupt Handling
- **EXTI Lines 1, 11**: Button interrupts with 200ms debouncing
- **EXTI Line 14**: IR sensor beam break detection
- **SysTick**: 1ms system timing

### Memory Usage
- **Flash**: ~21KB (32.7% of 64KB)
- **RAM**: ~1.4KB (6.7% of 20KB)

## Development Notes

### Modular Design
The project uses a modular architecture where each peripheral has its own header/source file pair. This approach provides:
- **Maintainability**: Easy to modify individual components
- **Reusability**: Components can be used in other projects
- **Scalability**: Simple to add new peripherals
- **Resource Efficiency**: Only include what you need

### Adding New Components
1. Create header file in `include/`
2. Create source file in `src/`
3. Include the header in `main.c`
4. Initialize the component in `main()`

### Best Practices Demonstrated
- Proper GPIO configuration and pin definitions
- Interrupt-driven input with debouncing
- Timer configuration for precise PWM
- ADC calibration and conversion
- I2C communication protocol
- UART data transmission
- Modular code organization
- Resource optimization

## Learning Objectives

This project covers essential embedded systems concepts:
- **GPIO**: Digital input/output with interrupts
- **Timers**: PWM generation and precise timing
- **ADC**: Analog sensor interfacing
- **Communication**: I2C and UART protocols
- **Interrupts**: Event-driven programming
- **Architecture**: Modular firmware design
- **Debugging**: Serial output and status display

## Troubleshooting

### Common Issues
1. **Upload failures**: Check BOOT0 jumper position
2. **No UART output**: Verify baud rate and connections
3. **OLED not working**: Check I2C connections (PB8/PB9)
4. **Servo jitter**: Ensure stable power supply
5. **Button not responding**: Check pull-up configuration

### Debug Features
- Real-time status on OLED display
- Serial output for monitoring
- LED indicators for system status
- Error handlers for fault conditions

## Future Enhancements

Potential additions to expand learning:
- Real-time operating system (FreeRTOS)
- SD card data logging
- Wireless communication (ESP8266/NRF24L01)
- More sensor types (temperature, humidity, etc.)
- Advanced motor control (PID, encoders)
- Web interface for remote monitoring

---

This project serves as a comprehensive introduction to STM32 development, demonstrating professional embedded programming practices while maintaining code clarity for educational purposes.
