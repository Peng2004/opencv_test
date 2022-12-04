#include<iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat preProcessing(Mat img)
{
	Mat  imgGray, imgBlur, imgCanny, imgDil;
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 50, 100);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);
	return imgDil;
}

vector<Point2f> getContours(Mat imgDil)
{

	vector<vector<Point>> contours; //轮廓数据
	vector<Vec4i> hierarchy;
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//通过预处理的二值图像找到所有轮廓contours
	vector<vector<Point>> conPoly(contours.size());
	vector<Point2f> biggest;
	int maxArea = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]); //计算每个轮廓区域
		cout << area << endl;
		if (area > 1000) //过滤噪声
		{//找轮廓的近似多边形或曲线
			double peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			if (area > maxArea && conPoly[i].size() == 4)
			{
				biggest = { conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3] };
				maxArea = area;
			}
		}
	}
	return biggest; //返回最大轮廓四个点的坐标
}

Rect gettarget(vector<Point2f> points)
{
	int a, b, w, h;
	w = sqrt((points[0].x - points[1].x) * (points[0].x - points[1].x)
		+ (points[0].y - points[1].y) * (points[0].y - points[1].y));
	h = sqrt((points[0].x - points[2].x) * (points[0].x - points[2].x)
		+ (points[0].y - points[2].y) * (points[0].y - points[2].y));
	a = points[0].x;
	b = points[0].y;
	Rect target(a, b, w, h);
	return target;
}

vector<Point2f> reorder(vector<Point2f>point)//角点重新排序
{
	vector<Point2f> final_Point;
	final_Point = point;
	for (int i = 0; i < 3; i++)//利用冒泡排序使角点按y坐标大小排序
	{
		for (int j = 0; j < 3 - i; j++)
		{
			if (final_Point[j].y > final_Point[j + 1].y)
			{
				Point tempPoint;
				tempPoint = final_Point[j];
				final_Point[j] = final_Point[j + 1];
				final_Point[j + 1] = tempPoint;
			}
		}
	}
	if (final_Point[0].x > final_Point[1].x)//比较相邻的x坐标完成排序
	{
		Point tempPoint;
		tempPoint = final_Point[0];
		final_Point[0] = final_Point[1];
		final_Point[1] = tempPoint;
	}
	if (final_Point[2].x > final_Point[3].x)
	{
		Point tempPoint;
		tempPoint = final_Point[2];
		final_Point[2] = final_Point[3];
		final_Point[3] = tempPoint;
	}
	return final_Point;
}

int main()
{
	VideoCapture vid(1);
	if (!vid.isOpened())
	{
		cout << "打开摄像头失败";
		return -1;
	}
	int trackobject = 0;
	int hsize = 8;
	float Range[] = { 0,360 };
	const float* range = Range;
	Mat hsv, hue, hist, backproj, preimg, preimg1, Img1;
	vector<Point2f> points, points1;
	vid >> Img1;
	preimg1 = preProcessing(Img1);
	points1 = getContours(preimg1);
	if (points1.empty())
	{
		cout << "没有获取到轮廓" << endl;
		imshow("pre", preimg1);
		//return -1;
	}
	points1 = reorder(points1);
	Rect target1;
	target1 = gettarget(points1);
	//Rect targetCam = target1;
	int F, w;
	w = target1.width;
	F = w * 37 / 8;
	int d = 37;
	//Rect target = selectROI("选择目标区域", img, 1, 0);
	while (1)
	{
		vid >> Img1;
		preimg = preProcessing(Img1);
		points = getContours(preimg);
		if (points.empty());
		else
		{
			points1 = reorder(points);
			target1 = gettarget(points1);
			d = 8 * F / target1.width;
		}
		cvtColor(Img1, hsv, COLOR_BGR2HSV);
		int ch[] = { 0,0 };
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);
		if (trackobject <= 0)
		{
			Mat roi = hue(target1);
			calcHist(&roi, 1, 0, roi, hist, 1, &hsize, &range);
			normalize(hist, hist, 0, 255, NORM_MINMAX);
			trackobject = 1;
		}
		calcBackProject(&hue, 1, 0, hist, backproj, &range);
		Mat imgCam;
		Img1.copyTo(imgCam);
		meanShift(backproj, target1, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
		rectangle(Img1, target1, Scalar(0, 0, 255), 4, LINE_AA);
		putText(Img1, to_string(d) + "cm", target1.tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 4);
		imshow("test1", Img1);
		imshow("pre", preimg1);
		char c = cv::waitKey(1);
		if (c == 27)
		{
			break;
		}
	}
	cv::waitKey(0);
	cv::destroyAllWindows;
	return 0;
}