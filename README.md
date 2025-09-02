# YOLOv8_RK3588_object_detect

CSDN地址：[【YOLOv8部署至RK3588】模型训练→转换RKNN→开发板部署](https://blog.csdn.net/A_l_b_ert/article/details/141610417?spm=1001.2014.3001.5502)

QQ咨询（not free）：2506245294

# 目标检测仓库

切记：一定要在RK系列开发板上运行，不要在虚拟机上跑，ARM64和X86不一样！

1.项目代码介绍

src/main.cpp ：主程序运行文件

src/postprocess.cpp: 模型推理后的后处理代码

src/yolov8.cpp：模型初始化、推理、反初始化等函数代码

include/postprocess.h、yolov8.h：各函数声明

2.配置文件介绍

python_script/jpg2png.py是jpg图片转png格式的脚本，直接使用jpg图片读取会失败，所以先用该脚本转成png格式后再运行

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

此处统一说明：加QQ后直接说问题和小星星截图，对于常见的相同问题，很多都已在CSDN博客中提到了（RKNN转换流程是统一的，可去博主所有的RKNN相关博客下去翻评论），已在评论中详细解释过的问题，不予回复。
