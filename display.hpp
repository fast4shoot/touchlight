#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdint>

class Display
{
    int resetPin;
    int dcPin;
    int spi;

private:
    void flag_command();
    void flag_data();
    void hardware_reset();
    void perform_cmd(const uint8_t cmd);
    void perform_xfer(const uint8_t cmd, void* data, uint32_t len);
    void perform_write(const uint8_t cmd, const void* data, uint32_t len);

public:
	Display(int resetPin, int dcPin, int spi);

	uint16_t width();
	uint16_t height();

    void enter_sleep_mode();
    void sleep_out();
    void partial_mode_on();
    void partial_mode_off();
    void display_off();
    void display_on();
    void column_address_set(uint16_t sc, uint16_t ec);
    void page_address_set(uint16_t sc, uint16_t ec);
    void pixel_format_set(const uint8_t format);
    void memory_write(const void* data, uint32_t len);
    void memory_read(void* data, uint32_t len);
    void memory_write_continue(const void* data, uint32_t len);
    void memory_read_continue(void* data, uint32_t len);
};

#endif
