#!/bin/bash

if [ -r RayTracer.cpp ]
then
    clang++ -Xpreprocessor -fopenmp -lomp RayTracer.cpp -w `sdl2-config --cflags --libs` -O3 -o output/rt.app

    if [ -x output/rt.app ]
    then
        echo "Running..."
        ./output/rt.app
    else
        echo "File not found"
    fi
else 
    echo "File not found or permission denied"
fi

