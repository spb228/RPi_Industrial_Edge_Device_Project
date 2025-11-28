#pragma once

#include <stddef.h>
#include <stdint.h>

/* Initialize spi driver, vib sensor and ring buffer */
int (vib_sensor_acq_init(const char *spi_path, 
                         uint8_t mode, 
                         uint32_t speed,
                         uint8_t bits, 
                         size_t rb_capacity));

/* start producer consumer threads */
int vib_sensor_acq_start(void);

/* stop producer consumer threads */
int vib_sensor_acq_stop(void);
