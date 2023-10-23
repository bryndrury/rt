#!/bin/bash

clang++ -Xpreprocessor -fopenmp -lomp RayTracer.cpp -w `sdl2-config --cflags --libs` -O3 -o output/rt.app

./output/rt.app
