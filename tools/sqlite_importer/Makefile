# SQlite CyPhOS Benchmark reader

CXXFLAGS:=-O3 --std=c++14 

# Include main CyPhOS directory
CXXFLAGS += -I../..

# Include sqlite
CXXFLAGS += $$(pkg-config --libs --cflags sqlite3)
CC:=gcc
CXX:=g++

all: mandel

SOURCE_FILES := main.cc CyphosReader.cc DatabaseInterface.cc
HEADER_FILES := CyphosReader.h DatabaseInterface.h

mandel: $(SOURCE_FILES) $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) $(SOURCE_FILES) -o main


clean:
	rm main

