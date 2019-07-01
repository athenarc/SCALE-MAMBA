#!/bin/bash

./Player.x 0 Programs/naive_categorical1d -clients 1 -f 1 &
./Player.x 1 Programs/naive_categorical1d -clients 1 -f 1 &
./Player.x 2 Programs/naive_categorical1d -clients 1 -f 1
