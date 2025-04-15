CXX = g++
CXXFLAGS = -Iinclude -std=c++11
SOURCES = src/main.cpp src/sniffer.cpp src/scanner.cpp src/fraud_detector.cpp src/logger.cpp src/dashboard.cpp src/config.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = bin/CySecure

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)