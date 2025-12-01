#include <gtest/gtest.h>
#include "drivers/SPI/spi_driver.h"
#include "common_def.h"

// external mock control
extern bool mock_open_fail;
extern bool mock_ioctl_fail;

// Global test parameters
static uint32_t spi_speed = 25000000;
static uint32_t bits_per_word = 8;
static uint8_t reg = 0x01;
static uint8_t data = 0x0A;

TEST(SPI_init, fails_on_null_argument)
{
    spi_handle_t* handle = spi_init(NULL, SPI_MODE_0, spi_speed, bits_per_word); 
    EXPECT_EQ(handle, nullptr);
}

TEST(SPI_init, fails_on_open_failure)
{
    mock_open_fail = true; 
    spi_handle_t* handle = spi_init(SPI_DEVICE_0, SPI_MODE_0, spi_speed, bits_per_word);
    EXPECT_EQ(handle, nullptr);
    mock_open_fail = false; 
}

TEST(SPI_init, fails_on_ioctl_failure)
{
    mock_ioctl_fail = true; 
    spi_handle_t* handle = spi_init(SPI_DEVICE_0, SPI_MODE_0, spi_speed, bits_per_word);
    EXPECT_EQ(handle, nullptr);
    mock_ioctl_fail = false; 
}

TEST(SPI_init, success)
{
    spi_handle_t* handle = spi_init(SPI_DEVICE_0, SPI_MODE_0, spi_speed, bits_per_word);
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(1, handle->fd); 
    ASSERT_EQ(SPI_MODE_0, handle->mode);
    ASSERT_EQ(spi_speed, handle->speed);
    ASSERT_EQ(bits_per_word, handle->bits);
    ASSERT_EQ(0, handle->delay);
}

TEST(spi_write_reg, fail_on_null_argument)
{
    int ret = spi_write_reg(nullptr, reg, data);
    ASSERT_EQ(ERROR, ret);
}

TEST(spi_write_reg, success)
{
    spi_handle_t* handle = spi_init(SPI_DEVICE_0, SPI_MODE_0, spi_speed, bits_per_word);
    int ret = spi_write_reg(handle, reg, data);
    ASSERT_EQ(OK, ret);
}

TEST(spi_read_reg, fail_on_null_argument)
{
    int ret = spi_read_reg(nullptr, reg, &data);
    ASSERT_EQ(ERROR, ret);
}

TEST(spi_read_reg, success)
{
    spi_handle_t* handle = spi_init(SPI_DEVICE_0, SPI_MODE_0, spi_speed, bits_per_word);
    int ret = spi_read_reg(handle, reg, &data);
    ASSERT_EQ(OK, ret);
    // TODO - add a data value and check if it comes back
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
