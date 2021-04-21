#!/bin/sh
g++ -L../../src gui.cpp `wx-config --cxxflags --libs` -o main -ltex
