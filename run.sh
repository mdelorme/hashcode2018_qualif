#!/bin/bash

g++ -O3 -g -Wall -std=c++11 -o rides rides.cpp

echo "case A"
./rides a_example.in > a.out
echo "case B"
./rides b_should_be_easy.in > b.out
echo "case C"
./rides c_no_hurry.in > c.out
echo "case D"
./rides d_metropolis.in > d.out
echo "case E"
./rides e_high_bonus.in > e.out
