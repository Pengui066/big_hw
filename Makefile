## VER 4
CXX = g++
TARGET = big_hw
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

CXXFLAGS = -c -Wall

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $<

.PHONY: clean
clean:
	del *.o $(TARGET).exe