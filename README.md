YOLOv8目标检测仓库

1.项目代码介绍

src/main.cpp ：主程序运行文件

src/postprocess.cpp: 模型推理后的后处理代码

src/yolov8.cpp：模型初始化、推理、反初始化等函数代码

include/postprocess.h、yolov8.h：各函数声明

2.配置文件介绍

yolov8/3rdparty 中是第三方库

yolov8/build 是编译位置

yolov8/inputimage 是输入图片所在文件夹

yolov8/outputimage 是输出图片所在文件夹

yolov8/model 是RKNN模型以及标签名txt文件所在文件夹

yolov8/rknn_lib 是瑞芯微官方动态库librknnrt.so所在位置

3.编译运行

在编译前需在源文件中全局搜索 “*****” ，为路径修改标记

**①cd build**

**②cmake ..**

**③make**

**④./rknn_yolov8_demo**





CSDN地址：[【YOLOv8部署至RK3588】模型训练→转换RKNN→开发板部署_yolov8转rknn-CSDN博客](https://blog.csdn.net/A_l_b_ert/article/details/141610417?spm=1001.2014.3001.5502)

QQ咨询（not free）：2506245294
