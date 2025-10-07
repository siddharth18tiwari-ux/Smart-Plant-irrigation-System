# Smart Plant Irrigation System

An automated plant watering system that uses sensor data and programmatic control to water plants intelligently.

## Table of Contents

- [Overview](#overview)  
- [Features](#features)  
- [Components & Requirements](#components--requirements)  
- [Repository Structure](#repository-structure)  
- [Installation & Setup](#installation--setup)  
- [Usage](#usage)  
- [How It Works](#how-it-works)  
- [Contributing](#contributing)  
- [License](#license)

## Overview

This project aims to simplify plant care by making irrigation automatic and smart. The system continuously monitors soil moisture, and when the moisture drops below a predefined threshold, it triggers watering. You can also manually override watering via software.  

Two main parts:
- **DAA2.ino** — firmware for microcontroller (e.g. Arduino / ESP)  
- **DAAPBL.py** — Python script for monitoring, control, logging, or interfacing  

## Features

- Real-time soil moisture monitoring  
- Automatic watering when soil is too dry  
- Manual watering override  
- Logging of sensor readings and watering events  
- Configurable threshold values  

## Components & Requirements

### Hardware (example list)

- Microcontroller board (e.g. Arduino, ESP, etc.)  
- Soil moisture sensor  
- Water pump or valve  
- Driver transistor / relay / MOSFET  
- Flyback diode  
- Power supply (appropriate voltage for pump & controller)  
- Wires, breadboard or PCB  

### Software / Libraries

- Arduino IDE (for `.ino` firmware)  
- Python 3 (for `.py` script)  
- Required Python libraries (e.g. `serial`, `time`, `logging`)  
- (Optional) Additional modules or UI / network libraries  

## Repository Structure
