#ifndef SPI_HPP
#define SPI_HPP

#include <cstdint>

#define SPI_SPEED 80000000u

void spi_init();

int spi_xfer_impl(int spi, const void* tx, void* rx, uint32_t len, uint32_t speed);

inline int spi_xfer(int spi, void* data, uint32_t len, uint32_t speed = SPI_SPEED)
{
    return spi_xfer_impl(spi, data, data, len, speed);
}

inline int spi_write(int spi, const void* data, uint32_t len, uint32_t speed = SPI_SPEED)
{
    return spi_xfer_impl(spi, data, nullptr, len, speed);
}

#endif
