
#include "myacross.h"



void drawLine(uint8_t* img, cv::Point pt1, cv::Point pt2, uint8_t color) {
    // Bresenham's line algorithm实现
    int width = image_w;
    int height = image_h;
	
    int x1 = pt1.x;
    int y1 = pt1.y;
    int x2 = pt2.x;
    int y2 = pt2.y;
    
    bool steep = std::abs(y2 - y1) > std::abs(x2 - x1);
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    
    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    int dx = x2 - x1;
    int dy = std::abs(y2 - y1);
    int error = dx / 2;
    int ystep = (y1 < y2) ? 1 : -1;
    int y = y1;
    
    for (int x = x1; x <= x2; x++) {
        if (steep) {
            if (y >= 0 && y < width && x >= 0 && x < height) {
                img[y * height + x] = color;
            }
        } else {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                img[y * width + x] = color;
            }
        }
        
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}








static int16 limit_a_b(int16 x, int a, int b)
{
	if (x < a) x = a;
	if (x > b) x = b;
	return x;
}
/** 
* @brief 最小二乘法
* @param uint16 begin				输入起点
* @param uint16 end					输入终点
* @param uint16 *border				输入需要计算斜率的边界首地址
*  @see CTest		Slope_Calculate(start, end, border);//斜率
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
*/
float Slope_Calculate(uint16 begin, uint16 end, uint16 *border)
{
	float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
	int16 i = 0;
	float result = 0;
	static float resultlast;

	for (i = begin; i < end; i++)
	{
		xsum += i;
		ysum += border[i];
		xysum += i * (border[i]);
		x2sum += i * i;

	}
	if ((end - begin)*x2sum - xsum * xsum) //判断除数是否为零
	{
		result = ((end - begin)*xysum - xsum * ysum) / ((end - begin)*x2sum - xsum * xsum);
		resultlast = result;
	}
	else
	{
		result = resultlast;
	}
	return result;
}

/** 
* @brief 计算斜率截距
* @param uint16 start				输入起点
* @param uint16 end					输入终点
* @param uint16 *border				输入需要计算斜率的边界
* @param float *slope_rate			输入斜率地址
* @param float *intercept			输入截距地址
*  @see CTest		calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
*/
void calculate_s_i(uint16 start, uint16 end, uint16 *border, float *slope_rate, float *intercept)
{
	uint16 i, num = 0;
	uint16 xsum = 0, ysum = 0;
	float y_average, x_average;

	num = 0;
	xsum = 0;
	ysum = 0;
	y_average = 0;
	x_average = 0;
	for (i = start; i < end; i++)
	{
		xsum += i;
		ysum += border[i];
		num++;
	}

	//计算各个平均数
	if (num)
	{
		x_average = (float)(xsum / num);
		y_average = (float)(ysum / num);

	}

	/*计算斜率*/
	*slope_rate = Slope_Calculate(start, end, border);//斜率
	*intercept = y_average - (*slope_rate)*x_average;//截距
}

/** 
* @brief 十字补线函数
* @param uint8(*image)[image_w]		输入二值图像
* @param uint16 *l_border			输入左边界首地址
* @param uint16 *r_border			输入右边界首地址
* @param uint16 total_num_l			输入左边循环总次数
* @param uint16 total_num_r			输入右边循环总次数
* @param uint16 *dir_l				输入左边生长方向首地址
* @param uint16 *dir_r				输入右边生长方向首地址
* @param uint16(*points_l)[2]		输入左边轮廓首地址
* @param uint16(*points_r)[2]		输入右边轮廓首地址
*  @see CTest		cross_fill(image,l_border, r_border, data_statics_l, data_statics_r, dir_l, dir_r, points_l, points_r);
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
 */
void cross_fill(uint8(*image)[image_w], uint16 *l_border, uint16 *r_border, uint16 total_num_l, uint16 total_num_r,
										 uint16 *dir_l, uint16 *dir_r, uint16(*points_l)[2], uint16(*points_r)[2])
{
	uint16 i;
	uint16 break_num_l = 0;
	uint16 break_num_r = 0;
	uint16 start, end;
	float slope_l_rate = 0, intercept_l = 0;
	//出十字
	for (i = 1; i < total_num_l; i++)
	{
		if (dir_l[i - 1] == 4 && dir_l[i] == 4 && dir_l[i + 3] == 6 && dir_l[i + 5] == 6 && dir_l[i + 7] == 6)
		{
			break_num_l = points_l[i][1];//传递y坐标
			printf("brea_knum-L:%d\n", break_num_l);
			printf("I:%d\n", i);
			printf("十字标志位：1\n");
			break;
		}
	}
	for (i = 1; i < total_num_r; i++)
	{
		if (dir_r[i - 1] == 4 && dir_r[i] == 4 && dir_r[i + 3] == 6 && dir_r[i + 5] == 6 && dir_r[i + 7] == 6)
		{
			break_num_r = points_r[i][1];//传递y坐标
			printf("brea_knum-R:%d\n", break_num_r);
			printf("I:%d\n", i);
			printf("十字标志位：1\n");
			break;
		}
	}
	if (break_num_l&&break_num_r&&image[image_h - 1][4] && image[image_h - 1][image_w - 4])//两边生长方向都符合条件
	{
		//计算斜率
		start = break_num_l - 15;
		start = limit_a_b(start, 0, image_h);
		end = break_num_l - 5;
		calculate_s_i(start, end, l_border, &slope_l_rate, &intercept_l);
		printf("slope_l_rate:%f\nintercept_l:%f\n", slope_l_rate, intercept_l);
		for (i = break_num_l - 5; i < image_h - 1; i++)
		{
			l_border[i] = slope_l_rate * (i)+intercept_l;//y = kx+b
			l_border[i] = limit_a_b(l_border[i], border_min, border_max);//限幅
		}

		//计算斜率
		start = break_num_r - 15;//起点
		start = limit_a_b(start, 0, image_h);//限幅
		end = break_num_r - 5;//终点
		calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
		printf("slope_l_rate:%f\nintercept_l:%f\n", slope_l_rate, intercept_l);
		for (i = break_num_r - 5; i < image_h - 1; i++)
		{
			r_border[i] = slope_l_rate * (i)+intercept_l;
			r_border[i] = limit_a_b(r_border[i], border_min, border_max);
		}


	}

}

