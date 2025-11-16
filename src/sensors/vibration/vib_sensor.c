#include "vib_sensor.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Static Functions */
static int vib_write_reg(vib_sensor_t *dev, uint8_t reg, uint8_t value)
{
    if (!dev) return ERROR; 
    return spi_write_reg(dev->spi, reg, value); 
}

static int vib_read_reg(vib_sensor_t *dev, uint8_t reg, uint8_t *value)
{
    if (!dev || !value) return ERROR; 
    uint8_t reg_val = reg | IIS3DWB_READ_MASK;
    return spi_read_reg(dev->spi, reg_val, value);
}

vib_sensor_t* vib_sensor_init(const char *spi_dev_path, uint8_t mode, uint32_t speed, uint8_t bits)
{
    if (!spi_dev_path) return NULL; 
    vib_sensor_t *dev = (vib_sensor_t*)calloc(1, sizeof(vib_sensor_t));
    if (!dev)
    {
        fprintf(stderr, "VIB: alloc failure\n");
        return NULL;
    }

    /* open SPI device */
    dev->spi = spi_init(spi_dev_path, mode, speed, bits); 
    if (!dev->spi)
    {
        fprintf(stderr, "VIB: spi_init() failure\n");
        free(dev); 
        return NULL;
    }

    /* check WHO_AM_I */
    uint8_t who = 0; 
    if (vib_read_reg(dev, IIS3DWB_WHO_AM_I_REG, &who) < 0 || who != IIS3DWB_WHO_AM_I_VAL)
    {
        fprintf(stderr, "VIB: WHOAMI sensor error\n"); 
        spi_close(dev->spi);
        free(dev);
        return NULL;
    }

    fprintf(stdout, "VIB: Sensor detected (WHO_AM_I = 0x%02X)\n", who);

    /* reset the sensor */
    if (vib_sensor_reset(dev) < 0)
    {
        fprintf(stderr, "VIB: sensor reset failed\n");
        spi_close(dev->spi);
        free(dev);
        return NULL;
    }

    fprintf(stdout, "VIB: sensor init complete\n");

    return dev;
}

