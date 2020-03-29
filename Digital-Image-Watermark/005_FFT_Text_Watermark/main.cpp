#include <iostream>
#include <fstream>
#include <vector>
#ifdef __APPLE__
        #include <sys/uio.h>
#else
        #include <sys/io.h>
#endif
#include "opencv2/opencv.hpp"
#define CV_MINMAX   32

// #include "Watermark_Text.h"

using namespace std;
using namespace cv;


////
class WaterMarkText
{
public:

	/******************************
	��������ˮӡ������������������ͨ��ͼ��
	input: ���뵥ͨ��ͼ��
	text: ����ˮӡ����
	*******************************/
	cv::Mat addTextWatermarkSingleChannel(const cv::Mat &input, const std::string &text);

	/******************************
	��������ˮӡ������������������ɫͼ��
	input: �����ɫͼ��
	text: ����ˮӡ����
	*******************************/
	cv::Mat addTextWatermarkColorImage(const cv::Mat &inputImage, const std::string &text);

	/******************************
	��ȡ������ˮӡ���������Ҷ�ͼ��
	input: ����ͼ��
	*******************************/
	cv::Mat getWaterMarkSingleChannel(const cv::Mat &input);

	/******************************
	��ȡ������ˮӡ����������ɫͼ��
	input: ����ͼ��
	*******************************/
	cv::Mat getWatermarkColorImage(const cv::Mat &input);

	/******************************
	ͼ��ת��
	input: ����ͼ��
	*******************************/
	cv::Mat getTransposeImage(const cv::Mat &input);

private:

};

cv::Mat WaterMarkText::addTextWatermarkSingleChannel(const cv::Mat &input, const std::string &text)
{
	// 根据照片大小设置水印大小宽度
	double textSize = 0.0;
	int textWidth = 0;

	int minImgSize = input.rows > input.cols ? input.cols : input.rows;

	if (minImgSize < 150)
	{
		textSize = 1.0;
		textWidth = 1;
	}
	else if (minImgSize >= 150 && minImgSize < 300)
	{
		textSize = 1.5;
		textWidth = 2;
	}
	else if (minImgSize >= 300 && minImgSize < 400)
	{
		textSize = 2.5;
		textWidth = 3;
	}
	else if (minImgSize >= 400 && minImgSize < 650)
	{
		textSize = 3.0;
		textWidth = 3;
	}
	else if (minImgSize >= 650 && minImgSize < 1000)
	{
		textSize = 4.0;
		textWidth = 4;
	}
	else if (minImgSize >= 1000)
	{
		textSize = 4.5;;
		textWidth = 5;
	}

	int m = cv::getOptimalDFTSize(input.rows);
	int n = cv::getOptimalDFTSize(input.cols);

	cv::Mat dst;
	copyMakeBorder(input, dst, 0, m - input.rows, 0, n - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));


	cv::Mat planes[] = { cv::Mat_<float>(dst), cv::Mat::zeros(dst.size(), CV_32F) };
	cv::Mat complete;//二通道：实部+虚部
	merge(planes, 2, complete);
	dft(complete, complete);

	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;
	cv::minMaxIdx(complete, minp, maxp);
	std::cout << minv << "  " << maxv << std::endl;

	// 添加水印文字————中心对称
	int meanvalue = cv::mean(complete)[0], num;
	std::cout << meanvalue << std::endl;
	if (meanvalue > 128)
	{
		num = -log(abs(minv));
	}
	else
	{
		num = log(abs(maxv));
	}
	// FONT_HERSHEY_DUPLEX
	// FONT_HERSHEY_TRIPLEX 너무 연함
	putText(complete, text, cv::Point(input.cols * 0.2222, input.rows * 0.2222),
		cv::FONT_HERSHEY_SIMPLEX, textSize, cv::Scalar(num, num, num), textWidth);
	flip(complete, complete, -1); 
	putText(complete, text, cv::Point(input.cols * 0.2222, input.rows * 0.2222),
		cv::FONT_HERSHEY_SIMPLEX, textSize, cv::Scalar(num, num, num), textWidth);
	flip(complete, complete, -1);

	// 傅里叶逆变换
	idft(complete, complete);

	split(complete, planes);

	magnitude(planes[0], planes[1], planes[0]);
	cv::Mat result = planes[0];
	result = result(cv::Rect(0, 0, input.cols, input.rows));
	// 在进行归一化之前各点的像素值都特别大，归一化之后转换到【0,1】
	normalize(result, result, 0, 1, CV_MINMAX);

	return result;
}

/**************************************************
Author: Yuxfei 
Date: 2017-03-14 
Description: 添加文字水印函数————处理彩色图像
Parameter:
Mat input:输入彩色图像
string text:添加水印文字
***************************************************/
cv::Mat WaterMarkText::addTextWatermarkColorImage(const cv::Mat &inputImage, const std::string &text)
{
	int col = inputImage.cols;
	int row = inputImage.rows;
	cv::Mat resultshow;

	if (row > col)//行大于列
	{
		cv::Mat temp1 = getTransposeImage(inputImage);
		cv::Mat channelsrc[3], channelresult[3];
		split(temp1, channelsrc);
		channelresult[0] = addTextWatermarkSingleChannel(channelsrc[0], text);
		channelresult[1] = addTextWatermarkSingleChannel(channelsrc[1], text);
		channelresult[2] = addTextWatermarkSingleChannel(channelsrc[2], text);
		cv::Mat temp2;
		merge(channelresult, 3, temp2);
		resultshow = getTransposeImage(temp2);
	}
	else
	{
		cv::Mat channelsrc[3], channelresult[3];
		split(inputImage, channelsrc);
		channelresult[0] = addTextWatermarkSingleChannel(channelsrc[0], text);
		channelresult[1] = addTextWatermarkSingleChannel(channelsrc[1], text);
		channelresult[2] = addTextWatermarkSingleChannel(channelsrc[2], text);
		merge(channelresult, 3, resultshow);
	}

	return resultshow;
}

