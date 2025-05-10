#include "display_show.h"

/**
 * @brief 将OpenCV的Mat图像显示到IPS200屏幕上
 * @param img 输入的图像（支持BGR或灰度格式）
 * @note 图像将被自动缩放以适应屏幕分辨率（假设屏幕分辨率为IPS200_WIDTH x IPS200_HEIGHT）
 */
void displayMatOnIPS200(const cv::Mat& img) {
    
    // 检查输入图像是否有效
    if (img.empty()) {
        return;
    }
    
    // 转换图像颜色空间（BGR -> RGB565）并缩放尺寸
    cv::Mat resizedImg;
    cv::resize(img, resizedImg, cv::Size(240, 180));
    
    // 根据输入图像类型处理
    for (uint16_t y = 0; y < 180; ++y) {
        for (uint16_t x = 0; x < 240; ++x) {
            cv::Vec3b pixel = resizedImg.at<cv::Vec3b>(y, x);
            
            // 将BGR转换为RGB565 (5-6-5格式)
            uint16_t color = ((pixel[2] >> 3) << 11) |  // R
                             ((pixel[1] >> 2) << 5)  |  // G
                             (pixel[0] >> 3);           // B
            
            // 绘制像素点
            ips200_draw_point(x, y, color);
        }
    }
}