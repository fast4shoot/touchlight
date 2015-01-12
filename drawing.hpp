#ifndef DRAWING_HPP
#define DRAWING_HPP

#include <string>
#include <vector>
#include "display.hpp"
#include "touch.hpp"

struct Color 
{
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    
    uint16_t color;
};

static_assert(sizeof(Color) == 2, "Color must be two bytes");

struct PressState
{
	bool pressed;
	bool depressed;
	bool down;
	bool up;
};

class LocalDrawing
{
public:
	std::vector<Color> data;
	uint16_t width;
	uint16_t height;

	LocalDrawing(uint16_t w, uint16_t h, Color fill);
	
	void draw_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t line_width = 1);
	void draw_filled_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
};

class Drawing
{
	Display& display;
	Touch& touch;
	Color background;
	TouchPoint touch_point;
	TouchPoint last_touch_point;
	
	void push_drawing(uint16_t x, uint16_t y, const LocalDrawing& drawing);
	PressState get_press_state(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	
public:
	Drawing(Display& display, Touch& touch);
	
	void begin_frame();
	void clear();
	void draw_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t line_width = 1);
	void draw_filled_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	PressState draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const std::string& str);
	
	inline void setBackground(Color color)
	{
		background = color;
	}
	
	inline Color getBackground()
	{
		return background;
	}
};

#endif
