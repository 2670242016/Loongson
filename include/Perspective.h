
#pragma once

#include "common_system.h"
#include "common_program.h"
#include "AAAdefine.h"

#define USED_ROW    image_h  //用于透视图的行列
#define USED_COL    image_w
#define ImageUsed   *PerImg_ip//*PerImg_ip定义使用的图像，ImageUsed为用于巡线和识别的图像s

void ImagePerspective_Init(Img_Store *Img_Store_p, double change_un_Mat[3][3]) ;
void ApplyInversePerspective(Img_Store *Img_Store_p) ;
void ApplyInversePerspectiveAll(Img_Store *Img_Store_p) ;
cv::Point PointMap(cv::Point localPoint);
bool invertMatrix(double mat[3][3], double inv[3][3]);