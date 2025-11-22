/* 
Description : IIS3DWB Vibration Sensor header file
Author      : Swapnil Barot
*/

#pragma once
#include "drivers/SPI/spi_driver.h"
#include "common_def.h"

/* IIS3DWB Register Addresses
 - MSB of reg address is 0 for write and 1 for read
*/ 
#define IIS3DWB_WHO_AM_I_REG            0x0F
#define IIS3DWB_WHO_AM_I_VAL            0x7B    // expected value
#define IIS3DWB_STATUS_REG              0x1E
#define IIS3DWB_CTRL1_XL_REG            0x10    // output data rate
#define IIS3DWB_CTRL3_C_REG             0x12    // control boot, reset, etc.
#define IIS3DWB_OUT_X_L_REG             0X28    // x-axis accel data
#define IIS3DWB_FIFO_CTRL4_REG          0x0A    // FIFO configuration
#define IIS3DWB_READ_MASK               0x80    // MSB = 1 for read

/* Struct definitions */
typedef struct
{
    int16_t accel_x; 
    int16_t accel_y; 
    int16_t accel_z; 
} vib_sensor_data_t;

typedef enum {
    IIS3DWB_FS_2G  = 0b00,
    IIS3DWB_FS_16G = 0b01,
    IIS3DWB_FS_4G  = 0b10,
    IIS3DWB_FS_8G  = 0b11
} iis3dwb_fs_t;

typedef struct
{
    spi_handle_t *spi; 
    uint8_t fs;             // full scale msrment rate
    uint8_t lpf2_en;        // filter output from stage 1 filter (0 val) or stage 2 filer (1 val)
} vib_sensor_t;

/* Function definitions */
vib_sensor_t* vib_sensor_init(const char *spi_dev_path, uint8_t mode, uint32_t speed, uint8_t bits);
int vib_sensor_close(vib_sensor_t *dev);
int vib_sensor_reset(vib_sensor_t *dev);
int vib_sensor_config(vib_sensor_t *dev, iis3dwb_fs_t fs, uint8_t lpf2_en);
int vib_sensor_is_data_ready(vib_sensor_t *dev, uint8_t *ready);
int vib_sensor_read(vib_sensor_t *dev, vib_sensor_data_t *data);
