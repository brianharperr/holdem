#!/bin/sh
g++ -L../../bin gui.cpp `wx-config --cxxflags --libs` -o main -ltex
