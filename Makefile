CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2
LDFLAGS = -lcpr -lcurl

SRCS = ConfigReader.cpp FastBuyer.cpp ParseSites.cpp TelegramHandling.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = FastBuyer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
