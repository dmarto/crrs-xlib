TARGET=$(shell basename $(CURDIR))

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

CC=g++
CCFLAGS=-Wall -Ofast -Iinclude/
LDFLAGS= -lX11

all: $(TARGET)
.PHONY: all

$(TARGET): $(OBJECTS)
	@echo LD $@
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp %.h
	@echo CC $*.cpp
	@$(CC) $(CCFLAGS) -c $<

%.o: %.cpp
	@echo CC $*.cpp
	@$(CC) $(CCFLAGS) -c $<

clean:
	@rm -f $(OBJECTS) $(TARGET)
.PHONY: clean
