CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -O3

CXX_FLAGS_GDB := -Wall -Wextra -std=c++17 -ggdb

BIN		:= bin
SRC		:= src
INCLUDE		:= include
LIB		:= lib

LIBRARIES	:= -lpthread
EXECUTABLE	:= ssmaster

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES) 

clean:
	-rm $(BIN)/$(EXECUTABLE) & rm $(BIN)/*.txt
