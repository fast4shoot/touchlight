#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sstream>

#include "spi.hpp"

static spi_ioc_transfer xfer;

void spi_init()
{
	memset(&xfer, 0, sizeof(xfer));

	xfer.delay_usecs = 0u;
	xfer.bits_per_word = 8u;
	xfer.cs_change = 1u;
}

int spi_xfer_impl(int spi, const void* tx, void* rx, uint32_t len, uint32_t speed)
{
	xfer.len = len;
	xfer.tx_buf = reinterpret_cast<decltype(xfer.tx_buf)>(tx);
	xfer.rx_buf = reinterpret_cast<decltype(xfer.rx_buf)>(rx);
	xfer.speed_hz = speed;

	auto status = ioctl(spi, SPI_IOC_MESSAGE(1), &xfer);
	if (status == -1)
	{
		std::stringstream ss;
		ss << "SPI transfer ioctl failed with errno " << strerror(errno);
		throw new std::runtime_error(ss.str());
	}
	return status;
}


