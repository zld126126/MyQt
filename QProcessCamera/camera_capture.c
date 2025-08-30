//author:学生董格
//date:20250829
//command1:	sudo apt-get install libjpeg-dev v4l-utils
//command2:	gcc camera_capture.c -o camera_capture -ljpeg
//run:	./camera_capture

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <unistd.h>  // 新增：包含close()函数的声明

#define DEVICE "/dev/video0"
#define OUTPUT_FILE "capture.jpg"
#define WIDTH 640
#define HEIGHT 480

// 定义缓冲区结构
struct buffer {
    void   *start;
    size_t length;
};

static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static int fd = -1;

// JPEG错误处理结构
struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

// JPEG错误处理函数
METHODDEF(void)
my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

// 初始化摄像头设备
int init_device(const char *dev_name) {
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
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  // 常用的格式
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

// 将YUYV转换为RGB
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

// 保存RGB数据为JPEG
int save_jpeg(const unsigned char *rgb_data, int width, int height, const char *filename) {
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    FILE *outfile;
    JSAMPROW row_pointer[1];
    int row_stride;

    // 初始化JPEG压缩对象
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        return -1;
    }
    
    jpeg_create_compress(&cinfo);

    // 打开输出文件
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "无法打开输出文件 %s\n", filename);
        return -1;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    // 设置JPEG参数
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);  // 质量设置，1-100

    // 开始压缩
    jpeg_start_compress(&cinfo, TRUE);

    row_stride = width * 3;  // RGB每行的字节数

    // 逐行写入图像数据
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (JSAMPROW)&rgb_data[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // 完成压缩
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    return 0;
}

// 捕获一帧图像并保存
int capture_frame() {
    fd_set fds;
    struct timeval tv;
    int r;
    struct v4l2_buffer buf;
    unsigned char *rgb_data;

    // 等待数据就绪
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    tv.tv_sec = 2;
    tv.tv_usec = 0;

    r = select(fd + 1, &fds, NULL, NULL, &tv);
    if (r == -1) {
        if (errno == EINTR)
            return capture_frame();
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

    // 分配RGB缓冲区
    rgb_data = malloc(WIDTH * HEIGHT * 3);
    if (!rgb_data) {
        fprintf(stderr, "内存分配失败\n");
        return -1;
    }

    // 转换格式 YUYV -> RGB
    yuyv_to_rgb(buffers[buf.index].start, rgb_data, WIDTH, HEIGHT);

    // 保存为JPEG
    if (save_jpeg(rgb_data, WIDTH, HEIGHT, OUTPUT_FILE) != 0) {
        fprintf(stderr, "保存JPEG失败\n");
        free(rgb_data);
        return -1;
    }

    printf("成功保存截图到 %s\n", OUTPUT_FILE);

    // 释放资源
    free(rgb_data);

    // 重新入列缓冲区
    if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
        perror("VIDIOC_QBUF失败");
        return -1;
    }

    return 0;
}

// 清理资源
void cleanup() {
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

    // 关闭设备
    if (close(fd) == -1)
        perror("关闭设备失败");
}

int main() {
    // 初始化设备
    if (init_device(DEVICE) != 0) {
        fprintf(stderr, "初始化设备失败\n");
        return 1;
    }

    // 捕获一帧
    if (capture_frame() != 0) {
        fprintf(stderr, "捕获帧失败\n");
        cleanup();
        return 1;
    }

    // 清理资源
    cleanup();
    return 0;
}