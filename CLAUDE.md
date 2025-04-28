# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands
- Build project: `pio run`
- Build specific environment: `pio run -e emulator` or `pio run -e M5Dial`
- Run emulator: `pio run -e emulator -t execute`
- Upload to device: `pio run -e M5Dial -t upload`
- Monitor serial output: `pio device monitor`

## Code Style Guidelines
- Use C++17 features and idioms
- Class names: PascalCase
- Methods and variables: camelCase
- Private member variables: no prefix/suffix convention
- Headers: Use .hpp extension, implementation in .cpp files
- Organize related functionality into App classes that inherit from App base class
- Use smart pointers (std::unique_ptr) for ownership
- Follow LVGL conventions for UI components
- Indentation: 4 spaces
- Line length: Prefer under 120 characters
- Use lv_ prefix for LVGL-related variables
- Error handling: Check pointers before use, provide debug output

## Architecture
- App base class for all applications
- ScreenManager handles app registration and transitions
- All UI applications instantiated through factory methods
- Use LVGL for UI components