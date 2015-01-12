LDLIBS += -lwiringPi
CXXFLAGS += -std=c++11 -O3 -Wall
PROGRAM = display
HEADERS = display.hpp drawing.hpp spi.hpp touch.hpp
SOURCES = display.cpp drawing.cpp spi.cpp touch.cpp main.cpp

default: $(PROGRAM)

$(PROGRAM): $(SOURCES:.cpp=.o)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	
$(SOURCES): $(HEADERS) 

clean:
	@rm $(SOURCES:.cpp=.o)
	@rm $(PROGRAM)

$.PHONY: clean
