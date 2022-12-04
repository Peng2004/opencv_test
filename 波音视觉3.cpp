#include<iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct imgSize//����һ���ṹ�塰�ߴ硱����ͼ��Ŀ�͸�
{
	double w;
	double h;
};

Mat Pretreatment(Mat img)//ͼ��Ԥ����
{
	Mat grey, gaus, can, ker, dil;
	cvtColor(img, grey, COLOR_BGR2GRAY);//ת���ɻҶ�ͼ
	GaussianBlur(grey, gaus, Size(3, 3), 0);//��˹�˲�
	//Canny����ֵ�����ͼƬ��������Ŀ�еĿ�Ƭ������ֵΪ300��600��
	//�ļ�������ֵΪ100��200����������ͷʱһ��Ϊ50��100.
	Canny(gaus, can, 100, 200);//��Ե���
	ker = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(can, dil, ker);//����
	return dil;
}

vector<vector<Point>> getContours(Mat img)//��ȡ�ǵ�
{
	vector<vector<Point>> original_contours, final_contours;
	findContours(img, original_contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//��ȡ����
	for (int i = 0; i < original_contours.size(); i++)//ɸѡ����
	{
		double area = contourArea(original_contours[i]);
		if (area > 3000)//ͨ�������Сɸѡ
		{
			vector<Point>temppoint;
			double length = arcLength(original_contours[i], true);
			approxPolyDP(original_contours[i], temppoint, 0.02 * length, true);
			if (temppoint.size() == 4)//ɸѡ���ƽ�����Ϊ�ı��ε�����
			{
				final_contours.push_back(temppoint);
			}
		}
	}
	return final_contours;
}

void draw(Mat img, vector<vector<Point>>contours)//����ļ�
{
	namedWindow("����ļ�", WINDOW_FREERATIO);
	drawContours(img, contours, -1, Scalar(0, 0, 255), 8);//�������������
	for (int i = 0; i < contours.size(); i++)
	{
		putText(img, "target"+to_string(i + 1), contours[i][0], FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 4);
	}
	imshow("����ļ�", img);
}

vector<vector<Point>> reorder(vector<vector<Point>> point)//�ǵ���������
{
	vector<vector<Point>> final_Point;
	final_Point = point;
	for (int a = 0; a < final_Point.size(); a++)
	{
		for (int i = 0; i < 3; i++)//����ð������ʹ�ǵ㰴y�����С����
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
		if (final_Point[a][0].x > final_Point[a][1].x)//�Ƚ����ڵ�x�����������
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

imgSize getSize(vector<vector<Point>>contours)//��ȡ�ļ��ĳߴ�,���԰��ļ����ѡ��ߴ�
{
	imgSize size;

	//�˿��Ƴߴ�
	size.w = 285;
	size.h = 440;

	//A4ֽ�ߴ�
	/*size.w = 210*2;
	size.h = 297*2;*/

	//A3ֽ�ߴ�
	//size.w=297*2;
	//size.h=420*2;

	//���֤��У���ߴ�
	//size.w = 428;
	//size.h = 270;

	//����ԭͼ���ߴ磨���ܲ�����ȷ��
	//double arrw,arrh;
	//arrw = 0;
	//arrh = 0;
	//for (int i = 0; i < contours.size(); i++)//�������������Ŀ�ȡƽ��ֵ
	//{
	//	double w1, w2, wi;
	//	w1 = sqrt((contours[i][0].x - contours[i][1].x) * (contours[i][0].x - contours[i][1].x) + (contours[i][0].y - contours[i][1].y) * (contours[i][0].y - contours[i][1].y));
	//	w2 = sqrt((contours[i][2].x - contours[i][3].x) * (contours[i][2].x - contours[i][3].x) + (contours[i][2].y - contours[i][3].y) * (contours[i][2].y - contours[i][3].y));
	//	wi = (w1 + w2) / 2;
	//	arrw = arrw + wi;
	//}
	//for (int i = 0; i < contours.size(); i++)//�������������ĸ�ȡƽ��ֵ
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

Mat warp(vector<Point> point, Mat img, double w, double h)//͸�ӱ任
{
	Mat final_img, rotation;
	Point2f start_point[4] = { point[0],point[1] ,point[2] ,point[3] };
	Point2f final_point[4] = { Point2f(0.0,0.0),Point2f(w,0.0), Point2f(0.0,h), Point2f(w,h) };
	rotation = getPerspectiveTransform(start_point, final_point);
	warpPerspective(img, final_img, rotation, Point(w, h));
	return final_img;
}

//ʹ������ͷ��ʹ�øô���
int main()
{
	VideoCapture vid(0);
	if (!vid.isOpened())//�ж�����ͷ�Ƿ����ӳɹ�
	{
		cout << "����ͷ����ʧ��" << endl;
		return -1;
	}
	while (true)
	{
		//��ʾ֡��
		namedWindow("FPS");
		Mat m = Mat::zeros(Size(360, 240), CV_8UC3);//����ͼ��
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
		//��������ͷ��ȡ����Ϣ
		Mat img, pret, draw_img;
		vector<vector<Point>>original_contours, final_contours;
		imgSize size;
		vector<Mat> final_img;
		namedWindow("ԭͼ", WINDOW_FREERATIO);
		namedWindow("Ѱ����ֵ", WINDOW_FREERATIO);
		vid >> img;
		if (img.empty())//�ж��Ƿ��ȡͼ��
		{
			break;
		}
		img.copyTo(draw_img);
		pret = Pretreatment(img);//ͼ��Ԥ������Ӧ����λ��13~24
		original_contours = getContours(pret);//��ȡ��ʼ��������Ӧ����λ��26~45
		draw(draw_img, original_contours);//����������������Ӧ����λ��47~56
		final_contours = reorder(original_contours);//�ǵ��������򣬶�Ӧ����λ��58~93
		size = getSize(final_contours);//��ȡͼ��ߴ磬��Ӧ����λ��95~139
		for (int i = 0; i < final_contours.size(); i++)	//��ȡĿ���ļ���֧�ֶ����
		{
			Mat tempimg;
			namedWindow("�ļ�" + to_string(i + 1), WINDOW_FREERATIO);
			tempimg = warp(final_contours[i], img, size.w, size.h); //͸�ӱ任����Ӧ����λ��141~149
			final_img.push_back(tempimg);
			imshow("�ļ�" + to_string(i + 1), tempimg);
		}
		imshow("ԭͼ", img);
		imshow("Ѱ����ֵ", pret);//����ԭͼԤ����Ч��������ȷ��������ֵ
		if (waitKey(1) == 13)//�����ȡ���ļ���֧�ֶ����
		{
			for (int j = 0; j < final_img.size(); j++)
			{
				imwrite("D:/study/����/result" + to_string(j + 1) + ".png", final_img[j]);
			}
			break;
		}
	}
		waitKey(0);
		destroyAllWindows();
		return 0;
}

//��ȡͼƬ��ʹ�øô���
//int main()
//{
//	Mat img, pret, draw_img;
//	vector<vector<Point>>original_contours, final_contours;
//	imgSize size;
// 	vector<Mat> final_img;
//	namedWindow("ԭͼ", WINDOW_FREERATIO);
//	namedWindow("Ѱ����ֵ", WINDOW_FREERATIO);
//	img = imread("d:/study/����/����������������Ŀ/�����Ӿ������/cards.jpg");//��ȡͼƬ
//	img.copyTo(draw_img);
//	pret = Pretreatment(img);
//	original_contours = getContours(pret);
//	draw(draw_img, original_contours);
//	final_contours = reorder(original_contours);
//	size = getSize(final_contours);
//	for (int i = 0; i < final_contours.size(); i++)
//	{
//		Mat tempimg;
//		namedWindow("�ļ�" + to_string(i + 1), WINDOW_FREERATIO);
//		tempimg = warp(final_contours[i], img, size.w, size.h);
//		final_img.push_back(tempimg);
//		imshow("�ļ�" + to_string(i + 1), tempimg);
//	}
//	imshow("ԭͼ", img);
//	imshow("Ѱ����ֵ", pret);
//	for (int j = 0; j < final_img.size(); j++)
//	{
//		imwrite("d:/study/����/result" + to_string(j + 1) + ".png", final_img[j]);
//	}
//	waitKey(0);
//	destroyAllWindows();
//	return 0;
//}