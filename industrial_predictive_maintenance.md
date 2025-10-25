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

**Kernel Configuration:**
```
CONFIG_PREEMPT_RT=y
CONFIG_HIGH_RES_TIMERS=y
CONFIG_NO_HZ_FULL=y
CONFIG_RCU_NOCB_CPU=y
CONFIG_IRQ_FORCED_THREADING=y
CONFIG_CPU_ISOLATION=y
```

#### 2. Real-Time Acquisition Layer (C)

**rt-acquisition-service**

High-priority user-space daemon running with RT scheduling policy (SCHED_FIFO).

```c
// Core architecture
struct acquisition_context {
    int adc_fd;                    // ADC device file descriptor
    void *dma_buffer;              // Shared memory for DMA
    size_t buffer_size;            // Ring buffer size
    pthread_t acquisition_thread;  // RT thread handle
    pthread_t processing_thread;   // Processing thread handle
    
    struct {
        uint32_t sample_rate;      // Hz (10000-25000)
        uint8_t channels;          // Number of active channels
        uint16_t buffer_blocks;    // Number of buffer blocks
    } config;
    
    // Lock-free ring buffer for inter-thread communication
    struct lockfree_ringbuf *rb;
};
```

**Key Responsibilities:**
- Memory locking (`mlockall()`) to prevent page faults
- CPU affinity setting to isolated cores
- Real-time thread priorities (99 for acquisition, 98 for processing)
- Watchdog timer management
- Timestamp synchronization (CLOCK_MONOTONIC_RAW)
- Data integrity checking (CRC, checksums)

**Performance Critical Code:**
```c
// Acquisition thread main loop
void* acquisition_thread_func(void* arg) {
    struct acquisition_context *ctx = arg;
    struct sched_param param = { .sched_priority = 99 };
    
    // Set real-time priority
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    
    // Lock memory
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    // Pin to isolated CPU core
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    
    while (ctx->running) {
        // Read from ADC with minimal latency
        ssize_t bytes = read(ctx->adc_fd, ctx->dma_buffer, ctx->buffer_size);
        
        // Add timestamp
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
        
        // Push to lockfree ringbuffer for processing thread
        ringbuf_push(ctx->rb, ctx->dma_buffer, bytes, &ts);
    }
    
    return NULL;
}
```

#### 3. Signal Processing Pipeline (C++)

**dsp-processing-service**

Modern C++17/20 application for computationally intensive signal processing.

```cpp
// Architecture
class SignalProcessor {
public:
    struct ProcessingConfig {
        uint32_t sample_rate;
        uint32_t fft_size;
        WindowType window;
        std::vector<FilterConfig> filters;
    };
    
    struct ProcessingResult {
        std::vector<float> time_domain_features;
        std::vector<float> frequency_domain_features;
        std::vector<float> envelope_features;
        float anomaly_score;
        uint64_t timestamp_ns;
    };
    
    ProcessingResult process(const RawDataBlock& data);
    
private:
    // FFTW plans (pre-computed for performance)
    fftwf_plan fft_plan_;
    
    // Eigen matrices for linear algebra
    Eigen::MatrixXf filter_coefficients_;
    
    // Circular buffers for overlap-add
    std::deque<float> overlap_buffer_;
    
    // Feature extractors
    std::unique_ptr<TimeFeatureExtractor> time_features_;
    std::unique_ptr<FreqFeatureExtractor> freq_features_;
    std::unique_ptr<EnvelopeAnalyzer> envelope_analyzer_;
};
```

**Processing Pipeline Stages:**

1. **Preprocessing:**
   ```cpp
   class Preprocessor {
   public:
       // Remove DC offset
       void removeDCOffset(std::vector<float>& data);
       
       // Apply window function (Hanning, Hamming, Blackman)
       void applyWindow(std::vector<float>& data, WindowType type);
       
       // Digital filtering (Butterworth, Chebyshev)
       void applyFilter(std::vector<float>& data, const FilterConfig& cfg);
       
       // Decimation for multi-rate processing
       std::vector<float> decimate(const std::vector<float>& data, int factor);
   };
   ```

2. **Time Domain Analysis:**
   ```cpp
   class TimeFeatureExtractor {
   public:
       struct TimeFeatures {
           float rms;              // Root mean square
           float peak_to_peak;     // Peak-to-peak amplitude
           float crest_factor;     // Peak/RMS ratio
           float kurtosis;         // 4th moment (impulsiveness)
           float skewness;         // 3rd moment (asymmetry)
           float shape_factor;     // RMS/mean
           float impulse_factor;   // Peak/mean
           float clearance_factor; // Peak/RMS of sqrt(abs)
       };
       
       TimeFeatures extract(const std::vector<float>& data);
   };
   ```

