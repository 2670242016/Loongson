
#include "Perspective.h"
#include "common_system.h"
#include "common_program.h"

using namespace std;
using namespace cv;

std::vector<std::vector<InversePerspectiveMap>> inverse_mapping(RESULT_ROW,std::vector<InversePerspectiveMap>(RESULT_COL));
double inv_mat[3][3];

void ImagePerspective_Init(Img_Store *Img_Store_p, double change_un_Mat[3][3]) 
{
    std::vector<std::vector<InversePerspectiveMap>> mapping(RESULT_ROW, std::vector<InversePerspectiveMap>(RESULT_COL));

    
    if (!invertMatrix(change_un_Mat, inv_mat)) {
        // Handle matrix inversion failure
        std::cerr << "Error: Could not invert perspective transformation matrix" << std::endl;
        return;
    }

    for (int i = 0; i < RESULT_COL; i++) {
        for (int j = 0; j < RESULT_ROW; j++) {
            int local_x = (int)((change_un_Mat[0][0] * i
                    + change_un_Mat[0][1] * j + change_un_Mat[0][2])
                    / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j
                            + change_un_Mat[2][2]));
            int local_y = (int)((change_un_Mat[1][0] * i
                    + change_un_Mat[1][1] * j + change_un_Mat[1][2])
                    / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j
                            + change_un_Mat[2][2]));

            mapping[j][i] = {local_x, local_y};
        }
    }

    // for (int i = 0; i < RESULT_COL; i++) {
    //     for (int j = 0; j < RESULT_ROW; j++) {
    //         double denominator = (inv_mat[2][0] * i + inv_mat[2][1] * j + inv_mat[2][2]);
    //         int orig_x = (int)((inv_mat[0][0] * i + inv_mat[0][1] * j + inv_mat[0][2]) / denominator);
    //         int orig_y = (int)((inv_mat[1][0] * i + inv_mat[1][1] * j + inv_mat[1][2]) / denominator);

    //         inverse_mapping[j][i] = {orig_x, orig_y};
    //     }
    // }

    Img_Store_p->mapping = mapping;

}

Point PointMap(Point localPoint)
{
    Point backPoint;

    backPoint.x = inverse_mapping[localPoint.y][localPoint.x].local_x;
    backPoint.y = inverse_mapping[localPoint.y][localPoint.x].local_y;

    return backPoint;
}

void ApplyInversePerspectiveAll(Img_Store *Img_Store_p) 
{
    // Img_Store_p->Img_OTSU_Unpivot = Img_Store_p->Img_OTSU;
    Img_Store_p->Img_OTSU_Unpivot = cv::Mat(RESULT_ROW, RESULT_COL, CV_8UC1);
    for (int i = 0; i < RESULT_COL; i++) {
        for (int j = 0; j < RESULT_ROW; j++) {
            // 获取映射数组中的逆透视坐标
            int local_x = Img_Store_p->mapping[j][i].local_x;
            int local_y = Img_Store_p->mapping[j][i].local_y;

            inverse_mapping[local_y][local_x].local_x = i;
            inverse_mapping[local_y][local_x].local_y = j;

            // 检查坐标是否在有效范围内
            if (local_x >= 0 && local_y >= 0 && local_y < USED_ROW && local_x < USED_COL) {
                // 使用映射坐标从 Img_Color 获取像素值并存储到 Img_Color_Unpivot
                Img_Store_p->Img_Track_Unpivot.at<Vec3b>(j, i) = Img_Store_p->Img_Color.at<Vec3b>(local_y, local_x);
                Img_Store_p->Img_OTSU_Unpivot.at<uchar>(j, i) = Img_Store_p->Img_OTSU.at<uchar>(local_y, local_x);
            } else {
                // 如果映射坐标无效，则填充黑色像素
                Img_Store_p->Img_Track_Unpivot.at<Vec3b>(j, i) = Vec3b(0, 0, 0);
                Img_Store_p->Img_OTSU_Unpivot.at<uchar>(j, i) = 0;
            }
        }
    }
}

void ApplyInversePerspective(Img_Store *Img_Store_p) 
{
    static uint8 BlackColor = 0;

    for (int i = 0; i < RESULT_COL; i++) {
        for (int j = 0; j < RESULT_ROW; j++) {
            int local_x = Img_Store_p->mapping[j][i].local_x;
            int local_y = Img_Store_p->mapping[j][i].local_y;

            if (local_x >= 0 && local_y >= 0 && local_y < USED_ROW && local_x < USED_COL) {
                Img_Store_p->PerImg_ip[j][i] = Img_Store_p->bin_image[local_y][local_x];
            } else {
                Img_Store_p->PerImg_ip[j][i] = BlackColor;
            }
        }
    }

}

bool invertMatrix(double mat[3][3], double inv[3][3]) {
    // Compute determinant
    double det = mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) -
                 mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) +
                 mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);

    if (fabs(det) < 1e-6) // Matrix is singular
        return false;

    double inv_det = 1.0 / det;
    
    inv[0][0] = (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) * inv_det;
    inv[0][1] = (mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2]) * inv_det;
    inv[0][2] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) * inv_det;
    
    inv[1][0] = (mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2]) * inv_det;
    inv[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) * inv_det;
    inv[1][2] = (mat[0][2] * mat[1][0] - mat[0][0] * mat[1][2]) * inv_det;
    
    inv[2][0] = (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]) * inv_det;
    inv[2][1] = (mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1]) * inv_det;
    inv[2][2] = (mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]) * inv_det;

    return true;
}






