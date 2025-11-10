#include "spi_driver.h"

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

