#include "touch.hpp"

#include <algorithm>
#include "spi.hpp"

Touch::Touch(int spi):
	spi(spi)
{}

TouchPoint Touch::run()
{
	uint32_t pressure = run(0b001);
	uint32_t x = run(0b101);
	uint32_t y = run(0b001);

	if (pressure != 4095)
	{
		// normalize x
		x = std::max(400u, std::min(3780u, x));
		x -= 400;
		auto normX = x / 3380.0f;

		// normalize y
		y = std::max(330u, std::min(3780u, y));
		y -= 330;
		auto normY = y / 3450.0f;

		return TouchPoint{uint16_t(normX * 240), uint16_t(normY * 320), true};
	}
	else
	{
		return TouchPoint{0u, 0u, false};
	}
}

uint16_t Touch::run(uint8_t a)
{
	uint8_t buf[] = {
		uint8_t(0x80 | ((a & 0x7) << 4)),
		0, 0};
	spi_xfer(spi, buf, 3, 1000000);
	return ((buf[1] << 8) | buf[2]) >> 3;
}
