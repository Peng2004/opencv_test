# opencv学习笔记
*记录一些用过的函数，届时学了别的再记录，仅是一些函数形式、注意事项以及例子，了解详情记得去翻书捏，~~其实就是懒得打~~*
***



### 一、配置opencv

#### 1.1配置

视图>其他窗口>属性管理器>添加现有属性表>"D:\study\波音\start\opencv\PropertySheet1.props"

***以我自己的文件路径为例***

#### 1.2头文件

`#include<opencv2/opencv.hpp>`

#### 1.3命名空间

`using namespace cv;`

+++



### 二、图像、摄像头、视频的读取显示与保存

#### 2.1图像读取函数：imread   <u>P38</u>

`imread(const String & filename, int flags=IMREAD_COLOR)`

filename:   ***复制文件时的连接符为“\”需要将其改为”/“***

flags：具体列表见《书》P38，默认参数彩色

eg.

~~~c++
imread("d:/study/波音/波音技术部考核题目/波音视觉题材料/cards.jpg");
~~~

#### 2.2图像窗口函数：namedWindow   <u>P39</u>

`namewindow(const String& winname，int flags=WINDOW_AUTOSIZE)`

winname: 别重复命名，会被覆盖

flags：具体列表见《书》P39

eg.

~~~c++
namedWindow("原图", WINDOW_FREERATIO);
~~~

#### 2.3图像显示函数：imshow   <u>P40</u>

`imshow(const String& winname，InputArray mat)`

winname:可以用2.2新建一个窗口显示，若没创建就是`WINDOW_AUTOSIZE`

eg.

~~~c++
imshow("原图", img);
~~~

#### 2.4读取视频：VideoCapture   <u>P40</u>

`VideoCapture(const String& filename,int apiPreference =CAP_ANY)`

用get()可获取视频属性，具体参数列表见《》p41

***需调用摄像头时，在括号内加摄像头序号（从0开始）***

*建议在下面验证摄像头调用是否成功*

eg.

~~~c++
VideoCapture vid(0);
if (!vid.isOpened())
	{
		cout << "摄像头连接失败" << endl;
		return -1;
	}
~~~

#### 2.5数据保存：imwrite <u>P43</u>

`imwrite(const String& filename,InputArray img)`

视频保存为`VideoWriter`函数，见《》P45，具体不详讲
### 三、图像基本操作

#### 3.1颜色模型转换：cvtColor   <u>P56</u>

`cvtColor(InputArray src,OutputArray dst,int code,int dstCn=0)`

code:具体参数见《》P57

eg.

~~~c++
cvtColor(img, grey, COLOR_BGR2GRAY);
~~~

#### 3.2图像二值化：threshold   <u>P71</u>

`threshold(InputArray src,OutputArray dst,double thresh,double maxval,int type)`

type:具体参数见《》P71

**要尤其注意选择设置合适的阈值，如若对图像不了解，设置的阈值不合适，就会对处理后的效果造成严重的影响例如`TermCriteria(TermCriteria::EPS|TermCriteria::COUNT,10,1)`**

#### 3.3图像透视变换：warpPerspective   <u>P88</u>

`warpPerspective(InputArray src,OutputArray dst,InputArray M,Size dsize,int flags=INTER_LINEAR,int borderMode = BORDER_CONSTANT,const Scalar & borderValue = Scalar())`

**提供了getPerspectiveTransform这个函数对4个对应点求取变换尺寸**

eg.

~~~c++
Mat rotation;
rotation = getPerspectiveTransform(start_point, final_point);
warpPerspective(img, final_img, rotation, Point(w, h));
~~~


