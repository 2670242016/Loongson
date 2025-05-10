/*
#include "opencv_Code.hpp"


uint8 image_final[OUT_H][OUT_W];//逆变换图像
double map_square[CAMERA_H][CAMERA_W][2];//现实映射
int map_int[OUT_H][OUT_W][2];//图像映射

uint8 image_thereshold;//图像分割阈值

//图像数组

uint16 start_point_l[2] = { 0 };//左边起点的x，y值
uint16 start_point_r[2] = { 0 };//右边起点的x，y值
uint16 points_l[(uint16)USE_num][2] = { {  0 } };//左线
uint16 points_r[(uint16)USE_num][2] = { {  0 } };//右线
uint16 dir_r[(uint16)USE_num] = { 0 };//用来存储右边生长方向
uint16 dir_l[(uint16)USE_num] = { 0 };//用来存储左边生长方向

uint16 data_stastics_l = 0;//统计左边找到点的个数
uint16 data_stastics_r = 0;//统计右边找到点的个数
uint16 hightest = 0;//最高点

uint16 l_border[image_h];//左线数组
uint16 r_border[image_h];//右线数组
uint16 center_line[image_h];//中线数组

cv::Point point;

void ni_tou(void)
{
	double angle = 0.6;//俯仰角&&<1
	double dep = 5;//视点到面距离
	double prop_j = 1;//上下宽度矫正>1
	double prop_i = 0;//密度修正系数>-1&&<1
	double j_large = 1.4;//横向放大倍数
	uint16 i_abodon = 0;//上方舍弃的行数
	double hight = 20;//摄像头高度
	uint16 i;//y轴
	uint16 j;//x轴
	uint16 ii;
	double xg,yg;
	double x0,y0;
	double zt;
	double sin_a,cos_a;
	sin_a = sin(angle);
	cos_a = cos(angle);

	//初始化摄像头坐标系
	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			map_square[i][j][0] = ((float)CAMERA_H / 2 - (float)i + 0.5) / 10;
			map_square[i][j][1] = ((float)j - (float)CAMERA_W / 2 + 0.5) / 10;
		}
	}
	//横向拉伸
	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			map_square[i][j][1] = map_square[i][j][1] * (1 * (CAMERA_H - 1 - i) + (1 / prop_j) * i) / (CAMERA_H - 1);
		}
	}
	//逆透视变换
	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			xg = map_square[i][j][1];
			yg = map_square[i][j][0];
			y0 = (yg * dep + hight * cos_a * yg + hight * dep * sin_a) / (dep * cos_a - yg * sin_a);
			zt = -y0 * sin_a - hight * cos_a;
			x0 = xg * (dep - zt) / dep;
			map_square[i][j][1] = x0;
			map_square[i][j][0] = y0;
		}
	}
	double prop_x;//横坐标缩放比例
	prop_x = (OUT_W - 1) / (map_square[i_abodon][CAMERA_W - 1][1] - map_square[i_abodon][0][1]);
	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			map_square[i][j][1] *= prop_x;
			map_square[i][j][1] *= j_large;
			map_square[i][j][1] = map_square[i][j][1] + OUT_W / 2 - 0.5 * OUT_W / CAMERA_W;
		}
	}
	//前后方向
	double move_y;
	double prop_y;
	move_y = map_square[CAMERA_H - 1][0][0];
	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			map_square[i][j][0] -= move_y;
		}
	}
	prop_y = (OUT_H - 1) / map_square[i_abodon][0][0];
	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			map_square[i][j][0] *= prop_y;
			map_square[i][j][0] = OUT_H - OUT_H / CAMERA_H - map_square[i][j][0];
		}
	}
	//前后拉伸
	double dis_ever[CAMERA_H];
	double dis_add[CAMERA_H];
	double adjust_y[CAMERA_H];//每行的调值
	//计算每行宽度
	for (i = 0; i < CAMERA_H; i++)
	{
		dis_ever[i] = ((1 + prop_i) * (CAMERA_H - 1 - i) + (1 - prop_i) * i) / (CAMERA_H - 1);
	}
	dis_add[0] = 0;
	for (i = 0; i < CAMERA_H; i++)
	{
		if (i == 0)
		{
			dis_add[i] = 0;
		}
		else
		{
			dis_add[i] = dis_add[i - 1] + dis_ever[i - 1];
		}
	}
	adjust_y[0] = 1;
	for (i = 1; i < CAMERA_H; i++)
	{
		adjust_y[i] = dis_add[i] / i;
	}

	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			map_square[i][j][0] *= adjust_y[i];
		}
	}
	double y_fix;
	y_fix = (OUT_H - 1) / map_square[CAMERA_H - 1][0][0];
	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			map_square[i][j][0] *= y_fix;
		}
	}
	//逆映射-前后方向
	double fars;
	double far_min;
	int nears;
	for (i = 0; i < OUT_H; i++)
	{
		far_min = OUT_H;
		for (ii = 0; ii < CAMERA_H; ii++)
		{
			fars = (double)i - (double)(map_square[ii][CAMERA_H / 2][0]);
			if (fars < 0)
			{
				fars = -fars;
			}
			if (fars < far_min)
			{
				far_min = fars;
				nears = ii;
			}
		}
		for (j = 0; j < OUT_W; j++)
		{
			map_int[i][j][0] = nears;
		}

	}

	//左右方向
	int jj;
	double left_lim;
	double right_lim;
	for (i = 0; i < OUT_H; i++){
		//计算每一行取自哪一行
		ii = map_int[i][OUT_W / 2][0];
		left_lim = map_square[ii][0][1];
		right_lim = map_square[ii][CAMERA_W - 1][1];
		for (j = 0; j < OUT_W; j++)
		{
			if (j<left_lim - 1 || j>right_lim + 1)
			{
				map_int[i][j][1] = 255;
			}
			else
			{
				far_min = CAMERA_W;
				for (jj = 0; jj < CAMERA_W; jj++)
				{
					fars = (double)j - (double)(map_square[ii][jj][1]);
					if (fars < 0) fars = -fars;

					if (fars < far_min){
						far_min = fars;
						nears = jj;
					}
				}
				map_int[i][j][1] = nears;
			}
		}
	}
	//逆透视数据写入
	for (int i = 0; i < OUT_H; i++) {
		for (int j = 0; j < OUT_W; j++) {
			if (map_int[i][j][1] == 255) image_final[i][j] = 0x00; //无用位置
			else image_final[i][j] = bin_image[map_int[i][j][0]][map_int[i][j][1]];
		}
	}
}

void cropImage(cv::Mat& image, int cropHeight) {
    // 获取原图像的高度和宽度
    int originalHeight = image.rows;
    int originalWidth = image.cols;

    // 检查裁剪高度是否合理
    if (cropHeight >= originalHeight || cropHeight < 0) {
        std::cerr << "裁剪高度无效！" << std::endl;
        return;
    }

    // 计算裁剪区域的起始位置
    int top = originalHeight - cropHeight;

    // 使用ROI (Region Of Interest) 来裁剪图像
    cv::Rect roi(0, top, originalWidth, cropHeight);
    image = image(roi);  // 直接修改输入图像
}

int cyclicDiff(uint16 a, uint16 b) {
    int diff = a - b;
    if (diff > 3) return diff - 8; // 处理 7->0 的情况
    if (diff < -3) return diff + 8; // 处理 0->7 的情况
    return diff;
}

// 查找突变点
void findChangePoints(uint16* data, int size, struct ChangePoint** changePoints, int* changeCount, int threshold) {
    *changeCount = 0;  // 初始化突变点计数
    *changePoints = (struct ChangePoint*)malloc(size * sizeof(struct ChangePoint));  // 分配内存存储突变点

    int start = -1;  // 记录突变起点
    int isRising = 0; // 是否是上升沿（1是上升，-1是下降）

    for (int i = 1; i < size; ++i) {
        int diff = cyclicDiff(data[i], data[i - 1]); // 计算相邻点的循环差值

        if (abs(diff) >= threshold) {
            if (start == -1) {
                start = i - 1;  // 记录突变的起点
                isRising = (diff > 0) ? 1 : -1;  // 判断是上升沿还是下降沿
            }
        } else {
            if (start != -1) {
                // 记录突变点
                (*changePoints)[*changeCount].startIndex = start;
                (*changePoints)[*changeCount].endIndex = i - 1;
                (*changePoints)[*changeCount].type = isRising;
                (*changeCount)++;  // 增加突变点计数
                start = -1;  // 复位
            }
        }
    }

    // 若突变持续到最后，则记录最终突变点
    if (start != -1) {
        (*changePoints)[*changeCount].startIndex = start;
        (*changePoints)[*changeCount].endIndex = size - 1;
        (*changePoints)[*changeCount].type = isRising;
        (*changeCount)++;
    }
}

// 中值滤波函数
void medianFilter(std::vector<int>& data, int windowSize) {
    std::vector<int> temp = data;  // 复制原始数据
    int halfWin = windowSize / 2;

    for (size_t i = halfWin; i < data.size() - halfWin; ++i) {
        std::vector<int> window(data.begin() + i - halfWin, data.begin() + i + halfWin + 1);
        std::sort(window.begin(), window.end());
        temp[i] = window[window.size() / 2];  // 取中值
    }

    data = temp;  // 更新原始数据
}

// 峰值检测与去除（简单的局部极值检查）
void removePeaks(std::vector<int>& data, int threshold) {
    std::vector<int> temp = data;  // 复制数据
    for (size_t i = 1; i < data.size() - 1; ++i) {
        if (data[i] > data[i - 1] + threshold && data[i] > data[i + 1] + threshold) {
            temp[i] = (data[i - 1] + data[i + 1]) / 2;  // 用邻居均值替代
        }
    }
    data = temp;
}

// 主要滤波函数
void filterData(uint16* arr, int size)
{
    if (size < 3) return;  // 数据太短无法滤波

    std::vector<int> data(arr, arr + size);

    // 先进行中值滤波，去除 ±1 震荡
    medianFilter(data, 3);

    // 再去除峰值
    removePeaks(data, 2);  // 设定阈值 2，去除明显的孤立峰

    // 复制回原始数组
    for (int i = 0; i < size; ++i) {
        arr[i] = data[i];
    }
}

void printArrayAsCSV(const uint16 a[],size_t size) 
{
    for (size_t i = 0; i < size; ++i) {
        std::cout << a[i];
        if (i < size - 1) {
            std::cout << ",";
        }
    }
    std::cout << std::endl;
}

static int my_abs(int value)
{
	if (value >= 0) return value;
	else return -value;
}

// 限幅
static int16 limit_a_b(int16 x, int a, int b)
{
	if (x < a) x = a;
	if (x > b) x = b;
	return x;
}

// 获取图像
static void Get_image(uint8(*mt9v03x_image)[image_w])
{
	for (int i = 0; i < image_h; i++) {
		for (int j = 0; j < image_w; j++) {
			original_image[i][j] = mt9v03x_image[i][j];
		}
	}
	// printf("图像获取成功\n");
}

// 大津法（OTSU）是一种确定图像二值化分割阈值的算法
// 输入：图像数组，图像宽度，图像高度
static uint8 otsuThreshold(uint8* image, uint16 col, uint16 row)
{
	// printf("大津法阈值计算\n");
#define GrayScale 256
	uint16 Image_Width = col;
	uint16 Image_Height = row;
	int X; uint16 Y;
	uint8* data = image;
	int HistGram[GrayScale] = { 0 };

	uint32 Amount = 0;
	uint32 PixelBack = 0;
	uint32 PixelIntegralBack = 0;
	uint32 PixelIntegral = 0;
	int32 PixelIntegralFore = 0;
	int32 PixelFore = 0;
	double OmegaBack = 0, OmegaFore = 0, MicroBack = 0, MicroFore = 0, SigmaB = 0, Sigma = 0; // 类间方差;
	uint8 MinValue = 0, MaxValue = 0;
	uint8 Threshold = 0;


	for (Y = 0; Y < Image_Height; Y++) //Y<Image_Height改为Y =Image_Height；以便进行 行二值化
	{
		//Y=Image_Height;
		for (X = 0; X < Image_Width; X++)
		{
			HistGram[(int)data[Y * Image_Width + X]]++; //统计每个灰度值的个数信息
		}
	}




	for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++);        //获取最小灰度的值
	for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--); //获取最大灰度的值

	if (MaxValue == MinValue)
	{
		return MaxValue;          // 图像中只有一个颜色
	}
	if (MinValue + 1 == MaxValue)
	{
		return MinValue;      // 图像中只有二个颜色
	}

	for (Y = MinValue; Y <= MaxValue; Y++)
	{
		Amount += HistGram[Y];        //  像素总数
	}

	PixelIntegral = 0;
	for (Y = MinValue; Y <= MaxValue; Y++)
	{
		PixelIntegral += HistGram[Y] * Y;//灰度值总数
	}
	SigmaB = -1;
	for (Y = MinValue; Y < MaxValue; Y++)
	{
		PixelBack = PixelBack + HistGram[Y];    //前景像素点数
		PixelFore = Amount - PixelBack;         //背景像素点数
		OmegaBack = (double)PixelBack / Amount;//前景像素百分比
		OmegaFore = (double)PixelFore / Amount;//背景像素百分比
		PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
		PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
		MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
		MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
		if (Sigma > SigmaB)//遍历最大的类间方差g
		{
			SigmaB = Sigma;
			Threshold = (uint8)Y;
		}
	}
	// printf("计算完成,阈值为%d\n", Threshold);
	return Threshold;
}

// 二值化
static void turn_to_bin(void)
{
	// printf("图像二值化\n");
	
	image_thereshold = otsuThreshold(original_image[0], image_w, image_h);
	for (int i = 0;i < image_h;i++)
	{
		for (int j = 0;j < image_w;j++)
		{
			if (original_image[i][j] > image_thereshold)bin_image[i][j] = white_pixel;
			else bin_image[i][j] = black_pixel;
		}
	}
	// printf("图像二值化完成\n");
}

// 获取八邻域起始点
// 输入：起始行
static uint16 get_start_point(uint16 start_row)
{
	// printf("获取八邻域起始点\n");
	int i = 0, l_found = 0, r_found = 0;
	//清零
	start_point_l[0] = 0;//x
	start_point_l[1] = 0;//y

	start_point_r[0] = 0;//x
	start_point_r[1] = 0;//y

	//从中间往左边，先找起点
	for (i = image_w / 2; i > border_min; i--)
	{
		start_point_l[0] = i;//x
		start_point_l[1] = start_row;//y
		if (bin_image[start_row][i] == 255 && bin_image[start_row][i - 1] == 0)
		{
			// printf("找到左边起点image[%d][%d]\n", start_row,i);
			l_found = 1;
			break;
		}
	}

	for (i = image_w / 2; i < border_max; i++)
	{
		start_point_r[0] = i;//x
		start_point_r[1] = start_row;//y
		if (bin_image[start_row][i] == 255 && bin_image[start_row][i + 1] == 0)
		{
			// printf("找到右边起点image[%d][%d]\n",start_row, i);
			r_found = 1;
			break;
		}
	}

	if (l_found && r_found)return 1;
	else {
		//printf("未找到起点\n");
		return 0;
	}

	// printf("获取八邻域起始点完成\n");
}

// 左右巡线W
static void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16* l_stastic, uint16* r_stastic, uint16 l_start_x, uint16 l_start_y, uint16 r_start_x, uint16 r_start_y, uint16* hightest)
{

	uint16 i = 0, j = 0;

	//左边变量
	uint16 search_filds_l[8][2] = { {  0 } };
	uint16 center_point_l[2] = { 0 };
	uint16 index_l = 0;
	uint16 temp_l[8][2] = { {  0 } };
	uint16 l_data_statics;//统计左边
	//定义八个邻域
	static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
	//{-1,-1},{0,-1},{+1,-1},
	//{-1, 0},	     {+1, 0},
	//{-1,+1},{0,+1},{+1,+1},
	//这个是顺时针

	//右边变量
	uint16 search_filds_r[8][2] = { {  0 } };
	uint16 center_point_r[2] = { 0 };//中心坐标点
	uint16 index_r = 0;//索引下标
	uint16 temp_r[8][2] = { {  0 } };
	uint16 r_data_statics;//统计右边
	//定义八个邻域
	static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
	//{-1,-1},{0,-1},{+1,-1},
	//{-1, 0},	     {+1, 0},
	//{-1,+1},{0,+1},{+1,+1},
	//这个是逆时针

	l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
	r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

	//第一次更新坐标点  将找到的起点值传进来
	center_point_l[0] = l_start_x;//x
	center_point_l[1] = l_start_y;//y
	center_point_r[0] = r_start_x;//x
	center_point_r[1] = r_start_y;//y

	//开启邻域循环
	while (break_flag--)
	{

		//左边
		for (i = 0; i < 8; i++)//传递8F坐标
		{
			search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
			search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
		}
		//中心坐标点填充到已经找到的点内
		points_l[l_data_statics][0] = center_point_l[0];//x
		points_l[l_data_statics][1] = center_point_l[1];//y
		l_data_statics++;//索引加一

		//右边
		for (i = 0; i < 8; i++)//传递8F坐标
		{
			search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
			search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
		}
		//中心坐标点填充到已经找到的点内
		points_r[r_data_statics][0] = center_point_r[0];//x
		points_r[r_data_statics][1] = center_point_r[1];//y

		index_l = 0;//先清零，后使用
		for (i = 0; i < 8; i++)
		{
			temp_l[i][0] = 0;//先清零，后使用
			temp_l[i][1] = 0;//先清零，后使用
		}

		//左边判断
		for (i = 0; i < 8; i++)
		{
			if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
				&& image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
			{
				temp_l[index_l][0] = search_filds_l[(i)][0];
				temp_l[index_l][1] = search_filds_l[(i)][1];
				index_l++;
				dir_l[l_data_statics - 1] = (i);//记录生长方向
			}

			if (index_l)
			{
				//更新坐标点
				center_point_l[0] = temp_l[0][0];//x
				center_point_l[1] = temp_l[0][1];//y
				for (j = 0; j < index_l; j++)
				{
					if (center_point_l[1] > temp_l[j][1])
					{
						center_point_l[0] = temp_l[j][0];//x
						center_point_l[1] = temp_l[j][1];//y
					}
				}
			}

		}
		if ((points_r[r_data_statics][0] == points_r[r_data_statics - 1][0] && points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
			&& points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
			|| (points_l[l_data_statics - 1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics - 1][0] == points_l[l_data_statics - 3][0]
				&& points_l[l_data_statics - 1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 3][1]))
		{
			//printf("三次进入同一个点，退出\n");
			break;
		}
		if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
			&& my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
			)
		{
			//printf("\n左右相遇退出\n");	
			*hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
			//printf("\n在y=%d处退出\n",*hightest);
			break;
		}
		if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
		{
			// printf("\n如果左边比右边高了，左边等待右边\n");
			continue;//如果左边比右边高了，左边等待右边
		}
		if (dir_l[l_data_statics - 1] == 7
			&& (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
		{
			//printf("\n左边开始向下了，等待右边，等待中... \n");
			center_point_l[0] = points_l[l_data_statics - 1][0];//x
			center_point_l[1] = points_l[l_data_statics - 1][1];//y
			l_data_statics--;
		}
		r_data_statics++;//索引加一

		index_r = 0;//先清零，后使用
		for (i = 0; i < 8; i++)
		{
			temp_r[i][0] = 0;//先清零，后使用
			temp_r[i][1] = 0;//先清零，后使用
		}

		//右边判断
		for (i = 0; i < 8; i++)
		{
			if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
				&& image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
			{
				temp_r[index_r][0] = search_filds_r[(i)][0];
				temp_r[index_r][1] = search_filds_r[(i)][1];
				index_r++;//索引加一
				dir_r[r_data_statics - 1] = (i);//记录生长方向
				//printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
			}
			if (index_r)
			{

				//更新坐标点
				center_point_r[0] = temp_r[0][0];//x
				center_point_r[1] = temp_r[0][1];//y
				for (j = 0; j < index_r; j++)
				{
					if (center_point_r[1] > temp_r[j][1])
					{
						center_point_r[0] = temp_r[j][0];//x
						center_point_r[1] = temp_r[j][1];//y
					}
				}

			}
		}


	}


	//取出循环次数
	*l_stastic = l_data_statics;
	*r_stastic = r_data_statics;

}

// 获取左边界
static void get_left(uint16 total_L)
{
	uint16 i = 0;
	uint16 j = 0;
	uint16 h = 0;
	//初始化
	for (i = 0;i < image_h;i++)
	{
		l_border[i] = border_min;
	}
	h = image_h - 2;
	//左边
	for (j = 0; j < total_L; j++)
	{
		//printf("%d\n", j);
		if (points_l[j][1] == h)
		{
			l_border[h] = points_l[j][0] + 1;
		}
		else continue; //每行只取一个点，没到下一行就不记录
		h--;
		if (h == 0)
		{
			break;//到最后一行退出
		}
	}
}

// 获取右边界
static void get_right(uint16 total_R)
{
	uint16 i = 0;
	uint16 j = 0;
	uint16 h = 0;
	for (i = 0; i < image_h; i++)
	{
		r_border[i] = border_max;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
	}
	h = image_h - 2;
	//右边
	for (j = 0; j < total_R; j++)
	{
		if (points_r[j][1] == h)
		{
			r_border[h] = points_r[j][0] - 1;
		}
		else continue;//每行只取一个点，没到下一行就不记录
		h--;
		if (h == 0)break;//到最后一行退出
	}
}

// 形态学滤波 ， 简单来说就是膨胀和腐蚀的思想
static void image_filter(uint8(*bin_image)[image_w])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
	// printf("形态学滤波\n");
	uint16 i, j;
	uint32 num = 0;


	for (i = 1; i < image_h - 1; i++)
	{
		for (j = 1; j < (image_w - 1); j++)
		{
			//统计八个方向的像素值
			num =
				bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1]
				+ bin_image[i][j - 1] + bin_image[i][j + 1]
				+ bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];


			if (num >= threshold_max && bin_image[i][j] == 0)
			{

				bin_image[i][j] = 255;//白  可以搞成宏定义，方便更改

			}
			if (num <= threshold_min && bin_image[i][j] == 255)
			{

				bin_image[i][j] = 0;//黑

			}

		}
	}

}

// 给图像画一个黑框
static void image_draw_rectan(uint8(*image)[image_w])
{
	// printf("预处理--给图像画一个黑框\n");
	for (int i = 0; i < image_h; i++)
	{
		image[i][0] = 0;
		image[i][1] = 0;
		image[i][image_w - 1] = 0;
		image[i][image_w - 2] = 0;

	}
	for (int i = 0; i < image_w; i++)
	{
		image[0][i] = 0;
		image[1][i] = 0;
		//image[image_h-1][i] = 0;

	}
	// printf("预处理完成\n");
}

static Mat Array_to_Mat(uint8(*image)[image_w])
{
	Mat img(image_h, image_w, CV_8UC1, Scalar(0));
	for (int i = 0; i < image_h; i++)
	{
		for (int j = 0; j < image_w; j++)
		{
			img.at<uchar>(i, j) = image[i][j];
		}
	}
	return img;

}

static void myImshow(const String& winname, InputArray mat)
{
	imshow(winname, mat);
}

static Mat er_zhi(Mat imgxiu) 
{
	Mat imgTemp, imgCvt, imgThr;
	cvtColor(imgxiu, imgCvt, COLOR_BGR2GRAY);
	GaussianBlur(imgCvt, imgTemp, Size(5, 5), 0);
	threshold(imgTemp, imgThr, 0, 255, THRESH_BINARY + THRESH_OTSU);
	imshow("二值化", imgThr);
	return imgThr;
}


static void fitLineAndCalculateAngle(const std::vector<cv::Point>& points, double& angle, double& rSquared) {
    if (points.empty()) {
        std::cerr << "点集为空！" << std::endl;
        return;
    }

    // 提取 y 和 x 坐标
    std::vector<double> yCoords, xCoords;
    for (const auto& point : points) {
        yCoords.push_back(point.y);
        xCoords.push_back(point.x);
    }

    // 计算必要的和
    double sumY = 0, sumX = 0, sumXY = 0, sumY2 = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        sumY += yCoords[i];
        sumX += xCoords[i];
        sumXY += yCoords[i] * xCoords[i];
        sumY2 += yCoords[i] * yCoords[i];
    }

    // 计算斜率 k 和截距 b
    double n = points.size();
    double k = (n * sumXY - sumY * sumX) / (n * sumY2 - sumY * sumY);
    double b = (sumX - k * sumY) / n;

    // 计算拟合度 R^2
    double meanX = sumX / n;
    double ssTot = 0, ssRes = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        double predictedX = k * yCoords[i] + b;
        ssTot += (xCoords[i] - meanX) * (xCoords[i] - meanX);
        ssRes += (xCoords[i] - predictedX) * (xCoords[i] - predictedX);
    }
    rSquared = 1 - (ssRes / ssTot);

    // 计算中线与垂直方向的夹角
    angle = std::atan(k) * 180 / CV_PI; // 转换为角度
}

static int mapDoubleToInt(double value, double fromLow, double fromHigh, int toLow, int toHigh) {
    // 确保输入值在原始范围内
    if (value < fromLow) {
        value = fromLow;
    } else if (value > fromHigh) {
        value = fromHigh;
    }

    // 计算映射后的值
    double scale = static_cast<double>(toHigh - toLow) / (fromHigh - fromLow);
    int result = static_cast<int>(toLow + (value - fromLow) * scale);

    return result;
}

static void fitLineAndCalculateAngle(const std::vector<cv::Point>& points, double& angle, double& rSquared) 
{
    if (points.empty() || points.size() == 1) {
        std::cerr << "点集数据不足，无法拟合直线！" << std::endl;
        return;
    }

    // 检查是否所有点的 y 坐标相同
    bool allYSame = true;
    double firstY = points[0].y;
    for (const auto& point : points) {
        if (point.y != firstY) {
            allYSame = false;
            break;
        }
    }
    if (allYSame) {
        angle = 0.0;
        rSquared = 1.0; // 完全拟合
        return;
    }

    // 检查是否所有点的 x 坐标相同
    bool allXSame = true;
    double firstX = points[0].x;
    for (const auto& point : points) {
        if (point.x != firstX) {
            allXSame = false;
            break;
        }
    }
    if (allXSame) {
        angle = 90.0;
        rSquared = 1.0; // 完全拟合
        return;
    }

    // 提取 y 和 x 坐标
    std::vector<double> yCoords, xCoords;
    for (const auto& point : points) {
        yCoords.push_back(point.y);
        xCoords.push_back(point.x);
    }

    // 计算必要的和
    double sumY = 0, sumX = 0, sumXY = 0, sumY2 = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        sumY += yCoords[i];
        sumX += xCoords[i];
        sumXY += yCoords[i] * xCoords[i];
        sumY2 += yCoords[i] * yCoords[i];
    }

    // 计算斜率 k 和截距 b
    double n = points.size();
    double denominator = (n * sumY2 - sumY * sumY);
    if (denominator == 0) {
        std::cerr << "无法计算斜率，分母为 0！" << std::endl;
        return;
    }
    double k = (n * sumXY - sumY * sumX) / denominator;
    double b = (sumX - k * sumY) / n;

    // 计算拟合度 R^2
    double meanX = sumX / n;
    double ssTot = 0, ssRes = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        double predictedX = k * yCoords[i] + b;
        ssTot += (xCoords[i] - meanX) * (xCoords[i] - meanX);
        ssRes += (xCoords[i] - predictedX) * (xCoords[i] - predictedX);
    }
    if (ssTot == 0) {
        rSquared = 1.0; // 完全拟合
    } else {
        rSquared = 1 - (ssRes / ssTot);
    }

    // 计算中线与垂直方向的夹角
    angle = std::atan(k) * 180 / CV_PI; // 转换为角度
}

// 将 320x240 的图像转换为 160x120 的图像
cv::Mat resizeImage(const cv::Mat& inputImage) 
{
    // 检查输入图像的尺寸是否为 320x240
    if (inputImage.cols != 320 || inputImage.rows != 240) {
        std::cerr << "输入图像的尺寸不是 320x240！" << std::endl;
        return cv::Mat(); // 返回空矩阵
    }

    // 创建目标图像（160x120）
    cv::Mat resizedImage;

    // 调整图像尺寸
    cv::resize(inputImage, resizedImage, cv::Size(160, 120), 0, 0, cv::INTER_LINEAR);

    return resizedImage;
}

// 计算每两个相邻点的变化率（用于近似计算曲率）
vector<float> calculateCurvature(const vector<Point>& points) {
    vector<float> curvature(points.size(), 0.0f);
    
    for (size_t i = 1; i < points.size() - 1; ++i) {
        float deltaX1 = points[i].x - points[i - 1].x;
        float deltaX2 = points[i + 1].x - points[i].x;
        float deltaY1 = points[i].y - points[i - 1].y;
        float deltaY2 = points[i + 1].y - points[i].y;
        
        // 计算简单的曲率：曲率 ≈ (deltaX2 - deltaX1) / (deltaY2 - deltaY1)
        float curvatureX = deltaX2 - deltaX1;
        float curvatureY = deltaY2 - deltaY1;

        // 简单的曲率计算方式，基于 X 和 Y 方向的变化量
        curvature[i] = sqrt(curvatureX * curvatureX + curvatureY * curvatureY);
    }
    return curvature;
}

// 计算舵机角度
float calculateServoAngle(const vector<float>& curvature, float baseAngle) {
    float totalCurvature = 0.0f;
    for (float k : curvature) {
        totalCurvature += k;  // 累积曲率的绝对值
    }
    
    float avgCurvature = totalCurvature / curvature.size();
    
    // 映射曲率到舵机角度范围，这里可以根据实际情况调整最大曲率（例如 5.0）和角度范围
    float maxCurvature = 5.0f; // 假设最大曲率
    float angleAdjustment = avgCurvature / maxCurvature * 30.0f;  // 最大偏离 30 度
    
    // 将舵机角度控制在合理范围内
    float servoAngle = baseAngle + angleAdjustment;
    servoAngle = std::max(0.0f, std::min(90.0f, servoAngle));  // 限制舵机角度在 0 到 90 度之间
    
    return servoAngle;
}

void servodegree(float degree) 
{
    cout << "Servo angle: " << degree << " degrees" << endl;
}

*/

