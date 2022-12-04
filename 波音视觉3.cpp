#include<iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct imgSize//创建一个结构体“尺寸”包含图像的宽和高
{
	double w;
	double h;
};

Mat Pretreatment(Mat img)//图像预处理
{
	Mat grey, gaus, can, ker, dil;
	cvtColor(img, grey, COLOR_BGR2GRAY);//转换成灰度图
	GaussianBlur(grey, gaus, Size(3, 3), 0);//高斯滤波
	//Canny的阈值需根据图片调整，题目中的卡片最适阈值为300，600；
	//文件最适阈值为100，200；调用摄像头时一般为50，100.
	Canny(gaus, can, 100, 200);//边缘检测
	ker = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(can, dil, ker);//膨胀
	return dil;
}

vector<vector<Point>> getContours(Mat img)//获取角点
{
	vector<vector<Point>> original_contours, final_contours;
	findContours(img, original_contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//获取轮廓
	for (int i = 0; i < original_contours.size(); i++)//筛选轮廓
	{
		double area = contourArea(original_contours[i]);
		if (area > 3000)//通过面积大小筛选
		{
			vector<Point>temppoint;
			double length = arcLength(original_contours[i], true);
			approxPolyDP(original_contours[i], temppoint, 0.02 * length, true);
			if (temppoint.size() == 4)//筛选出逼近轮廓为四边形的轮廓
			{
				final_contours.push_back(temppoint);
			}
		}
	}
	return final_contours;
}

void draw(Mat img, vector<vector<Point>>contours)//框出文件
{
	namedWindow("检测文件", WINDOW_FREERATIO);
	drawContours(img, contours, -1, Scalar(0, 0, 255), 8);//框出检测出的轮廓
	for (int i = 0; i < contours.size(); i++)
	{
		putText(img, "target"+to_string(i + 1), contours[i][0], FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 4);
	}
	imshow("检测文件", img);
}

vector<vector<Point>> reorder(vector<vector<Point>> point)//角点重新排序
{
	vector<vector<Point>> final_Point;
	final_Point = point;
	for (int a = 0; a < final_Point.size(); a++)
	{
		for (int i = 0; i < 3; i++)//利用冒泡排序使角点按y坐标大小排序
		{
			for (int j = 0; j < 3 - i; j++)
			{
				if (final_Point[a][j].y > final_Point[a][j + 1].y)
				{
					Point tempPoint;
					tempPoint = final_Point[a][j];
					final_Point[a][j] = final_Point[a][j + 1];
					final_Point[a][j + 1] = tempPoint;
				}
			}
		}
		if (final_Point[a][0].x > final_Point[a][1].x)//比较相邻的x坐标完成排序
		{
			Point tempPoint;
			tempPoint = final_Point[a][0];
			final_Point[a][0] = final_Point[a][1];
			final_Point[a][1] = tempPoint;
		}
		if (final_Point[a][2].x > final_Point[a][3].x)
		{
			Point tempPoint;
			tempPoint = final_Point[a][2];
			final_Point[a][2] = final_Point[a][3];
			final_Point[a][3] = tempPoint;
		}
	}
	return final_Point;
}

imgSize getSize(vector<vector<Point>>contours)//获取文件的尺寸,可以按文件情况选择尺寸
{
	imgSize size;

	//扑克牌尺寸
	size.w = 285;
	size.h = 440;

	//A4纸尺寸
	/*size.w = 210*2;
	size.h = 297*2;*/

	//A3纸尺寸
	//size.w=297*2;
	//size.h=420*2;

	//身份证、校卡尺寸
	//size.w = 428;
	//size.h = 270;

	//根据原图检测尺寸（可能不够精确）
	//double arrw,arrh;
	//arrw = 0;
	//arrh = 0;
	//for (int i = 0; i < contours.size(); i++)//计算所有轮廓的宽取平均值
	//{
	//	double w1, w2, wi;
	//	w1 = sqrt((contours[i][0].x - contours[i][1].x) * (contours[i][0].x - contours[i][1].x) + (contours[i][0].y - contours[i][1].y) * (contours[i][0].y - contours[i][1].y));
	//	w2 = sqrt((contours[i][2].x - contours[i][3].x) * (contours[i][2].x - contours[i][3].x) + (contours[i][2].y - contours[i][3].y) * (contours[i][2].y - contours[i][3].y));
	//	wi = (w1 + w2) / 2;
	//	arrw = arrw + wi;
	//}
	//for (int i = 0; i < contours.size(); i++)//计算所有轮廓的高取平均值
	//{
	//	double h1, h2, hi;
	//	h1 = sqrt((contours[i][0].x - contours[i][2].x) * (contours[i][0].x - contours[i][2].x) + (contours[i][0].y - contours[i][2].y) * (contours[i][0].y - contours[i][2].y));
	//	h2 = sqrt((contours[i][1].x - contours[i][3].x) * (contours[i][1].x - contours[i][3].x) + (contours[i][1].y - contours[i][3].y) * (contours[i][1].y - contours[i][3].y));
	//	hi = (h1 + h2) / 2;
	//	arrh = arrh + hi;
	//}
	//size.w = arrw / contours.size();
	//size.h = arrh / contours.size();

	return size;
}

Mat warp(vector<Point> point, Mat img, double w, double h)//透视变换
{
	Mat final_img, rotation;
	Point2f start_point[4] = { point[0],point[1] ,point[2] ,point[3] };
	Point2f final_point[4] = { Point2f(0.0,0.0),Point2f(w,0.0), Point2f(0.0,h), Point2f(w,h) };
	rotation = getPerspectiveTransform(start_point, final_point);
	warpPerspective(img, final_img, rotation, Point(w, h));
	return final_img;
}

//使用摄像头则使用该代码
int main()
{
	VideoCapture vid(0);
	if (!vid.isOpened())//判断摄像头是否连接成功
	{
		cout << "摄像头连接失败" << endl;
		return -1;
	}
	while (true)
	{
		//显示帧率
		namedWindow("FPS");
		Mat m = Mat::zeros(Size(360, 240), CV_8UC3);//创建图像
		m = Scalar(255, 255, 255);
		double fps;
		double t = getTickCount();
		if (waitKey(1) == 13) 
		{
			break;
		}
		fps = 1.0 /((getTickCount()-t) / getTickFrequency());
		putText(m, "FPS:"+to_string(fps), Point2f(0, 120), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 0), 2);
		imshow("FPS", m);
		//处理摄像头获取的信息
		Mat img, pret, draw_img;
		vector<vector<Point>>original_contours, final_contours;
		imgSize size;
		vector<Mat> final_img;
		namedWindow("原图", WINDOW_FREERATIO);
		namedWindow("寻找阈值", WINDOW_FREERATIO);
		vid >> img;
		if (img.empty())//判断是否获取图像
		{
			break;
		}
		img.copyTo(draw_img);
		pret = Pretreatment(img);//图像预处理，对应函数位于13~24
		original_contours = getContours(pret);//获取初始轮廓，对应函数位于26~45
		draw(draw_img, original_contours);//框出并标记轮廓，对应函数位于47~56
		final_contours = reorder(original_contours);//角点重新排序，对应函数位于58~93
		size = getSize(final_contours);//获取图像尺寸，对应函数位于95~139
		for (int i = 0; i < final_contours.size(); i++)	//提取目标文件（支持多个）
		{
			Mat tempimg;
			namedWindow("文件" + to_string(i + 1), WINDOW_FREERATIO);
			tempimg = warp(final_contours[i], img, size.w, size.h); //透视变换，对应函数位于141~149
			final_img.push_back(tempimg);
			imshow("文件" + to_string(i + 1), tempimg);
		}
		imshow("原图", img);
		imshow("寻找阈值", pret);//给出原图预处理效果，方便确定最适阈值
		if (waitKey(1) == 13)//保存获取的文件（支持多个）
		{
			for (int j = 0; j < final_img.size(); j++)
			{
				imwrite("D:/study/波音/result" + to_string(j + 1) + ".png", final_img[j]);
			}
			break;
		}
	}
		waitKey(0);
		destroyAllWindows();
		return 0;
}

