#pragma once

#include <stdint.h>
#include <stddef.h>

#define SPI_MODE_0          0x00        // CPOL=0, CPHA=0
#define SPI_MODE_1          0x01        // CPOL=0, CPHA=1
#define SPI_MODE_2          0x02        // CPOL=1, CPHA=0
#define SPI_MODE_3          0x03        // CPOL=1, CPHA=1

#define SPI_CS_HIGH         0x04        // CS active high
#define SPI_LSB_FIRST       0x08        // LSB first
#define SPI_3WIRE           0x10        // 3-wire mode
#define SPI_LOOP            0x20        // loopback mode
#define SPI_NO_CS           0x40        // no CS

#define SPI_DEVICE_0        "/dev/spidev0.0"
#define SPI_DEVICE_1        "/dev/spidev0.1"

typedef struct 
{
    int fd; 
    uint8_t mode;           // SPI Mode
    uint8_t bits;           // Bits per word
    uint32_t speed;         // Speed in hz
    uint16_t delay;         // Delay in uS
} spi_handle_t;

spi_handle_t* spi_init(const char *device, uint8_t mode, uint32_t speed, uint8_t bits);

int spi_close(spi_handle_t* handle);

int spi_write(spi_handle_t *handle, const uint8_t *data, size_t len);

int spi_read(spi_handle_t *handle, uint8_t *data, size_t len); 

/* Full-dupex read and write simulatneously */
int spi_transfer(spi_handle_t *handle, const uint8_t *tx_data, uint8_t rx_data, size_t len);

int spi_write_reg(spi_handle_t *handle, uint8_t reg, uint8_t data);

int spi_read_reg(spi_handle_t *handle, uint8_t reg, uint8_t *data);

int spi_set_speed(spi_handle_t *handle, uint32_t speed);

int spi_set_mode(spi_handle_t *handle, uint8_t mode);
