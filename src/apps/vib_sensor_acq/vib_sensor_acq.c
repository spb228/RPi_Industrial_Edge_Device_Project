#include "vib_sensor_acq.h"
#include "drivers/SPI/spi_driver.h"
#include "sensors/vibration/vib_sensor.h"
#include "utilities/ring_buffer/ring_buffer.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdatomic.h>
#include <stdbool.h>

static vib_sensor_t *vib_sensor = NULL; 
static ring_buffer_t *vib_rb;

static pthread_t vib_prod_thread;
static pthread_t vib_cons_thread;

static _Atomic bool v_run = false; 

/* Producer Thread */
static void *producer_thread(void *arg)
{
    vib_sensor_data_t read_sample; 
    while (atomic_load(&v_run))
    {
        uint8_t ready = 0; 
        vib_sensor_is_data_ready(vib_sensor, &ready); 
        if (!ready)
        {
            usleep(200);
            continue; 
        }

        /* read sensor */
        if (vib_sensor_read(vib_sensor, &read_sample) == OK)
        {
            ring_buffer_push(vib_rb, &read_sample);
        }

    }

    return NULL;
}

/* Consumer Thread */
static void *consumer_thread(void *arg)
{
    vib_sensor_data_t write_sample; 
    while (atomic_load(&v_run))
    {
        if (ring_buffer_pop(vib_rb, &write_sample) == OK)
        {
            /* TODO : user app logic here */
            fprintf(stdout,"[VIB_ACQ : CONSUMER] X = %d ; Y = %d ; Z = %d\n", 
                write_sample.accel_x, write_sample.accel_y, write_sample.accel_z);
        }
        else
        {
            usleep(500);
        }
    }

    return NULL;
}

/* INIT function */
int (vib_sensor_acq_init(const char *spi_path, 
                         uint8_t mode, 
                         uint32_t speed,
                         uint8_t bits, 
                         size_t rb_capacity))
{
    if (!spi_path || rb_capacity == 0) return ERROR; 

    /* open sensor */
    vib_sensor = vib_sensor_init(spi_path, mode, speed, bits);
    if (!vib_sensor) return ERROR; 

    /* configure vibration sensor */
    if (vib_sensor_config(vib_sensor, IIS3DWB_FS_2G, 0) != OK) return ERROR; 

    /* ring buffer init */
    if (ring_buffer_init(vib_rb, rb_capacity, sizeof(vib_sensor_t)) != OK) return ERROR;

    return OK;
}

int vib_sensor_acq_start(void)
{
    atomic_store(&v_run, true); /* TODO: confirm atomic works here */

    /* init threads */
    if (pthread_create(&vib_prod_thread, NULL, producer_thread, NULL) != 0) return ERROR; 
    if (pthread_create(&vib_cons_thread, NULL, consumer_thread, NULL) != 0) return ERROR; 

    return OK;
}

int vib_sensor_acq_stop(void)
{
    atomic_store(&v_run, false);

    if (pthread_join(vib_prod_thread, NULL) != 0) return ERROR; 
    if (pthread_join(vib_cons_thread, NULL) != 0) return ERROR; 

    ring_buffer_free(vib_rb);

    vib_sensor_close(vib_sensor);

    return OK; 
}

