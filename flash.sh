#!/bin/bash
# 请根据你的串口号修改 /dev/ttyUSB0（Windows 是 COM3/COM4）
idf.py -p /dev/ttyUSB0 flash monitor