3. **Frequency Domain Analysis:**
   ```cpp
   class FreqFeatureExtractor {
   public:
       struct FreqFeatures {
           std::vector<float> fft_spectrum;
           std::vector<float> peak_frequencies;
           std::vector<float> peak_amplitudes;
           std::vector<float> band_powers;  // Power in specific bands
           float spectral_centroid;
           float spectral_spread;
           float spectral_rolloff;
           float spectral_flux;
           
           // Machine-specific features
           BearingFeatures bearing_faults;
           GearFeatures gear_faults;
           float rotation_frequency;
       };
       
       FreqFeatures extract(const std::vector<float>& data, float sample_rate);
       
   private:
       // Bearing fault frequency calculator
       BearingFeatures calculateBearingFrequencies(
           const std::vector<float>& spectrum,
           float rotation_freq,
           const BearingGeometry& geometry
       );
   };
   ```

4. **Envelope Analysis:**
   ```cpp
   class EnvelopeAnalyzer {
   public:
       // Hilbert transform for envelope extraction
       std::vector<float> extractEnvelope(const std::vector<float>& data);
       
       // Bandpass filter + envelope for bearing analysis
       std::vector<float> bearingEnvelopeAnalysis(
           const std::vector<float>& data,
           float low_freq,
           float high_freq
       );
       
       // Envelope spectrum
       std::vector<float> envelopeSpectrum(const std::vector<float>& envelope);
   };
   ```

**Optimization Techniques:**
- SIMD vectorization (ARM NEON intrinsics)
- FFTW wisdom files for optimal FFT plans
- Thread pool for parallel processing of multiple channels
- Memory pool allocators to avoid dynamic allocation
- Zero-copy data passing using shared memory

#### 4. ML Inference Engine (C++)

**ml-inference-service**

```cpp
class MLInferenceEngine {
public:
    struct ModelConfig {
        std::string model_path;
        ModelType type;  // ANOMALY_DETECTION, CLASSIFICATION, REGRESSION
        std::vector<std::string> input_features;
        InferenceBackend backend;  // ONNX, TFLite, Custom
    };
    
    struct InferenceResult {
        float anomaly_score;              // 0.0 - 1.0
        std::string fault_class;          // "NORMAL", "BEARING", "IMBALANCE", etc.
        float confidence;                 // Classification confidence
        std::map<std::string, float> probabilities;  // All class probabilities
        uint32_t inference_time_us;       // Performance metric
    };
    
    // Load model from file
    bool loadModel(const ModelConfig& config);
    
    // Run inference on feature vector
    InferenceResult infer(const std::vector<float>& features);
    
    // Update model (for online learning)
    bool updateModel(const std::string& new_model_path);
    
private:
    std::unique_ptr<Ort::Session> onnx_session_;  // ONNX Runtime
    std::unique_ptr<Ort::Env> onnx_env_;
    
    // Feature normalization parameters (learned during training)
    Eigen::VectorXf feature_mean_;
    Eigen::VectorXf feature_std_;
    
    // Anomaly detection threshold
    float anomaly_threshold_;
};
```

**Supported Models:**

1. **Anomaly Detection:**
   - One-Class SVM
   - Isolation Forest
   - Autoencoder (neural network)
   - Statistical baseline (Mahalanobis distance)

2. **Fault Classification:**
   - Random Forest
   - XGBoost
   - Lightweight CNN (1D convolutions on time/freq data)
   - LSTM for temporal patterns

**Model Format:**
- ONNX (Open Neural Network Exchange) for portability
- Quantized INT8 models for 4x speedup on ARM
- Model size target: <50MB for edge deployment

#### 5. Data Management Layer (C++)

**data-manager-service**

