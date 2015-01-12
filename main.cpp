#include <algorithm>
#include <chrono>
#include <iostream>
#include <cstdint>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <softPwm.h>
#include <cmath>

#include "display.hpp"
#include "drawing.hpp"
#include "spi.hpp"
#include "touch.hpp"

#define RESET 0
#define DC 1
#define LED 7

static const Color WHITE = Color{0xff, 0xff, 0xff};

int main()
{
	wiringPiSetup();
	spi_init();
	int displaySpi = wiringPiSPISetup(0, SPI_SPEED);
	int touchSpi = wiringPiSPISetup(1, 1000000);
	
	//pwmSetClock(4096);
	pwmSetMode(PWM_MODE_MS);
	pinMode(LED, PWM_OUTPUT);
	pwmWrite(LED, 0);

	Display display(RESET, DC, displaySpi);
	Touch touch(touchSpi);
	Drawing drawing(display, touch);
	drawing.setBackground(Color(255, 255, 255));
	drawing.clear();

	float value = 0.0f;
	std::chrono::steady_clock::time_point time;
	for(unsigned i = 0;;i++)
	{
		auto last_time = time;
		time = std::chrono::steady_clock::now();
		auto delta_time = time - last_time;
		
		/*if (i % 1000 == 0)
		{
			auto lastTime = startTime;
			startTime = std::chrono::steady_clock::now();

			if (i != 0)
			{
				auto duration = startTime - lastTime;
				auto seconds = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1,1>>>(duration);
				std::cout 
					<< "Rendered 1000 frames in " 
					<< seconds.count() 
					<< " seconds (" 
					<< 1000 / seconds.count() 
					<< " FPS)" 
					<< std::endl;
			}
		}*/

		drawing.begin_frame();
		
		float diff = 0.0f;
		
		auto minusBtn = drawing.draw_button(20, 20, 90, 90, "-");
		auto plusBtn = drawing.draw_button(130, 20, 90, 90, "+");
		if (minusBtn.down)
		{
			diff -= delta_time.count();
		}
		
		if (plusBtn.down)
		{
			diff += delta_time.count();
		}
		
		if (diff != 0.0f)
		{
			uint16_t lastx = 23u + uint16_t(value * 194u);
			drawing.draw_filled_rectangle(Color{255, 255, 255}, lastx - 3, 130, 6, 12);
			
			value += diff * std::chrono::steady_clock::duration::period::num / std::chrono::steady_clock::duration::period::den;
			value = std::max(0.0f, std::min(1.0f, value));
			
			uint16_t newx = 23u + uint16_t(value * 194u);
			drawing.draw_filled_rectangle(Color{0, 0, 0}, newx - 3, 130, 6, 12);
			
			pwmWrite(LED, int(value * value * 1023));
		}
		
		delay(10);
	}

	return 0;
}
