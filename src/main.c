#include <stdio.h>
#include "drivers/SPI/spi_driver.h"

int main(void)
{
    printf("hello from main\n");

    spi_init();

    return 0;
}