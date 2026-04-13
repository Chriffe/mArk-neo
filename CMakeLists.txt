# Task Viewer — ESP-IDF project for CrowPanel Advanced 10.1" (ESP32-P4)
# Requires ESP-IDF v5.4.2 or higher
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Include Elecrow BSP peripheral components
set(EXTRA_COMPONENT_DIRS "peripheral")

project(task_viewer)
