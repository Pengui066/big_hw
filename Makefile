# ##  VER 1 
# hello: main.cpp factorial.cpp printhello.cpp
# 	g++ -o hello main.cpp factorial.cpp printhello.cpp 
#
#The first line means project file "hello" relies on those 3 cpp files after it.
#The next line is complie command, which should begin with a Tab

# ## VER 2
# CXX = g++
# TARGET = hello
# OBJ = main.o printhello.o factorial.o

# $(TARGET): $(OBJ)
# 	$(CXX) -o $(TARGET) $(OBJ)

# main.o: main.cpp
# 	$(CXX) -c main.cpp

# factorial.o: factorial.cpp
# 	$(CXX) -c factorial.cpp

# printhello.o: printhello.cpp
# 	$(CXX) -c printhello.cpp
#
# The target relies on those 3 .o files, if the compiler couldn't find the .o file
# it would execute the following commands. 
# The following commands informs the compiler that these .o files relies on their corresponding .cpp files, 
# and if a .cpp file is modified, the compiler would only re-compile that mmodified cpp file. 

# ## VER 3
# CXX = g++
# TARGET = hello
# OBJ = main.o printhello.o factorial.o

# CXXFLAGS = -c -Wall

# $(TARGET): $(OBJ)
# 	$(CXX) -o $@ $^
# ## $@ means the variable before the colon, which is $(TARGET)
# ## $^ means the variable after the colon, which are all of the dependencies, $(OBJ)

# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) $<
# ## $< means the first item of the dependencies

# .PHONY: clean
# clean:
# 	del *.o hello.exe

## VER 4
CXX = g++
TARGET = hello
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