```cpp
class DataManager {
public:
    struct DataRecord {
        uint64_t timestamp_ns;
        std::string equipment_id;
        std::vector<float> features;
        float anomaly_score;
        std::string status;
        std::optional<std::vector<float>> raw_waveform;  // Stored on anomaly
    };
    
    // Local time-series storage
    class LocalStorage {
    public:
        // Write features to local database
        bool write(const DataRecord& record);
        
        // Query historical data
        std::vector<DataRecord> query(
            const std::string& equipment_id,
            uint64_t start_time,
            uint64_t end_time
        );
        
        // Manage storage (rotation, compression)
        void rotate();
        void compress(uint64_t before_timestamp);
        
    private:
        // Embedded SQLite or custom time-series format
        std::unique_ptr<Database> db_;
    };
    
    // Cloud synchronization
    class CloudSync {
    public:
        // Queue data for upload
        void enqueue(const DataRecord& record);
        
        // Background sync thread
        void sync();
        
        // Adaptive sync based on network/anomaly
        void setAdaptiveSync(bool enabled);
        
    private:
        // Upload queue (persistent across reboots)
        std::deque<DataRecord> upload_queue_;
        
        // Network client
        std::unique_ptr<MQTTClient> mqtt_client_;
    };
    
    // Data compression
    class Compressor {
    public:
        // Compress feature vectors
        std::vector<uint8_t> compress(const std::vector<float>& data);
        
        // Decompress
        std::vector<float> decompress(const std::vector<uint8_t>& data);
    };
};
```

**Storage Strategy:**
- **High-frequency features**: Store locally, upload hourly
- **Anomaly events**: Store raw waveform, upload immediately
- **Normal operation**: Compress and thin (1 sample/5min)
- **Compression**: Zstd for feature vectors, lossy for raw waveforms
- **Local retention**: 30 days (configurable)

#### 6. Communication Gateway (C++)

**cloud-gateway-service**

```cpp
class CloudGateway {
public:
    struct Config {
        std::string mqtt_broker;
        uint16_t mqtt_port;
        std::string client_id;
        std::string username;
        std::string password;
        bool use_tls;
        std::string ca_cert_path;
    };
    
    // Connect to cloud
    bool connect(const Config& config);
    
    // Publish data
    bool publish(const std::string& topic, const std::vector<uint8_t>& payload);
    
    // Subscribe to commands
    void subscribe(const std::string& topic, MessageCallback callback);
    
    // Handle incoming messages (config updates, OTA triggers)
    void messageLoop();
    
private:
    std::unique_ptr<mqtt::async_client> mqtt_client_;
    
    // Reconnection logic with exponential backoff
    void handleDisconnect();
    
    // Message serialization (Protocol Buffers)
    std::vector<uint8_t> serialize(const DataRecord& record);
    DataRecord deserialize(const std::vector<uint8_t>& data);
};
```

**Communication Protocols:**

1. **MQTT:**
   - Primary protocol for cloud communication
   - QoS 1 for critical alerts
   - QoS 0 for regular telemetry
   - Last Will & Testament for device offline detection

2. **Modbus TCP/RTU:**
   - Integration with industrial PLCs/SCADA
   - Read equipment parameters (RPM, temperature)
   - Write control commands

3. **OPC UA:**
   - Standards-based industrial connectivity
   - Pub/Sub for real-time data
   - Client/Server for equipment integration

4. **REST API:**
   - Local configuration interface
   - Status queries
   - Manual data retrieval

#### 7. System Services

**Watchdog Service:**
```cpp
class WatchdogManager {
public:
    // Register service with watchdog
    void registerService(const std::string& service_name, uint32_t timeout_ms);
    
    // Heartbeat from service
    void heartbeat(const std::string& service_name);
    
    // Monitor and restart failed services
    void monitor();
    
private:
    std::map<std::string, ServiceStatus> services_;
    
    // System reboot if critical services fail
    void systemReboot();
};
```

**Configuration Manager:**
```cpp
class ConfigManager {
public:
    // Load configuration from file
    bool load(const std::string& config_file);
    
    // Save configuration
    bool save();
    
    // Get/Set parameters
    template<typename T>
    T get(const std::string& key);
    
    template<typename T>
    void set(const std::string& key, const T& value);
    
    // Watch for configuration changes
    void watchFile(ConfigChangeCallback callback);
    
private:
    // JSON or TOML configuration
    nlohmann::json config_;
};
```

**OTA Update Manager:**
```cpp
class OTAManager {
public:
    // Download update package
    bool downloadUpdate(const std::string& update_url);
    
    // Verify update signature
    bool verifyUpdate(const std::string& update_file);
    
    // Apply update (A/B partition scheme)
    bool applyUpdate();
    
    // Rollback on failure
    bool rollback();
    
private:
    // Secure boot verification
    bool verifySignature(const std::vector<uint8_t>& data, 
                         const std::vector<uint8_t>& signature);
};
```

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