/************************************************** 
Author: Yuxfei 
Date: 2017-03-14 
Description: 提取出文字水印——灰度图像（单通道）
Parameter:
Mat input: 输入图像
***************************************************/
cv::Mat WaterMarkText::getWaterMarkSingleChannel(const cv::Mat &input)
{
	int m = cv::getOptimalDFTSize(input.rows);
	int n = cv::getOptimalDFTSize(input.cols);

	cv::Mat dst;
	copyMakeBorder(input, dst, 0, m - input.rows, 0, n - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	cv::Mat planes[] = { cv::Mat_<float>(dst), cv::Mat::zeros(dst.size(), CV_32F) };
	cv::Mat complete;
	merge(planes, 2, complete);

	dft(complete, complete);

	split(complete, planes);
	magnitude(planes[0], planes[1], planes[0]);// 过程不可逆
	cv::Mat result = planes[0];
	result += 1;
	log(result, result);
	result = result(cv::Rect(0, 0, input.cols, input.rows));
	normalize(result, result, 0, 1, CV_MINMAX);

	// 中心化——将傅里叶分解后的图像的低频部分移至图像中心
	int cx = result.cols / 2;
	int cy = result.rows / 2;
	cv::Mat temp;
	cv::Mat q0(result, cv::Rect(0, 0, cx, cy));
	cv::Mat q1(result, cv::Rect(cx, 0, cx, cy));
	cv::Mat q2(result, cv::Rect(0, cy, cx, cy));
	cv::Mat q3(result, cv::Rect(cx, cy, cx, cy));
	q0.copyTo(temp);
	q3.copyTo(q0);
	temp.copyTo(q3);
	q1.copyTo(temp);
	q2.copyTo(q1);
	temp.copyTo(q2);

	return result;
}

/***************************************************
Author: Yuxfei 
Date: 2017-03-14 
Description: 提取出文字水印——彩色图像（多通道）
Parameter:
Mat input:输入图像
****************************************************/
cv::Mat WaterMarkText::getWatermarkColorImage(const cv::Mat &input)
{
	cv::Mat channelsrc[3], channelresult[3];
	split(input, channelsrc);

	channelresult[0] = getWaterMarkSingleChannel(channelsrc[0]);
	channelresult[1] = getWaterMarkSingleChannel(channelsrc[1]);
	channelresult[2] = getWaterMarkSingleChannel(channelsrc[2]);

	cv::Mat resultshow;
	merge(channelresult, 3, resultshow);

	return resultshow;
}

/***************************************************
Author: Yuxfei 
Date: 2017-03-16 
Description: 图像转置（转置两次就还原）
Parameter:
Mat input:输入图像
****************************************************/
cv::Mat WaterMarkText::getTransposeImage(const cv::Mat &input)
{
	cv::Mat result;
	transpose(input, result);
	return result;
}


////

int main(int argc, char** argv)
{
	WaterMarkText wmt;

	const char* env_input = std::getenv("INPUT");
	const char* env_ifft  = std::getenv("IFFT_OUPUT");
	const char* env_text  = std::getenv("TEXT"); // "CS SWJTU"

	const char* env_result_spatial = std::getenv("RESULT_SPATIAL"); // "./image/resultsave-1.jpg"
	const char* env_result_fft     = std::getenv("RESULT_FFT"); // "./image/testImgFly1.jpg"
    // if() {
    //     std::cout << "Your INPUT is: " << env_p << '\n';
	// }

	// ԭʼͼ��
	Mat img = imread(env_input);
	// imshow("ԭʼͼ��", img);

	// ԭʼͼ����Ҷ�ֽ�
	Mat imgFly = wmt.getWatermarkColorImage(img); // fft
	// imshow("ԭʼ��Ƭ����Ҷ�ֽ�", imgFly);
	imwrite(env_ifft, imgFly * 255); // 원본 이미지의 fft 저장

	// ԭʼͼ������ˮӡ
	Mat result = wmt.addTextWatermarkColorImage(img, env_text);
	// imshow("����ˮӡ��ͼ��", result);  // 이미지에 txt넣은 화면 spatial
	// ˮӡʵ��һ
	Mat resultsave = result * 255; // ����֮ǰ����255  /// spatial데이터에 255곱함
	imwrite(env_result_spatial, resultsave); /// spatial데이터 저장
	Mat imgWatermark1 = imread(env_result_spatial);  // spatial 불러옴
	Mat imgFly1 = wmt.getWatermarkColorImage(imgWatermark1);  // spatial 이미지로 부터 fft 
	// imshow("ʵ��һˮӡ��ȡ", imgFly1); // freq 표시
	imwrite(env_result_fft, imgFly1 * 255); // freq 이미지 저장

	// cv::waitKey(0);
	return 0;
}