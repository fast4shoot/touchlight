#include "drawing.hpp"

#include <vector>

Color::Color():
	color(0)
{}

Color::Color(uint8_t r, uint8_t g, uint8_t b):
	color((g >> 5) | (b & 0xf8) | ((r & 0xf8) << 5) | ((g & 0x1c) << 11)) // absolutely disgusting
{}

LocalDrawing::LocalDrawing(uint16_t w, uint16_t h, Color fill):
	data(w * h, fill),
	width(w),
	height(h)
{}

void LocalDrawing::draw_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t line_width)
{
	for (uint16_t line = 0; line < line_width; line++)
	{
		for (uint16_t col = x; col < x + w; col++)
		{
			data[(y + line) * width + col] = color;
			data[(y + h - line - 1) * width + col] = color;
		}
		
		for (uint16_t row = line_width + y; row < y + h - line_width; row++)
		{
			data[row * width + x + line] = color;
			data[row * width + x + w - line - 1] = color;
		}
	}
}

void LocalDrawing::draw_filled_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	for (uint16_t row = y; row < y + h; row++)
	for (uint16_t col = x; col < x + w; col++)
	{
		data[row * width + col] = color;
	}
}

void Drawing::push_drawing(uint16_t x, uint16_t y, const LocalDrawing& drawing)
{
	display.column_address_set(x, x + drawing.width - 1);
	display.page_address_set(y, y + drawing.height - 1);
	display.memory_write(drawing.data.data(), drawing.data.size() * sizeof(Color));
}

PressState Drawing::get_press_state(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	bool was_inside = last_touch_point.valid && last_touch_point.x >= x && last_touch_point.x < x + w && last_touch_point.y >= y && last_touch_point.y < y + h; 
	bool is_inside = touch_point.valid && touch_point.x >= x && touch_point.x < x + w && touch_point.y >= y && touch_point.y < y + h;
	
	PressState ret;
	ret.pressed = !was_inside && is_inside;
	ret.depressed = was_inside && !is_inside;
	ret.down = is_inside;
	ret.up = !is_inside;
	return ret;
}

Drawing::Drawing(Display& display, Touch& touch):
	display(display),
	touch(touch),
	background(0xff, 0xff, 0xff)
{
}

void Drawing::begin_frame()
{
	last_touch_point = touch_point;
	touch_point = touch.run();
}

void Drawing::clear()
{
	draw_filled_rectangle(background, 0, 0, display.width(), display.height());
}

void Drawing::draw_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t line_width)
{
	LocalDrawing drawing{w, h, background};
	drawing.draw_rectangle(color, 0, 0, w, h, line_width);
	push_drawing(x, y, drawing);
}

void Drawing::draw_filled_rectangle(Color color, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	push_drawing(x, y, LocalDrawing{w, h, color});
}

PressState Drawing::draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const std::string& str)
{
	static const Color inactive_border_color(23, 220, 255);
	static const Color active_border_color(19, 182, 211);
	static const Color inactive_fill_color(171, 199, 206);
	static const Color active_fill_color(133, 155, 159);
	
	auto state = get_press_state(x, y, w, h);
	if (state.pressed)
	{
		LocalDrawing drawing{w, h, active_fill_color};
		drawing.draw_rectangle(active_border_color, 0u, 0u, w, h, 2u);
		push_drawing(x, y, drawing);
	}
	else if (state.depressed)
	{
		LocalDrawing drawing{w, h, inactive_fill_color};
		drawing.draw_rectangle(inactive_border_color, 0u, 0u, w, h, 2u);
		push_drawing(x, y, drawing);
	}
	
	return state;
}
