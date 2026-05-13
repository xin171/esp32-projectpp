# ESP32 编译仓库
用于编译生成 ESP32 bin 固件文件

## 编译环境
- ESP-IDF v5.x 推荐
- 目标芯片：ESP32

## 编译命令
```bash
# 编译（生成 bin）
./build.sh

# 清理
./clean.sh

# 烧录+串口监控
./flash.sh
