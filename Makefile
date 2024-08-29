## VER 4
CXX = g++
TARGET = main
SOURCEDIRS = src

FIXPATH = $(subst /,\,$1)

SRC		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS))) 
OBJ = $(SRC:.cpp=.o)

CXXFLAGS = -g -Wall

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	del $(call FIXPATH,$(OBJ)) main.exe