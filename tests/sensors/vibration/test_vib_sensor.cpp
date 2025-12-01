#include <gtest/gtest.h>
#include "sensors/vibration/vib_sensor.h"
#include "common_def.h"

// Global test parameters
static uint32_t spi_speed = 25000000;
static uint32_t bits_per_word = 8;

TEST(VIB_init, fails_on_null_argument)
{
    vib_sensor_t *vib_sensor = vib_sensor_init(NULL, SPI_MODE_0, spi_speed, bits_per_word);
    EXPECT_EQ(nullptr, vib_sensor);
}

