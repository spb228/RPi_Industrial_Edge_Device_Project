/* 
Description : main project file
Author      : Swapnil Barot
*/

#include "common_def.h"
#include "apps/vib_sensor_acq/vib_sensor_acq.h"
#include "sensors/vibration/vib_sensor.h"
#include "drivers/SPI/spi_driver.h"

#include <stdio.h>
#include <unistd.h>

int main(void)
{
    fprintf(stdout, "[TRACE] running main\n");

    /* start vib sensor */
    if (vib_sensor_acq_init(SPI_DEVICE_0, 0, 8000000, 8, 512) != OK) return ERROR; 
    vib_sensor_acq_start();
    usleep(1000);  /* let threads run */
    vib_sensor_acq_stop();

    return 0;
}