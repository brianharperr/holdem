CC = g++
LDFLAGS = -L../../bin `wx-config --cxxflags --libs` -ltex
TARGET_EXE = holdemGUI

SRCS = gui.cpp

.PHONY: all
all: ${TARGET_EXE}

$(TARGET_EXE): $(SRCS)
	$(CC) $(LDFLAGS) -o holdemGUI $(SRCS)
