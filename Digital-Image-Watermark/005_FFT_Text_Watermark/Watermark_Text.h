#ifndef WATERMARK_TEXT_H
#define WATERMARK_TEXT_H

#include <iostream>
#include "opencv2/opencv.hpp"

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

#endif