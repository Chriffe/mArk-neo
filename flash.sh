#!/bin/zsh
source ~/esp/esp-idf/export.sh
cd /Users/theswedishmaker/daily-scroll/esp32-project
idf.py -p /dev/cu.wchusbserial10 flash monitor
