### 编译前确保你的Linux有以下编译工具和依赖
```
build-essential cmake
libusb-1.0-0-dev
```

### 编译/运行前执行以下命令
```
sudo ln lib/libECanVci.so /usr/lib/libECanVci.so.1
sudo ln lib/libusb.so /usr/lib/libusb.so
```

### 编译命令
```
mkdir build && cd build
cmake ..
make
```

### 执行测试
```
./motor_test 4 0 3 0x1400 2 3 1000
```