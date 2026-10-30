#!/bin/bash

MODE=$1

gcc -Wall -Wextra -D$MODE -DCHIP8_LEGACY -DNDEBUG -O3 -o speedtest $PWD/platform/speedtest.c
