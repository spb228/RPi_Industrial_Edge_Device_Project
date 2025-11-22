#include "vib_sensor.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

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

int vib_sensor_close(vib_sensor_t *dev)
{
    if (!dev) return ERROR; 
    if (spi_close(dev->spi) < 0)
    {
        fprintf(stderr, "VIB: sensor close failure\n");
        return ERROR;
    }

    free(dev);

    return OK;
}


int vib_sensor_reset(vib_sensor_t *dev)
{
    if (!dev) return ERROR; 
    const uint8_t sw_reset_mask = 0x01;
    if (vib_write_reg(dev, IIS3DWB_CTRL3_C_REG, sw_reset_mask) < 0)
    { 
        fprintf(stderr, "VIB: sensor reset failure\n");
        return ERROR;
    }

    /* wait for reset to finish */
    usleep(10000); /* 10 ms */

    return OK;
}

int vib_sensor_config(vib_sensor_t *dev, iis3dwb_fs_t fs, uint8_t lpf2_en)
{
    if (!dev) return ERROR; 
    dev->fs = fs;
    dev->lpf2_en = lpf2_en;

    uint8_t reg = 0;
    const uint8_t enable_sensor_val = 0x5; 
    reg |= (enable_sensor_val << 5);        // enable sensor 
    reg |= (fs << 2);                       
    if (lpf2_en == 1) reg |= (1 << 1);

    if (vib_write_reg(dev, IIS3DWB_CTRL1_XL_REG, reg) < 0)
    {
        fprintf(stderr, "VIB: sensor config write reg error\n");
        return ERROR;
    }

    dev->fs = fs; 
    dev->lpf2_en = lpf2_en;

    return OK;
}

int vib_sensor_is_data_ready(vib_sensor_t *dev, uint8_t *ready)
{
    if (!dev || !ready) return ERROR; 

    uint8_t read_val = 0;
    if (vib_read_reg(dev, IIS3DWB_STATUS_REG, &read_val) < 0)
    {
        fprintf(stderr, "VIB: sensor data ready error\n");
        return ERROR;
    }

    const uint8_t data_ready_mask = 0x01;
    if (read_val & 0x01)
    {
        *ready = 1; // data is ready to be read
    }
    else
    {
        *ready = 0;
    }

    return 1;
}

