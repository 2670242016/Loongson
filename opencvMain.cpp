#include "stdio.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>
#include <cmath>		
#include <stdlib.h>

#include "opencv_Code.hpp"


int main()
{

	double res_angle, res_rSquared;
	// uint16 i;
	uint16 hightest = 0;//定义一个最高行，tip：这里的最高指的是y值的最小
    string path = "/home/fhfh/Desktop/test_vedio/test_4.mp4";
	// string path = "http://192.168.73.124:8080/?action=stream";
	// string path = "~/Desktop/test_vedio/test_5.mp4";
	
	VideoCapture cap(path);

	Mat img, GrayImg, bin_img;
	unsigned char grayArray[image_h][image_w];
	int ret;

	char input_chr = 's';
	size_t size;

	struct ChangePoint* changePoints = NULL;
	int changeCount = 0;

	namedWindow("最终", WINDOW_NORMAL);

	static int findhigh = 90;
	float Servo_P = 15.0/(float(image_w)/2.0);




	while(1)
	{
		ret = cap.read(img);

		// img = resizeImage(img);
		cropImage(img,image_h);
		// imwrite("test.jpg",img);

		if (ret == 0)
		{
			printf("视频播放完毕");
			return 0;
		}

		// bin_img = er_zhi(img);

		cvtColor(img, img, COLOR_BGR2GRAY);

		for (int i = 0; i < image_h; i++) {
			for (int j = 0; j < image_w; j++) {
				original_image[i][j] = img.at<unsigned char>(i, j);
			}
		}

		turn_to_bin();
		// ImagePerspective_Init();
		// imshow("nitou",Array_to_Mat(PerImg_ip));
		imshow("erzhi",Array_to_Mat(bin_image));
		image_filter(bin_image);
		image_draw_rectan(bin_image);

		// ni_tou();



		data_stastics_l = 0;
		data_stastics_r = 0;

		if (get_start_point(image_h - 2))
		{		
			search_l_r((uint16)USE_num, bin_image, &data_stastics_l, &data_stastics_r, 
			start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
			
			get_left(data_stastics_l);
			get_right(data_stastics_r);


		}
		cvtColor(img, img, COLOR_GRAY2BGR);
/*********************************************************************************************
 *******************************      ShuJvDaYin      ****************************************
 *********************************************************************************************/		

		printArrayAsCSV(dir_l,data_stastics_l);
		printArrayAsCSV(dir_r,data_stastics_r);

		filterData(dir_l,data_stastics_l);
		filterData(dir_r,data_stastics_r);

		printArrayAsCSV(dir_l,data_stastics_l);
		printArrayAsCSV(dir_r,data_stastics_r);

		findChangePoints(dir_l, data_stastics_l, &changePoints, &changeCount, 2); 


		for (int i = 0; i < changeCount; ++i) {
        	printf("突变范围: [%d] - [%d] (%d -> %d) %s\n",
			changePoints[i].startIndex,
			changePoints[i].endIndex,
			dir_l[changePoints[i].startIndex],
			dir_l[changePoints[i].endIndex],
			(changePoints[i].type == 1) ? "上升沿" : "下降沿");
			point.x = points_l[changePoints[i].startIndex][0];point.y = points_l[changePoints[i].startIndex][1];
			circle(img, point, 3, Scalar(0, 255, 0), FILLED);
    	}
		
		findChangePoints(dir_r, data_stastics_r, &changePoints, &changeCount, 2);
		for (int i = 0; i < changeCount; ++i) {
			printf("突变范围: [%d] - [%d] (%d -> %d) %s\n",
			changePoints[i].startIndex,
			changePoints[i].endIndex,
			dir_r[changePoints[i].startIndex],
			dir_r[changePoints[i].endIndex],
			(changePoints[i].type == 1) ? "上升沿" : "下降沿");
			point.x = points_r[changePoints[i].startIndex][0];point.y = points_r[changePoints[i].startIndex][1];
			circle(img, point, 3, Scalar(0, 255, 0), FILLED);
    }




/***********************************************************************************
 * ******************************  	DISPLAY   **************************************
 * *********************************************************************************/
		for (int i = 0; i < data_stastics_l; i++)
		{
			point.x = points_l[i][0];point.y = points_l[i][1];
			circle(img, point, 1, Scalar(0, 0, 255), FILLED);//显示起点
		}
		for (int i = 0; i < data_stastics_r; i++)
		{
			point.x = points_r[i][0];point.y = points_r[i][1];
			circle(img, point, 1, Scalar(255, 0, 0), FILLED);//显示起点
		}

		for (int i = hightest; i < image_h - 1; i++)
		{
			center_line[i] = (l_border[i] + r_border[i]) >> 1;//求中线

			point.x = center_line[i];	point.y = i;
			circle(img, point, 1, Scalar(255, 140, 0), FILLED);//显示起点 显示中线	
			point.x = l_border[i];	point.y = i;
			circle(img, point, 1, Scalar(0, 255, 255), FILLED);//显示起点 显示左边线
			point.x = r_border[i];	point.y = i;
			circle(img, point, 1, Scalar(0, 255, 255), FILLED);//显示起点 显示右边线
		}
		printArrayAsCSV(center_line,image_h);

/*********************************************************************************************
 *******************************    NiHeZhongXian     ****************************************
 *********************************************************************************************/
		
		vector<cv::Point> points(image_h-hightest-1);
		for (int i = 0; i < image_h-hightest-1; i++)
		{
			points[i-hightest].x = center_line[i];
			points[i-hightest].y = i;
		}
		// int find_last;
		// if ((findhigh-hightest)<0 || (findhigh-hightest)>(image_h-hightest-1)) find_last = hightest;
		// else find_last = findhigh-hightest;
		servodegree((float(image_w)/2.0-float(points[hightest].x))*Servo_P);

/*
		vector<float> curvature = calculateCurvature(points);
		float servoAngle = calculateServoAngle(curvature);
		servodegree(servoAngle);
*/

/*
		string curveType = classifyCurve(points);
		cout << "Curve Type: " << curveType << endl;

		// 计算曲率
		vector<float> curvatures = computeCurvature(points);

		// 计算平均曲率
		float meanCurvature = 0.0f;
		for (float k : curvatures) meanCurvature += abs(k);
		meanCurvature /= curvatures.size();

		// 计算舵机角度
		float servoAngle = curvatureToServoAngle(meanCurvature);
		cout << "Servo Angle: " << servoAngle << " degrees " << meanCurvature << " 曲率" << endl;

*/

		/*
		fitLineAndCalculateAngle(points,res_angle,res_rSquared);
		int send_angle = mapDoubleToInt(res_angle,-50.0,50.0,0,30);
		printf("angle= %f ,R²= %f ,send_angle= %d ",res_angle,res_rSquared,send_angle);
		*/
		




		printf("\n/*********************************************************************************************/\n");
		imshow("最终", img);
		waitKey(0);
	}

/**
 * **********************************************************************************
 */
		

	destroyAllWindows();
	printf("程序结束\n");
	return 0;
}
