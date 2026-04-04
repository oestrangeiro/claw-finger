#!/bin/bash

OUTPUT="claw-finger_X64"

gcc main.c src/*.c \
-Iincludes \
-Ivendor/raylib/include \
-Lvendor/raylib/lib \
-lraylib \
-lGL -lm -lpthread -ldl -lrt -lX11 \
-Wall -Wextra \
-o $OUTPUT && ./$OUTPUT
