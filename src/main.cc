#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yolov8.h"
#include "image_utils.h"
#include "file_utils.h"
#include "image_drawing.h"
#include <string> 
#include <chrono>  
#include <vector> 
#include <iostream>
#include <dirent.h> // For POSIX directory functions  
#include <sys/types.h>  
#include <unistd.h>  
#include <cstring> // For strcmp  
#include <opencv2/opencv.hpp>  


int read_image_opencv(const char* path, image_buffer_t* image) 
{
    cv::Mat cv_img = cv::imread(path, cv::IMREAD_COLOR);
    if (cv_img.empty()) return -1;

    // 统一转换为RGB格式
    cv::Mat rgb_img;
    if (cv_img.channels() == 4) 
    {
        cv::cvtColor(cv_img, rgb_img, cv::COLOR_BGRA2RGB);
    } 
    else 
    {
        cv::cvtColor(cv_img, rgb_img, cv::COLOR_BGR2RGB);
    }

    // 设置图像参数
    image->format = IMAGE_FORMAT_RGB888;
    image->width = rgb_img.cols;
    image->height = rgb_img.rows;
    
    // 复制数据
    int size = rgb_img.total() * rgb_img.channels();
    image->virt_addr = (unsigned char*)malloc(size);
    memcpy(image->virt_addr, rgb_img.data, size);
    
    return 0;
}

int write_image(const char* path, const image_buffer_t* img) 
{
    int width = img->width;
    int height = img->height;
    int channels = (img->format == IMAGE_FORMAT_RGB888) ? 3 : 
                   (img->format == IMAGE_FORMAT_GRAY8) ? 1 : 4;
    void* data = img->virt_addr;

    cv::Mat cv_img(height, width, CV_8UC(channels), data);
    cv::Mat bgr_img;

    // 始终将 RGB 转换为 BGR
    if (channels == 3 && img->format == IMAGE_FORMAT_RGB888) 
    {
        cv::cvtColor(cv_img, bgr_img, cv::COLOR_RGB2BGR);
    } 
    else 
    {
        bgr_img = cv_img; // 其他格式直接使用
    }

    bool success = cv::imwrite(path, bgr_img);
    return success ? 0 : -1;
}

//去除文件地址&后缀
std::string extractFileNameWithoutExtension(const std::string& path) 
{  
    auto pos = path.find_last_of("/\\");  
    std::string filename = (pos == std::string::npos) ? path : path.substr(pos + 1);  
      
    // 查找并去除文件后缀  
    pos = filename.find_last_of(".");  
    if (pos != std::string::npos) {  
        filename = filename.substr(0, pos);  
    }  
      
    return filename;  
}

// 处理一个文件夹中的所有图像文件  
void processImagesInFolder(const std::string& folderPath, rknn_app_context_t* rknn_app_ctx, const std::string& outputFolderPath) 
{  
    DIR *dir = opendir(folderPath.c_str());  
    if (dir == nullptr) {  
        perror("opendir");  
        return;  
    }  
  
    struct dirent *entry;  
    while ((entry = readdir(dir)) != nullptr) 
    {  
        std::string fileName = entry->d_name;  
        std::string fullPath = folderPath + "/" + fileName;  
         // 检查文件扩展名  
        if ((fileName.size() >= 4 && strcmp(fileName.c_str() + fileName.size() - 4, ".jpg") == 0) ||  
            (fileName.size() >= 5 && strcmp(fileName.c_str() + fileName.size() - 5, ".jpeg") == 0) ||  
            (fileName.size() >= 4 && strcmp(fileName.c_str() + fileName.size() - 4, ".png") == 0)) {  
  
            std::string outputFileName = outputFolderPath + "/" + extractFileNameWithoutExtension(fullPath) + "_out.png";  
  
            int ret;  
            image_buffer_t src_image;  
            memset(&src_image, 0, sizeof(image_buffer_t));  
 
            ret = read_image_opencv(fullPath.c_str(), &src_image); 

  
            if (ret != 0) {  
                printf("read image fail! ret=%d image_path=%s\n", ret, fullPath.c_str());  
                continue;  
            }  
  
            object_detect_result_list od_results;  
            
            ret = inference_yolov8_model(rknn_app_ctx, &src_image, &od_results);  
            if (ret != 0) {  
                printf("inference_yolov8_model fail! ret=%d\n", ret);  
                if (src_image.virt_addr != NULL) {  
                    free(src_image.virt_addr);  
                }  
                continue;  
            } 

            // 画框和概率  
            char text[256];  
            for (int i = 0; i < od_results.count; i++) 
            {  
                object_detect_result *det_result = &(od_results.results[i]);  
                printf("%s @ (%d %d %d %d) %.3f\n", coco_cls_to_name(det_result->cls_id),  
                       det_result->box.left, det_result->box.top,  
                       det_result->box.right, det_result->box.bottom,  
                       det_result->prop);  
                int x1 = det_result->box.left;  
                int y1 = det_result->box.top;  
                int x2 = det_result->box.right;  
                int y2 = det_result->box.bottom;  

                draw_rectangle(&src_image, x1, y1, x2 - x1, y2 - y1, COLOR_BLUE, 3);  
                sprintf(text, "%s %.1f%%", coco_cls_to_name(det_result->cls_id), det_result->prop * 100);  
                draw_text(&src_image, text, x1, y1 - 20, COLOR_RED, 10);  
            }  

            write_image(outputFileName.c_str(), &src_image);  

  
            if (src_image.virt_addr != NULL) 
            {  
                free(src_image.virt_addr);  
            }  
        }  
    }  
  
    closedir(dir);  
}   
  
int main(int argc, char **argv)  
{   
    // ***** 以下三个参数：rknn模型路径、输入图片所在文件夹路径、输出结果图片所在文件夹路径（均为绝对路径）
    const std::string modelPath = "/home/firefly/GitHUb测试/YOLOv8_RK3588_object_detect/model/500img_8_27yolov8relu.rknn";  
    const std::string imageFolder = "/home/firefly/GitHUb测试/YOLOv8_RK3588_object_detect/inputimage";  
    const std::string outputFolder = "/home/firefly/GitHUb测试/YOLOv8_RK3588_object_detect/outputimage"; 
    
    int ret;  
    rknn_app_context_t rknn_app_ctx;  
    memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t)); 

    init_post_process(); 

    ret = init_yolov8_model(modelPath.c_str(), &rknn_app_ctx);  
    if (ret != 0) 
    {  
        printf("init_yolov8_model fail! ret=%d model_path=%s\n", ret, modelPath.c_str());  
        return -1;  
    }      

    processImagesInFolder(imageFolder, &rknn_app_ctx, outputFolder); 

    ret = release_yolov8_model(&rknn_app_ctx); 

    if (ret != 0) 
    {  
        printf("release_yolov8_model fail! ret=%d\n", ret);  
    }  

    deinit_post_process();  

    return 0;  
}

