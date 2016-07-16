#!/bin/bash

../bbst test_1000000.txt < input/Commands_1\ \ test_1000000.txt > actual_output/Commands_1\ \ test_1000000.txt
../bbst test_100.txt < input/Commands_1\ test_100.txt > actual_output/Commands_1\ test_100.txt
../bbst test_1000.txt < input/commands_1\ \ test_1000.txt > actual_output/commands_1\ \ test_1000.txt
../bbst test_1000.txt < input/commands\ test_1000\ .txt > actual_output/commands\ test_1000\ .txt
../bbst test_1000.txt < input/Commands_2\ \ test_1000.txt > actual_output/Commands_2\ \ test_1000.txt
../bbst test_100.txt < input/Commands_2\ test_100.txt > actual_output/Commands_2\ test_100.txt
