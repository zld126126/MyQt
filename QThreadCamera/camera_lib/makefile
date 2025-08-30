# 编译目标：动态库
TARGET = libcamera.so
# 源文件
SRC = camera_process.c
# 编译选项：-fPIC（位置无关代码，动态库必需），-shared（生成动态库）
CFLAGS = -Wall -fPIC -shared
# 链接选项：链接JPEG库（-ljpeg）和V4L2相关库
LDFLAGS = -ljpeg

all: $(TARGET)

$(TARGET): $(SRC)
	gcc $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# 清理编译产物
clean:
	rm -f $(TARGET)