//读取图片则使用该代码
//int main()
//{
//	Mat img, pret, draw_img;
//	vector<vector<Point>>original_contours, final_contours;
//	imgSize size;
// 	vector<Mat> final_img;
//	namedWindow("原图", WINDOW_FREERATIO);
//	namedWindow("寻找阈值", WINDOW_FREERATIO);
//	img = imread("d:/study/波音/波音技术部考核题目/波音视觉题材料/cards.jpg");//读取图片
//	img.copyTo(draw_img);
//	pret = Pretreatment(img);
//	original_contours = getContours(pret);
//	draw(draw_img, original_contours);
//	final_contours = reorder(original_contours);
//	size = getSize(final_contours);
//	for (int i = 0; i < final_contours.size(); i++)
//	{
//		Mat tempimg;
//		namedWindow("文件" + to_string(i + 1), WINDOW_FREERATIO);
//		tempimg = warp(final_contours[i], img, size.w, size.h);
//		final_img.push_back(tempimg);
//		imshow("文件" + to_string(i + 1), tempimg);
//	}
//	imshow("原图", img);
//	imshow("寻找阈值", pret);
//	for (int j = 0; j < final_img.size(); j++)
//	{
//		imwrite("d:/study/波音/result" + to_string(j + 1) + ".png", final_img[j]);
//	}
//	waitKey(0);
//	destroyAllWindows();
//	return 0;
//}