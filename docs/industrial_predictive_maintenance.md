# Industrial Predictive Maintenance System
## Raspberry Pi-based Edge Computing Platform for Industry 4.0

> A comprehensive, open-source industrial predictive maintenance solution built on Raspberry Pi 4/5, leveraging real-time Linux capabilities, custom Yocto distributions, and high-performance C/C++ for edge computing in manufacturing environments.

## Table of Contents
- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Hardware Platform](#hardware-platform)

---

## Overview

Industrial equipment failures cost manufacturers $50-100K+ per hour in unplanned downtime. This project delivers an enterprise-grade predictive maintenance system using cost-effective Raspberry Pi hardware combined with real-time Linux kernel capabilities, enabling sub-millisecond deterministic response times for critical industrial monitoring applications.

### Key Features

- **Real-time Performance**: PREEMPT_RT patched Linux 6.12 kernel for deterministic latency (<100μs)
- **High-frequency Data Acquisition**: Multi-channel vibration analysis at 25+ kHz sampling rates
- **Edge ML Inference**: On-device anomaly detection and fault classification
- **Industrial Protocols**: Native support for Modbus, MQTT, OPC UA
- **Custom Linux Distribution**: Minimal Yocto-based system optimized for industrial edge computing
- **Production Ready**: Systemd services, watchdog support, OTA updates, logging infrastructure

### Technology Stack

- **Hardware**: Raspberry Pi 4 (4GB+) or Raspberry Pi 5
- **OS**: Custom Yocto Linux 6.12 with PREEMPT_RT patch
- **Core Languages**: C (kernel modules, drivers, real-time components) and C++ (application logic, signal processing)
- **Build System**: CMake, Yocto BitBake
- **Libraries**: FFTW3, Eigen, ONNX Runtime, Protocol Buffers

---

## System Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     CLOUD SERVICES                          │
│  ┌──────────────┬──────────────┬────────────────────────┐  │
│  │ Time-Series  │ ML Training  │ Web Dashboard/Alerts   │  │
│  │   Database   │   Pipeline   │                        │  │
│  └──────────────┴──────────────┴────────────────────────┘  │
└───────────────────────────┬─────────────────────────────────┘
                            │ MQTT/HTTPS
                            │
┌───────────────────────────┴─────────────────────────────────┐
│              RASPBERRY PI EDGE DEVICE                       │
│  ┌─────────────────────────────────────────────────────┐   │
│  │         APPLICATION LAYER (User Space)              │   │
│  │  ┌──────────────┬──────────────┬─────────────────┐ │   │
│  │  │ Data Manager │ ML Inference │ Cloud Gateway   │ │   │
│  │  │   (C++)      │  Engine      │    (C++)        │ │   │
│  │  │              │   (C++)      │                 │ │   │
│  │  └──────────────┴──────────────┴─────────────────┘ │   │
│  │  ┌──────────────────────────────────────────────┐  │   │
│  │  │     Signal Processing Pipeline (C++)         │  │   │
│  │  │  FFT • Filtering • Feature Extraction        │  │   │
│  │  └──────────────────────────────────────────────┘  │   │
│  │  ┌──────────────────────────────────────────────┐  │   │
│  │  │    Real-Time Acquisition Service (C)         │  │   │
│  │  │  High-priority threads • RT scheduling       │  │   │
│  │  └──────────────────────────────────────────────┘  │   │
│  └─────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │          KERNEL LAYER (Linux 6.12 RT)               │   │
│  │  ┌──────────────┬──────────────┬─────────────────┐ │   │
│  │  │ ADC Driver   │ GPIO Driver  │ I2C/SPI Drivers │ │   │
│  │  │  (Kernel     │  (Kernel     │   (Kernel       │ │   │
│  │  │   Module)    │   Module)    │    Modules)     │ │   │
│  │  └──────────────┴──────────────┴─────────────────┘ │   │
│  └─────────────────────────────────────────────────────┘   │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────┴─────────────────────────────────┐
│                  HARDWARE LAYER                             │
│  ┌──────────────┬──────────────┬─────────────────────────┐ │
│  │ ADC Board    │ Accelero-    │ Temperature/Current     │ │
│  │ (ADS1256)    │  meters      │      Sensors            │ │
│  │ 8ch 24-bit   │ (I2C/SPI)    │                         │ │
│  └──────────────┴──────────────┴─────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Component Architecture

#### 1. Kernel Layer (C)

**Custom Kernel Modules:**
- **ADC Driver Module** (`adc_driver.ko`)
  - Character device driver for high-speed ADC (ADS1256 or similar)
  - DMA-based data transfer for zero-copy acquisition
  - Implements circular buffer in kernel space
  - Provides `/dev/adc0` character device interface
  - Real-time priority IRQ handlers

- **GPIO/Trigger Module** (`gpio_trigger.ko`)
  - Synchronization triggers for multi-channel acquisition
  - Precision timing using hardware timers
  - Interrupt handling for external triggers

- **Sensor Interface Drivers**
  - I2C/SPI accelerometer drivers (ADXL355, ADXL357)
  - Temperature sensor drivers (MAX31855, PT100)
  - Current sensor interfaces (INA219, ACS712)

#### 2. Real-Time Acquisition Layer (C)

**rt-acquisition-service**

High-priority user-space daemon running with RT scheduling policy (SCHED_FIFO).

**Key Responsibilities:**
- Memory locking (`mlockall()`) to prevent page faults
- CPU affinity setting to isolated cores
- Real-time thread priorities (99 for acquisition, 98 for processing)
- Watchdog timer management
- Timestamp synchronization (CLOCK_MONOTONIC_RAW)
- Data integrity checking (CRC, checksums)


#### 3. Signal Processing Pipeline (C++)

**dsp-processing-service**

Modern C++17/20 application for computationally intensive signal processing.

TBD

#### 4. ML Inference Engine (C++)

TBD

---

## Hardware Platform

### Raspberry Pi Configuration

**Recommended Hardware:**
- **Raspberry Pi 5 (8GB)** - Best performance, PCIe support
  - Quad-core Cortex-A76 @ 2.4GHz
  - 8GB LPDDR4X RAM
  - PCIe 2.0 x1 (for high-speed ADC cards)
  
- **Raspberry Pi 4 Model B (4GB/8GB)** - Cost-effective alternative
  - Quad-core Cortex-A72 @ 1.8GHz
  - 4GB/8GB LPDDR4 RAM

**Storage:**
- Industrial-grade microSD card (SLC or pSLC) - 32GB minimum
- Or M.2 SSD via PCIe (Pi 5) or USB 3.0 adapter (Pi 4)

### Sensor Interface Hardware

**ADC Solution:**

**Option 1: High-Speed Multi-Channel ADC HAT**
- Texas Instruments ADS1256 (8-channel, 24-bit, 30kSPS)
- SPI interface to Raspberry Pi
- Differential inputs for noise immunity
- Programmable gain amplifier (PGA)
- On-board voltage reference

**Option 2: Industrial I/O HAT**
- Sequent Microsystems Industrial Automation HAT
- 8x analog inputs (0-10V, 4-20mA)
- 8x digital I/O
- RS485 for Modbus RTU

**Option 3: PCIe ADC Card (Pi 5 only)**
- Commercial PCIe data acquisition card
- Higher sampling rates (100kSPS+)
- More channels (16+)

**Accelerometer Options:**

1. **ADXL355 (I2C/SPI):**
   - ±2g/±4g/±8g ranges
   - 4kHz output data rate
   - Ultra-low noise (25μg/√Hz)
   - Industrial temperature range

2. **ADXL357 (I2C/SPI):**
   - ±10g/±20g/±40g ranges
   - Higher range for heavy machinery

3. **Industrial Accelerometers (Analog):**
   - ICP/IEPE accelerometers via ADC
   - Higher sensitivity and wider frequency range
   - Requires signal conditioning

**Temperature Sensors:**
- MAX31855 (Thermocouple amplifier, SPI)
- PT100/PT1000 RTD via MAX31865 (SPI)
- DS18B20 (1-Wire, lower accuracy but simple)

**Current Sensors:**
- INA219 (I2C, ±3.2A, high-side)
- ACS712 (Analog out, ±5A/20A/30A)
- Split-core CT clamps via ADC

### Enclosure & Industrial Requirements

**Enclosure:**
- IP65 rated minimum (dust-tight, water-resistant)
- DIN rail mountable
- Ventilation/fan for Pi in hot environments
- Cable glands for sensor connections

**Power:**
- 24V DC industrial power supply
- DC-DC converter to 5V 3A for Raspberry Pi
- Backup capacitor bank for graceful shutdown
- Overvoltage/reverse polarity protection

**Environmental:**
- Operating temperature: -20°C to +60°C (with cooling)
- Humidity: 10-90% non-condensing
- Vibration: Secure mounting with dampers if needed

---
