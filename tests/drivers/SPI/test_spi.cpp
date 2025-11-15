#include <gtest/gtest.h>
#include "drivers/SPI/spi_driver.h"
#include "common_def.h"

// external mock control
extern bool mock_open_fail;
extern bool mock_ioctl_fail;

TEST(SPI_Init, FailsOnNullDevice)
{
    auto h = spi_init(NULL, 0, 500000, 8);
    EXPECT_EQ(h, nullptr);
}

TEST(SPI_Init, FailsWhenOpenFails)
{
    mock_open_fail = true;
    auto h = spi_init("/dev/spidev0.0", 0, 500000, 8);
    EXPECT_EQ(h, nullptr);
    mock_open_fail = false;
}

TEST(SPI_Init, Success)
{
    auto h = spi_init("/dev/spidev0.0", 0, 500000, 8);
    ASSERT_NE(h, nullptr);
    EXPECT_EQ(h->speed, 500000);
    spi_close(h);
}

TEST(SPI_Write, FailsOnNullHandle)
{
    uint8_t buf[1] = {0xAA};
    EXPECT_EQ(spi_write(NULL, buf, 1), ERROR);
}

TEST(SPI_Write, IoctlFailure)
{
    auto h = spi_init("/dev/spidev0.0", 0, 500000, 8);
    mock_ioctl_fail = true;

    uint8_t buf[1] = {0xAA};
    EXPECT_EQ(spi_write(h, buf, 1), ERROR);

    mock_ioctl_fail = false;
    spi_close(h);
}

TEST(SPI_ReadReg, SuccessfulRead)
{
    auto h = spi_init("/dev/spidev0.0", 0, 500000, 8);

    uint8_t value = 0;
    EXPECT_EQ(spi_read_reg(h, 0x10, &value), OK);

    spi_close(h);
}
