# libhmk - Open Source Hall-Effect Keyboard Library

This library is for building custom Hall-effect keyboard firmware, built on top of [tinyusb](https://github.com/hathach/tinyusb). It is designed with performance in mind and currently supports only the STM32F446RE microcontroller.

## Table of Contents

- [Features](#features)
- [Limitations](#limitations)
- [Getting Started](#getting-started)
- [Acknowledgements](#acknowledgements)

## Features

- [x] **Analog Input**: Processes Hall-effect sensor output to accurately determine key positions, enabling advanced features beyond digital input.
- [x] **Automatic Calibration**: Calibrates analog input automatically to compensate for environmental factors and manufacturing variations.
- [x] **Rapid Trigger**: Handles key press/release events dynamically based on key position changes.
- [x] **Continuous Rapid Trigger**: Allows customization of activation/deactivation points for Rapid Trigger functionality.
- [x] **Dynamic Keystroke**: Supports up to 4 different actions per keystroke.
- [x] **Dynamic Keymap**: Enables real-time keymap modifications.
- [x] **Tap-Hold**: Executes different actions based on whether a key is tapped or held.
- [x] **N-Key Rollover**: Provides N-Key Rollover with automatic fallback to 6-Key Rollover for incompatible hosts.
- [x] **Web Configurator**: Configure firmware settings through the [webhmk](https://github.com/peppapighs/webhmk) web interface.

## Limitations

- **MCU Support**: Currently limited to STM32F446RE microcontroller.
- **Polling Rate**: Maximum 1000Hz polling rate due to STM32F446RE and USB Full-Speed limitations.
- **RGB Lighting**: Not supported and no plans for future support.

## Getting Started

1. Create a new STM32F446RE project in STM32CubeIDE and set to Release configuration (or Debug for development).

2. Configure MCU settings for:

   - Clock timing
   - ADC (Analog-Digital Converter)
   - GPIO (General Purpose Input/Output)
   - USB interface

3. Copy these library directories to your project:

   - [`lib/app`](lib/app)
   - [`lib/hardware`](lib/hardware)
   - [`lib/tinyusb`](lib/tinyusb)

   Then add them to your include path and source locations.

4. Create the config file `Core/Inc/config.h` with your firmware settings.

   See [`example/GAUSS64/app/Core/Inc/config.h`](example/GAUSS64/app/Core/Inc/config.h) for reference.

5. Create `Core/Src/user.c` and implement:

   - Hall-effect sensor scanning logic
   - Calibration helper function overrides
   - Default user configuration

   See [`example/GAUSS64/app/Core/Src/user.c`](example/GAUSS64/app/Core/Src/user.c) for reference.

6. Compile and flash the firmware using your preferred method.

## Acknowledgements

- [hathach/tinyusb](https://github.com/hathach/tinyusb) for the USB stack.
- [qmk/qmk_firmware](https://github.com/qmk/qmk_firmware) for inspiration, including EEPROM emulation and matrix scanning.
- [@riskable](https://github.com/riskable) for pioneering custom Hall-effect keyboard firmware development.
- [@heiso](https://github.com/heiso/) for his [macrolev](https://github.com/heiso/macrolev) and his helpfulness throughout the development process.
- [Wooting](https://wooting.io/) for pioneering Hall-effect gaming keyboards and introducing many advanced features based on analog input.
- [GEONWORKS](https://geon.works/) for the Venom 60HE PCB and inspiring the web configurator.
