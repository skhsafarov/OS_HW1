#!/bin/bash

make
rm output/*

for file in ../input/*
do
    if [ -f "$file" ]; then
        ./program "$file" "output/$(basename "$file")"
    fi
done

make clean
