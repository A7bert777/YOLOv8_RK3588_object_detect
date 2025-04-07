import cv2
import os

# 输入和输出目录
input_dir = "/home/firefly/GitHUb测试/YOLOv8_RK3588_object_detect/new"  # 存放.jpg图片的目录
output_dir = "/home/firefly/GitHUb测试/YOLOv8_RK3588_object_detect/inputimage"  # 存放.png图片的目录（自动创建）

# 确保输出目录存在
os.makedirs(output_dir, exist_ok=True)

# 遍历输入目录下的所有.jpg文件
for filename in os.listdir(input_dir):
    if filename.lower().endswith(('.jpg', '.jpeg')):
        # 读取.jpg图片
        img_path = os.path.join(input_dir, filename)
        img = cv2.imread(img_path)

        if img is not None:
            # 构造输出路径（替换扩展名为.png）
            output_path = os.path.join(output_dir, os.path.splitext(filename)[0] + ".png")

            # 保存为.png
            cv2.imwrite(output_path, img)
            print(f"Converted: {filename} -> {output_path}")
        else:
            print(f"Failed to read: {filename}")

print("All images converted!")
