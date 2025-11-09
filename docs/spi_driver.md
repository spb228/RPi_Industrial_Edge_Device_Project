# Linux SPI User Space Driver

A high-level user space driver for Linux SPI devices using the `spidev` interface. This driver provides a clean, easy-to-use API for SPI communication suitable for industrial applications.

## Features

- **Simple API**: Clean, intuitive interface for SPI operations
- **Full Configuration**: Support for all SPI modes, speeds, and options
- **Error Handling**: Comprehensive error reporting
- **Thread-Safe**: Can be used in multi-threaded applications
- **Industrial Grade**: Designed for reliable operation in industrial environments

## Requirements

- Linux kernel with SPI support
- `spidev` kernel module enabled
- Access to `/dev/spidevX.Y` device files
- CMake 3.10 or higher (for building)

## Building

```bash
mkdir build
cd build
cmake ..
make
```

This will create:
- `libspi_driver.a` - Static library
- `spi_example` - Example program

## Enabling SPI on Raspberry Pi

1. Enable SPI in `raspi-config`:
   ```bash
   sudo raspi-config
   # Navigate to: Interface Options -> SPI -> Enable
   ```

2. Or manually enable in `/boot/config.txt`:
   ```
   dtparam=spi=on
   ```

3. Load the spidev module:
   ```bash
   sudo modprobe spidev
   ```

4. Verify device files exist:
   ```bash
   ls -l /dev/spidev*
   # Should show /dev/spidev0.0 and /dev/spidev0.1
   ```

5. Add user to `spi` group (to avoid needing sudo):
   ```bash
   sudo usermod -a -G spi $USER
   # Log out and back in for changes to take effect
   ```

## Usage

### Basic Example

```c
#include "spi_driver.h"

int main() {
    // Configure SPI
    spi_config_t config = SPI_CONFIG_DEFAULT;
    config.max_speed_hz = 10000000;  // 10 MHz
    config.mode = SPI_MODE_0;
    
    // Initialize device (bus 0, CS 0)
    spi_device_t* spi = spi_init(0, 0, &config);
    if (!spi) {
        fprintf(stderr, "Failed to initialize SPI\n");
        return 1;
    }
    
    // Write data
    uint8_t data[] = {0x01, 0x02, 0x03};
    spi_write(spi, data, sizeof(data));
    
    // Read data
    uint8_t rx_buf[3] = {0};
    spi_read(spi, rx_buf, sizeof(rx_buf));
    
    // Full duplex transfer
    uint8_t tx[] = {0xAA, 0xBB};
    uint8_t rx[2] = {0};
    spi_transfer(spi, tx, rx, sizeof(tx));
    
    // Cleanup
    spi_close(spi);
    return 0;
}
```

### SPI Modes

- **SPI_MODE_0**: CPOL=0, CPHA=0 (most common)
- **SPI_MODE_1**: CPOL=0, CPHA=1
- **SPI_MODE_2**: CPOL=1, CPHA=0
- **SPI_MODE_3**: CPOL=1, CPHA=1

### Configuration Options

```c
spi_config_t config = {
    .max_speed_hz = 10000000,  // Clock speed in Hz
    .mode = SPI_MODE_0,        // SPI mode (0-3)
    .bits_per_word = 8,        // Bits per word
    .lsb_first = false,        // LSB first (false = MSB first)
    .cs_high = false,          // CS active high
    .three_wire = false,       // 3-wire mode
    .loopback = false          // Loopback mode for testing
};
```

## API Reference

### Initialization

- `spi_init(bus, cs, config)` - Initialize SPI device
- `spi_close(dev)` - Close SPI device

### Data Transfer

- `spi_transfer(dev, tx_buf, rx_buf, len)` - Full duplex transfer
- `spi_write(dev, buf, len)` - Write data
- `spi_read(dev, buf, len)` - Read data
- `spi_write_read(dev, cmd, cmd_len, rx_buf, rx_len)` - Write then read

### Configuration

- `spi_set_config(dev, config)` - Update full configuration
- `spi_get_config(dev, config)` - Get current configuration
- `spi_set_speed(dev, speed_hz)` - Set clock speed
- `spi_get_speed(dev)` - Get clock speed
- `spi_set_mode(dev, mode)` - Set SPI mode
- `spi_get_mode(dev)` - Get SPI mode

### Utilities

- `spi_get_error(dev)` - Get last error message
- `spi_is_valid(dev)` - Check if device is valid

## Example: Reading from IIS3DWBTR Accelerometer

```c
#include "spi_driver.h"

// IIS3DWBTR register addresses (example)
#define IIS3DWBTR_WHO_AM_I    0x0F
#define IIS3DWBTR_CTRL1_XL    0x10
#define IIS3DWBTR_OUTX_L      0x28

int read_accelerometer() {
    spi_config_t config = SPI_CONFIG_DEFAULT;
    config.max_speed_hz = 10000000;  // 10 MHz
    config.mode = SPI_MODE_3;        // IIS3DWBTR uses mode 3
    config.bits_per_word = 8;
    
    spi_device_t* spi = spi_init(0, 0, &config);
    if (!spi) {
        return -1;
    }
    
    // Read WHO_AM_I register (0x0F | 0x80 for read)
    uint8_t cmd = IIS3DWBTR_WHO_AM_I | 0x80;
    uint8_t whoami = 0;
    spi_write_read(spi, &cmd, 1, &whoami, 1);
    
    printf("WHO_AM_I: 0x%02X\n", whoami);
    
    // Read accelerometer data (6 bytes: X, Y, Z)
    cmd = IIS3DWBTR_OUTX_L | 0x80;
    uint8_t accel_data[6] = {0};
    spi_write_read(spi, &cmd, 1, accel_data, 6);
    
    // Process data...
    
    spi_close(spi);
    return 0;
}
```

## Error Handling

Always check return values and use `spi_get_error()` for detailed error messages:

```c
if (spi_write(spi, data, len) != 0) {
    fprintf(stderr, "SPI write failed: %s\n", spi_get_error(spi));
    // Handle error
}
```

## Thread Safety

The driver is thread-safe for concurrent access to different SPI devices. However, if multiple threads access the same `spi_device_t` handle, external synchronization is required.

## Performance Considerations

- Use appropriate clock speeds for your device
- Minimize the number of transfers by batching operations
- Consider using DMA for high-speed transfers (requires kernel driver)
- For real-time applications, use `SCHED_FIFO` scheduling and CPU affinity

## Troubleshooting

### Permission Denied
```bash
# Add user to spi group
sudo usermod -a -G spi $USER
# Log out and back in
```

### Device Not Found
```bash
# Check if SPI is enabled
ls -l /dev/spidev*
# Enable SPI in raspi-config or /boot/config.txt
```

### Transfer Errors
- Verify SPI mode matches device requirements
- Check clock speed (some devices have maximum speeds)
- Verify wiring connections
- Use `spi_get_error()` for detailed error messages

## License

See LICENSE file in project root.

