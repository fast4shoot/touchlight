#include "display.hpp"

#include <algorithm>
#include <iostream>
#include <wiringPi.h>
#include "spi.hpp"


#define HIBYTE(x)((uint8_t)(((x) >> 8) & 0xff))
#define LOBYTE(x)((uint8_t)((x) & 0xff))

#define ILI9341_NOP 0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID 0x04
#define ILI9341_RDDST 0x09
#define ILI9341_SLPIN 0x10
#define ILI9341_SLPOUT 0x11
#define ILI9341_PTLON 0x12
#define ILI9341_NORON 0x13
#define ILI9341_RDMODE 0x0A
#define ILI9341_RDMADCTL 0x0B
#define ILI9341_RDPIXFMT 0x0C
#define ILI9341_RDIMGFMT 0x0A
#define ILI9341_RDSELFDIAG 0x0F
#define ILI9341_INVOFF 0x20
#define ILI9341_INVON 0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON 0x29
#define ILI9341_CASET 0x2A
#define ILI9341_PASET 0x2B
#define ILI9341_RAMWR 0x2C
#define ILI9341_RAMRD 0x2E
#define ILI9341_PTLAR 0x30
#define ILI9341_MADCTL 0x36
#define ILI9341_IDMOFF 0x38
#define ILI9341_IDMON 0x39
#define ILI9341_PIXFMT 0x3A
#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR 0xB4
#define ILI9341_DFUNCTR 0xB6
#define ILI9341_PWCTR1 0xC0
#define ILI9341_PWCTR2 0xC1
#define ILI9341_PWCTR3 0xC2
#define ILI9341_PWCTR4 0xC3
#define ILI9341_PWCTR5 0xC4
#define ILI9341_VMCTR1 0xC5
#define ILI9341_VMCTR2 0xC7
#define ILI9341_RDID1 0xDA
#define ILI9341_RDID2 0xDB
#define ILI9341_RDID3 0xDC
#define ILI9341_RDID4 0xDD
#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
#define ILI9341_IFCTL 0xF6

static const uint8_t init_commands[] = {
    //4, 0xEF, 0x03, 0x80, 0x02,
    //4, 0xCF, 0x00, 0XC1, 0X30,
    //5, 0xED, 0x64, 0x03, 0X12, 0X81,
    //4, 0xE8, 0x85, 0x00, 0x78,
    //6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
    //2, 0xF7, 0x20,
    //3, 0xEA, 0x00, 0x00,
    //2, ILI9341_PWCTR1, 0x23, // Power control
    //2, ILI9341_PWCTR2, 0x10, // Power control
    //3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
    //2, ILI9341_VMCTR2, 0x86, // VCM control2
    //2, ILI9341_MADCTL, 0x48, // Memory Access Control
    2, ILI9341_PIXFMT, 0x55,
    //3, ILI9341_FRMCTR1, 0x00, 0x18,
    //4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
    //2, 0xF2, 0x00, // Gamma Function Disable
    2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
    16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
    16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    0
};

Display::Display(int resetPin, int dcPin, int spi):
    resetPin(resetPin),
    dcPin(dcPin),
    spi(spi)
{
    pinMode(resetPin, OUTPUT);
    pinMode(dcPin, OUTPUT);
    flag_command();
    hardware_reset();
    delay(20);
    display_on();
    partial_mode_off(); 

    const uint8_t* cmds = init_commands;
    while (true) {
        uint8_t count = *(cmds++);
        if (count == 0) break;
        count--;
        auto cmd = *(cmds++);
        if (count == 0)
        {
            perform_cmd(cmd);
        }
        else
        {
            perform_write(cmd, cmds, count);
            cmds += count;
        }
    }
    
    sleep_out();
}

uint16_t Display::width()
{
	return 240u;
}

uint16_t Display::height()
{
	return 320u;
}

void Display::flag_command()
{
    digitalWrite(dcPin, LOW);
}

void Display::flag_data()
{
    digitalWrite(dcPin, HIGH);
}

void Display::hardware_reset()
{
    digitalWrite(resetPin, LOW);
    delay(20);
    digitalWrite(resetPin, HIGH);
    delay(20);    
}

void Display::perform_cmd(const uint8_t cmd)
{
    spi_write(spi, &cmd, 1);
}

void Display::perform_xfer(const uint8_t cmd, void* data, uint32_t len)
{
    spi_write(spi, &cmd, 1);
    flag_data();
    spi_xfer(spi, data, len);
    flag_command();
}

void Display::perform_write(const uint8_t cmd, const void* data, uint32_t len)
{
    spi_write(spi, &cmd, 1);   
    flag_data();
    spi_write(spi, data, len);
    flag_command();
}

void Display::enter_sleep_mode()
{
    perform_cmd(ILI9341_SLPIN);
}

void Display::sleep_out()
{
    perform_cmd(ILI9341_SLPOUT);
}

void Display::partial_mode_on()
{
    perform_cmd(ILI9341_PTLON);
}

void Display::partial_mode_off()
{
    perform_cmd(ILI9341_NORON);
}

void Display::display_off()
{
    perform_cmd(ILI9341_DISPOFF);
}

void Display::display_on()
{
    perform_cmd(ILI9341_DISPON);
}

void Display::column_address_set(uint16_t sc, uint16_t ec)
{
    const uint8_t data[] = {HIBYTE(sc), LOBYTE(sc), HIBYTE(ec), LOBYTE(ec)};
    perform_write(ILI9341_CASET, data, 4);
}
  
void Display::page_address_set(uint16_t sc, uint16_t ec)
{
    const uint8_t data[] = {HIBYTE(sc), LOBYTE(sc), HIBYTE(ec), LOBYTE(ec)};
    perform_write(ILI9341_PASET, data, 4);
}

void Display::pixel_format_set(const uint8_t format)
{
    perform_write(ILI9341_PIXFMT, &format, 1);
}

#define SPI_BLOCK_SIZE 4096u

void Display::memory_write(const void* data, uint32_t len)
{
	perform_write(ILI9341_RAMWR, data, std::min(SPI_BLOCK_SIZE, len));

	for (uint32_t pos = std::min(SPI_BLOCK_SIZE, len);
		pos < len;
		pos += SPI_BLOCK_SIZE)
	{
		memory_write_continue(static_cast<const uint8_t*>(data) + pos, std::min(SPI_BLOCK_SIZE, len - pos));
	}
}

void Display::memory_read(void* data, uint32_t len)
{
	perform_xfer(ILI9341_RAMRD, data, std::min(SPI_BLOCK_SIZE, len));

	for (uint32_t pos = std::min(SPI_BLOCK_SIZE, len);
		pos < len;
		pos += SPI_BLOCK_SIZE)
	{
		memory_read_continue(static_cast<uint8_t*>(data) + pos, std::min(SPI_BLOCK_SIZE, len - pos));
	}
}

void Display::memory_write_continue(const void* data, uint32_t len)
{
    perform_write(0x3c, data, len);
}

void Display::memory_read_continue(void* data, uint32_t len)
{
    perform_xfer(0x3e, data, len);
}
