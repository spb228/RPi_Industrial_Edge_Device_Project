#include "spi_driver.h"
#include "common_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>

spi_handle_t* spi_init(const char *device, uint8_t mode, uint32_t speed, uint8_t bits)
{
    printf("[TRACE] running spi_init\n");

    if (!device)
    {
        fprintf(stderr, "SPI: Invalid device path\n"); 
        return NULL; 
    }

    spi_handle_t *handle = (spi_handle_t*)malloc(sizeof(spi_handle_t)); 
    if (!handle)
    {
        fprintf(stderr, "SPI: mem alloc failed\n"); 
        return NULL; 
    }

    /* open SPI device */
    handle->fd = open(device, O_RDWR);
    if (handle->fd < 0)
    {
        fprintf(stderr, "SPI: failed to open device\n"); 
        free(handle);
        return NULL;
    }

    /* set SPI mode */
    if (ioctl(handle->fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        fprintf(stderr, "SPI: failed to set mode\n"); 
        close(handle->fd);
        free(handle);
        return NULL;
    }

    /* set bits per word */
    if (ioctl(handle->fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
    {
        fprintf(stderr, "SPI: failed to set bits\n"); 
        close(handle->fd);
        free(handle);
        return NULL;
    }

    /* set max speed*/
    if (ioctl(handle->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        fprintf(stderr, "SPI: failed to set speed\n"); 
        close(handle->fd);
        free(handle);
        return NULL;
    }

    handle->mode = mode; 
    handle->bits = bits; 
    handle->speed = speed;
    handle->delay = 0; 

    return handle; 
}

int spi_close(spi_handle_t* handle)
{
    if (!handle)
    {
        fprintf(stderr, "SPI: Invalid handle\n"); 
        return ERROR; 
    }

    int ret = close(handle->fd); 
    free(handle);

    return ret; 
}

int spi_write(spi_handle_t *handle, const uint8_t *data, size_t len)
{
    if (!handle || !data || len == 0)
    {
        fprintf(stderr, "SPI: Invalid parameters\n"); 
        return ERROR; 
    }

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)data,
        .rx_buf = 0, 
        .len = len, 
        .speed_hz = handle->speed,
        .delay_usecs = handle->delay, 
        .bits_per_word = handle->bits,
    };

    if (ioctl(handle->fd, SPI_IOC_MESSAGE(1), &tr) < 0)
    {
        fprintf(stderr, "SPI: write failed\n"); 
        return ERROR; 
    }

    return OK;
}

int spi_read(spi_handle_t *handle, uint8_t *data, size_t len)
{
    if (!handle || !data || len == 0)
    {
        fprintf(stderr, "SPI: Invalid parameters\n"); 
        return ERROR; 
    }

    struct spi_ioc_transfer tr = {
        .tx_buf = 0,
        .rx_buf = (unsigned long)data, 
        .len = len, 
        .speed_hz = handle->speed,
        .delay_usecs = handle->delay, 
        .bits_per_word = handle->bits,
    };

    if (ioctl(handle->fd, SPI_IOC_MESSAGE(1), &tr) < 0)
    {
        fprintf(stderr, "SPI: read failed\n"); 
        return ERROR; 
    }

    return OK;
}

int spi_transfer(spi_handle_t *handle, const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    if (!handle || !tx_data || !rx_data || len == 0)
    {
        fprintf(stderr, "SPI: Invalid parameters\n"); 
        return ERROR; 
    }

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_data,
        .rx_buf = (unsigned long)rx_data, 
        .len = len, 
        .speed_hz = handle->speed,
        .delay_usecs = handle->delay, 
        .bits_per_word = handle->bits,
    };

    if (ioctl(handle->fd, SPI_IOC_MESSAGE(1), &tr) < 0)
    {
        fprintf(stderr, "SPI: transfer failed\n"); 
        return ERROR; 
    }

    return OK;
}

int spi_write_reg(spi_handle_t *handle, uint8_t reg, uint8_t data)
{
    if (!handle)
    {
        fprintf(stderr, "SPI: Invalid handle\n"); 
        return ERROR; 
    }

    uint8_t buf[2] = {reg, data};

    return spi_write(handle, buf, sizeof(buf));
}

int spi_read_reg(spi_handle_t *handle, uint8_t reg, uint8_t *data)
{
    if (!handle || !data)
    {
        fprintf(stderr, "SPI: Invalid parameters\n"); 
        return ERROR; 
    }

    uint8_t tx_buf[2] = {reg, 0x00}; 
    uint8_t rx_buf[2] = {0};

    if (spi_transfer(handle, tx_buf, rx_buf, sizeof(tx_buf)) < 0)
    {
        fprintf(stderr, "SPI: register transfer failed\n");
        return ERROR; 
    }

    *data = rx_buf[1];

    return OK;
}
