#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <unistd.h>

// 摄像头配置
#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
#define RGB_SIZE (WIDTH * HEIGHT * 3)  // RGB888格式数据大小

// 全局变量
static int fd = -1;
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static unsigned char *rgb_data = NULL;  // 用于存储RGB数据

// 缓冲区结构
struct buffer {
    void   *start;
    size_t length;
};

// YUYV转RGB转换函数
void yuyv_to_rgb(const unsigned char *yuyv, unsigned char *rgb, int width, int height) {
    int i, j;
    int y0, u, y1, v;
    int r, g, b;
    
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j += 2) {
            // 提取YUYV分量
            y0 = yuyv[2 * (i * width + j)];
            u = yuyv[2 * (i * width + j) + 1] - 128;
            y1 = yuyv[2 * (i * width + j) + 2];
            v = yuyv[2 * (i * width + j) + 3] - 128;
            
            // 转换第一个像素
            r = y0 + (int)(1.402f * v);
            g = y0 - (int)(0.344f * u + 0.714f * v);
            b = y0 + (int)(1.772f * u);
            
            rgb[3 * (i * width + j)] = (r > 255) ? 255 : ((r < 0) ? 0 : r);
            rgb[3 * (i * width + j) + 1] = (g > 255) ? 255 : ((g < 0) ? 0 : g);
            rgb[3 * (i * width + j) + 2] = (b > 255) ? 255 : ((b < 0) ? 0 : b);
            
            // 转换第二个像素
            r = y1 + (int)(1.402f * v);
            g = y1 - (int)(0.344f * u + 0.714f * v);
            b = y1 + (int)(1.772f * u);
            
            rgb[3 * (i * width + j + 1)] = (r > 255) ? 255 : ((r < 0) ? 0 : r);
            rgb[3 * (i * width + j + 1) + 1] = (g > 255) ? 255 : ((g < 0) ? 0 : g);
            rgb[3 * (i * width + j + 1) + 2] = (b > 255) ? 255 : ((b < 0) ? 0 : b);
        }
    }
}

// 初始化摄像头设备
static int init_device(const char *dev_name) {
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    unsigned int i;

    // 打开设备
    fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (fd == -1) {
        perror("无法打开设备");
        return -1;
    }

    // 检查设备是否支持视频捕获
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("VIDIOC_QUERYCAP失败");
        return -1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "设备不支持视频捕获\n");
        return -1;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "设备不支持流式捕获\n");
        return -1;
    }

    // 设置视频格式
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = WIDTH;
    fmt.fmt.pix.height      = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("VIDIOC_S_FMT失败");
        return -1;
    }

    // 请求缓冲区
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("VIDIOC_REQBUFS失败");
        return -1;
    }

    if (req.count < 2) {
        fprintf(stderr, "内存不足，无法分配缓冲区\n");
        return -1;
    }

    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        fprintf(stderr, "内存分配失败\n");
        return -1;
    }

    // 映射缓冲区
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
            perror("VIDIOC_QUERYBUF失败");
            return -1;
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, 
            PROT_READ | PROT_WRITE, MAP_SHARED, 
            fd, buf.m.offset);

        if (buffers[n_buffers].start == MAP_FAILED) {
            perror("mmap失败");
            return -1;
        }
    }

    // 队列缓冲区
    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF失败");
            return -1;
        }
    }

    // 开始捕获
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        perror("VIDIOC_STREAMON失败");
        return -1;
    }

    return 0;
}

// 清理摄像头资源
static void cleanup_device() {
    enum v4l2_buf_type type;
    unsigned int i;

    // 停止捕获
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
        perror("VIDIOC_STREAMOFF失败");

    // 解除映射
    for (i = 0; i < n_buffers; ++i)
        if (munmap(buffers[i].start, buffers[i].length) == -1)
            perror("munmap失败");

    free(buffers);
    buffers = NULL;
    n_buffers = 0;

    // 关闭设备
    if (close(fd) == -1)
        perror("关闭设备失败");
        
    fd = -1;
}

// 外部C接口实现

// 初始化摄像头
int camera_init() {
    // 分配RGB数据缓冲区
    rgb_data = (unsigned char *)malloc(RGB_SIZE);
    if (!rgb_data) {
        perror("RGB内存分配失败");
        return -1;
    }
    
    // 初始化摄像头设备
    return init_device(DEVICE);
}

// 捕获一帧（生成RGB数据）
int camera_capture() {
    fd_set fds;
    struct timeval tv;
    int r;
    struct v4l2_buffer buf;

    if (fd == -1 || !rgb_data) {
        fprintf(stderr, "摄像头未初始化\n");
        return -1;
    }

    // 等待摄像头数据就绪
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    r = select(fd + 1, &fds, NULL, NULL, &tv);
    if (r == -1) {
        if (errno == EINTR)
            return -1;
        perror("select失败");
        return -1;
    }

    if (r == 0) {
        fprintf(stderr, "捕获超时\n");
        return -1;
    }

    // 出列缓冲区
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
        perror("VIDIOC_DQBUF失败");
        return -1;
    }

    // 转换格式 YUYV -> RGB
    yuyv_to_rgb(buffers[buf.index].start, rgb_data, WIDTH, HEIGHT);

    // 重新入列缓冲区
    if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
        perror("VIDIOC_QBUF失败");
        return -1;
    }

    return 0;
}

// 获取RGB数据指针
unsigned char* get_rgb_data() {
    return rgb_data;
}

// 获取RGB数据长度
unsigned int get_rgb_data_len() {
    return RGB_SIZE;
}

// 释放RGB数据
void free_rgb_data() {
    if (rgb_data) {
        free(rgb_data);
        rgb_data = NULL;
    }
}

// 清理摄像头资源
void camera_cleanup() {
    cleanup_device();
    free_rgb_data();
}

