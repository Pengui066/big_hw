## VER 4
CXX = g++
TARGET = main
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

CXXFLAGS = -g -Wall

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	del *.o main.exe