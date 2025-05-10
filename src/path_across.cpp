#include "common_system.h"
#include "common_program.h"
#include "myacross.h"

using namespace std;
using namespace cv;

/*
    AcrossTrack说明
    十字赛道
*/
void AcrossTrack(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
    ImgProcess ImgProcess;
	Point a,b;
	// 赛道二值化图像 赛道彩色图像
	if(Data_Path_p -> InflectionPointNum[0]-1 >= 2)
	{
		// 左边线中断点补线绘制：十字四点均存在
		a = Point((Data_Path_p -> InflectionPointCoordinate[0][0]),(Data_Path_p -> InflectionPointCoordinate[0][1]));
		b = Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1]));
		line((Img_Store_p -> Img_Track),a,b,Scalar(128,0,128),4);
		line((Img_Store_p -> Img_OTSU),a,b,Scalar(0),4);	
		// drawLine(Img_Store_p->bin_image[0],a,b);

		a = Point((Data_Path_p -> SideCoordinate_Eight[0][0]),(Data_Path_p -> SideCoordinate_Eight[0][1]));
		line((Img_Store_p -> Img_OTSU),a,b,Scalar(0),4);
		line((Img_Store_p -> Img_Track),a,b,Scalar(128,0,128),4);
		// drawLine(Img_Store_p->bin_image[0],a,b);
	}
	else
	{
		a = Point((Data_Path_p -> SideCoordinate_Eight[0][0]),(Data_Path_p -> SideCoordinate_Eight[0][1]));
		b = Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1]));
		// 左边线中断点补线绘制：十字只存在上两点
		line((Img_Store_p -> Img_OTSU),a,b,Scalar(0),4);
		line((Img_Store_p -> Img_Track),a,b,Scalar(128,0,128),4);
		// drawLine(Img_Store_p->bin_image[0],a,b);
	}

    if(Data_Path_p -> InflectionPointNum[1]-1 >= 2)
	{
		// 右边线中断点补线绘制：十字四点均存在
		a = Point((Data_Path_p -> InflectionPointCoordinate[0][2]),(Data_Path_p -> InflectionPointCoordinate[0][3]));
		b = Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3]));
		line((Img_Store_p -> Img_OTSU),a,b,Scalar(0),4);
		line((Img_Store_p -> Img_Track),a,b,Scalar(128,0,128),4);
		// drawLine(Img_Store_p->bin_image[0],a,b);

		a = Point((Data_Path_p -> SideCoordinate_Eight[0][2]),(Data_Path_p -> SideCoordinate_Eight[0][3]));
		b = Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3]));
		line((Img_Store_p -> Img_OTSU),a,b,Scalar(0),4);
		line((Img_Store_p -> Img_Track),a,b,Scalar(128,0,128),4);
		// drawLine(Img_Store_p->bin_image[0],a,b);	
	}
	else
	{
		// 右边线中断点补线绘制：十字只存在上两点
		a = Point((Data_Path_p -> SideCoordinate_Eight[0][2]),(Data_Path_p -> SideCoordinate_Eight[0][3]));
		b = Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3]));
		line((Img_Store_p -> Img_OTSU),a,b,Scalar(0),4);
		line((Img_Store_p -> Img_Track),a,b,Scalar(128,0,128),4);
		// drawLine(Img_Store_p->bin_image[0],a,b);
	}
}




