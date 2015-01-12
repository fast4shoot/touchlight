#ifndef TOUCH_HPP
#define TOUCH_HPP

#include <cstdint>

struct TouchPoint
{
    uint16_t x;
    uint16_t y;
    bool valid;
};

class Touch
{
    int spi;
	
	uint16_t run(uint8_t a);
	
public:
    Touch(int spi);
    
    TouchPoint run();
};

#endif
